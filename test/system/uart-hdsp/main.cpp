/*
author          Oliver Blaser
date            12.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

// using the HDSP Display project (v1.x) for testing UART
// https://github.com/oblaser/HDSP-21xx-driver


// std includes
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

// prj includes
//...

// lib includes
#include "rpihal/gpio.h"
#include "rpihal/uart.h"


#define BTN_0   4
#define BTN_1   17
#define LED_0   22
#define LED_1   10

#define HDSP_RXDATALEN  ((size_t)(rxBuffer[1] + 3))

using std::cout;
using std::endl;



UART_port_t uartObj;
constexpr size_t rxBufferSize = 10;
uint8_t txBuffer[rxBufferSize * 2]; // huge size because sprintf is used
uint8_t rxBuffer[rxBufferSize];
bool rxReady = false;


static int initGpio();
static uint32_t readInp();
static void procHdspAns();
static void printHdspAns();
static void rxTask();
static uint8_t hdspCalcCheckSum(const uint8_t* data, size_t count);
static void hdspGenCheckSum(uint8_t* data);


int main(int argc, char** argv)
{
    int r = -1;
    UART_port_t* const port = &uartObj;
    time_t tOld = 0;
    uint32_t inp, inp_old, inp_pos, inp_neg;
    uint8_t hdspOutputs = 0;

    if(initGpio() != 0) return 1;

    if(argc == 1) r = UART_open(port, "/dev/ttyUSB0", UART_BAUD_19200);
    else
    {
        std::string argDevName(std::string(argv[1]), 0, UART_NAME_SIZE - 1);
        r = UART_open(port, argDevName.c_str(), UART_BAUD_19200);
    }

    if(r)
    {
        cout << "could not open port \"" << port->name << "\"" << endl;
        return 1;
    }


    txBuffer[0] = 0x01; // get info
    txBuffer[1] = 1;    // data len
    txBuffer[2] = 0x00; // get version
    hdspGenCheckSum(txBuffer);
    UART_write(port, txBuffer, txBuffer[1] + 3);

    inp_old = readInp();

    while(1)
    {
        if(rxReady)
        {
            procHdspAns();
            rxReady = false;

            time_t tNow = time(nullptr);

            if(tNow != tOld)
            {
                tOld = tNow;

                const tm ltm = *localtime(&tNow);

                sprintf((char*)(txBuffer + 2), "%02i:%02i:%02i        ", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
                txBuffer[0] = 0x0A; // get I/O
                txBuffer[1] = 8;    // data len
                hdspGenCheckSum(txBuffer);
                UART_write(port, txBuffer, txBuffer[1] + 3);
            }
            else
            {
                inp = readInp();
                inp_pos = ~inp_old & inp;
                inp_neg = inp_old & ~inp;
                inp_old = inp;

                if(inp_pos || inp_neg)
                {
                    hdspOutputs |= (uint8_t)inp_pos;
                    hdspOutputs &= ~((uint8_t)inp_neg);

                    txBuffer[0] = 0x10; // set I/O
                    txBuffer[1] = 1;    // data len
                    txBuffer[2] = hdspOutputs;
                    hdspGenCheckSum(txBuffer);
                    UART_write(port, txBuffer, txBuffer[1] + 3);
                }
                else
                {
                    txBuffer[0] = 0x11; // get I/O
                    txBuffer[1] = 0;    // data len
                    hdspGenCheckSum(txBuffer);
                    UART_write(port, txBuffer, txBuffer[1] + 3);
                }
            }
        }

        rxTask();
    }

    return r;
}


int initGpio()
{
    if(GPIO_init() != 0)
    {
        cout << "GPIO init failed!" << endl;
        return 1;
    }

    GPIO_init_t initStruct;
    GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(LED_0, &initStruct);
    GPIO_initPin(LED_1, &initStruct);

    initStruct.mode = GPIO_MODE_IN;
    GPIO_initPin(BTN_0, &initStruct);
    GPIO_initPin(BTN_1, &initStruct);

    return 0;
}

uint32_t readInp()
{
    uint32_t r = 0;

    if(GPIO_readPin(BTN_0)) r |= 0x01;
    if(GPIO_readPin(BTN_1)) r |= 0x02;

    return r;
}

void procHdspAns()
{
    if(hdspCalcCheckSum(rxBuffer, HDSP_RXDATALEN) != 0)
    {
        cout << "wrong checksum" << endl;
        printHdspAns();
    }
    else if(rxBuffer[2] != 0)
    {
        printf("HDSP answer error! Status: 0x%02X\n", (unsigned int)(rxBuffer[2]));
    }
    else
    {
        switch(rxBuffer[0])
        {
        case 0x01: // get info
            if(rxBuffer[3] == 0) cout << "connected to HDSP v" << (int)(rxBuffer[4]) << "." << (int)(rxBuffer[5]) << endl;
            else printHdspAns();
            break;

        case 0x0A: // set display
            // nop, check cum and status already checked
            break;

        case 0x10: // set I/O
            // nop, check cum and status already checked
            break;

        case 0x11: // get I/O

            if(rxBuffer[3] & 0x20)
            {
                cout << "HDSP button presed" << endl;
                GPIO_writePin(LED_0, 1);
            }

            if(rxBuffer[3] & 0x40)
            {
                cout << "HDSP button released" << endl;
                GPIO_writePin(LED_0, 0);
            }

            break;

        default:
            cout << "unknown CMD" << endl;
            printHdspAns();
            break;
        }
    }
}

void printHdspAns()
{
    for(size_t i = 0; i < HDSP_RXDATALEN; ++i)
    {
        if(i > 0) printf(" ");
        printf("%02X", (unsigned int)(rxBuffer[i]));
    }

    printf("\n");
}

void rxTask()
{
    static size_t rxIndex = 0;
    size_t nBytes = 0;

    if(UART_readByte(&uartObj, rxBuffer + rxIndex, &nBytes) != 0) cout << "UART_readByte() failed!" << endl;

    if(rxReady && nBytes) cout << "rxBuffer overwritten!" << endl;

    rxIndex += nBytes;

    // received the needed number of bytes (HDSP specific)
    if((rxIndex > 1) && ((size_t)(rxBuffer[1] + 3) == rxIndex))
    {
        rxReady = true;
        rxIndex = 0;
    }
    // you might want something like this instead:
    //if((rxIndex > 0) && (rxBuffer[rxIndex - 1] == 0x0A))
    //{
    //    rxReady = true;
    //    rxIndex = 0;
    //}
}

uint8_t hdspCalcCheckSum(const uint8_t* data, size_t count)
{
    uint8_t cs = *data;
    for(size_t i = 1; i < count; ++i) cs ^= *(data + i);
    return cs;
}

void hdspGenCheckSum(uint8_t* data)
{
    data[data[1] + 2] = hdspCalcCheckSum(data, data[1] + 2);
}
