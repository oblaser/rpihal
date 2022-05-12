/*
author          Oliver Blaser
date            12.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

// using the HDSP Display project (v1.x) for testing UART
// https://github.com/oblaser/HDSP-21xx-driver


// std includes
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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



UART_port_t uartObj;
#define rxBufferSize ((size_t)(10))
uint8_t txBuffer[rxBufferSize * 2]; // let's reserve enough memory, because sprintf is used
uint8_t rxBuffer[rxBufferSize];
int rxReady = 0;


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
    const char* serialPortDevName;
    UART_port_t* const port = &uartObj;
    time_t tOld = 0;
    uint32_t inp, inp_old, inp_pos, inp_neg;
    uint8_t hdspOutputs = 0;

    if(initGpio() != 0) return 1;

    if(argc == 1) serialPortDevName = "/dev/ttyUSB0";
    else serialPortDevName = argv[1];

    r = UART_open(port, serialPortDevName, UART_BAUD_19200);

    if(r)
    {
        printf("could not open port \"%s\"\n", port->name);
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
            rxReady = 0;

            time_t tNow = time(NULL);

            if(tNow != tOld)
            {
                tOld = tNow;

                const struct tm ltm = *localtime(&tNow);

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
        printf("GPIO init failed!\n");
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
        printf("wrong checksum");
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
            if(rxBuffer[3] == 0) printf("connected to HDSP v%i.%i\n", (int)(rxBuffer[4]), (int)(rxBuffer[5]));
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
                printf("HDSP button presed\n");
                GPIO_writePin(LED_0, 1);
            }

            if(rxBuffer[3] & 0x40)
            {
                printf("HDSP button released\n");
                GPIO_writePin(LED_0, 0);
            }

            break;

        default:
            printf("unknown CMD\n");
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

    if(UART_readByte(&uartObj, rxBuffer + rxIndex, &nBytes) != 0) printf("UART_readByte() failed!\n");

    if(rxReady && nBytes) printf("rxBuffer overwritten!\n");

    rxIndex += nBytes;

    // received the needed number of bytes (HDSP specific)
    if((rxIndex > 1) && ((size_t)(rxBuffer[1] + 3) == rxIndex))
    {
        rxReady = 1;
        rxIndex = 0;
    }
    // you might want something like this instead:
    //if((rxIndex > 0) && (rxBuffer[rxIndex - 1] == 0x0A))
    //{
    //    rxReady = 1;
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