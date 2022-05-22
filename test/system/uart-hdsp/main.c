/*
author          Oliver Blaser
date            13.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

// using the HDSP Display project (v1.x) for testing UART
// https://github.com/oblaser/HDSP-21xx-driver


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <rpihal/gpio.h>
#include <rpihal/uart.h>

#if 0 // using my WP board
#define BTN_0   4
#define BTN_1   17
#define LED_0   22
#define LED_1   10
#elif 1 // using the Iono Pi
#define BTN_0   12  // DI4
#define BTN_1   6   // DI5
#define LED_0   7   // L1
#define LED_1   23  // O4
#else
#error "choose board"
#endif

#define HDSP_RXDATALEN  ((size_t)(rxBuffer[1] + 3))



RPIHAL_UART_port_t uartObj;
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
    RPIHAL_UART_port_t* const port = &uartObj;
    time_t tOld = 0;
    uint32_t inp, inp_old, inp_pos, inp_neg;
    uint8_t hdspOutputs = 0;

    if(initGpio() != 0) return 1;

    if(argc == 1) serialPortDevName = "/dev/ttyUSB0";
    else serialPortDevName = argv[1];

    r = RPIHAL_UART_open(port, serialPortDevName, RPIHAL_UART_BAUD_19200);

    if(r)
    {
        printf("could not open port \"%s\"\n", port->name);
        return 1;
    }


    txBuffer[0] = 0x01; // get info
    txBuffer[1] = 1;    // data len
    txBuffer[2] = 0x00; // get version
    hdspGenCheckSum(txBuffer);
    RPIHAL_UART_write(port, txBuffer, txBuffer[1] + 3);

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
                txBuffer[0] = 0x0A; // set display
                txBuffer[1] = 8;    // data len
                hdspGenCheckSum(txBuffer);
                RPIHAL_UART_write(port, txBuffer, txBuffer[1] + 3);
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
                    RPIHAL_UART_write(port, txBuffer, txBuffer[1] + 3);
                }
                else
                {
                    txBuffer[0] = 0x11; // get I/O
                    txBuffer[1] = 0;    // data len
                    hdspGenCheckSum(txBuffer);
                    RPIHAL_UART_write(port, txBuffer, txBuffer[1] + 3);
                }
            }
        }

        rxTask();
    }

    return r;
}


int initGpio()
{
    if(RPIHAL_GPIO_init() != 0)
    {
        printf("GPIO init failed!\n");
        return 1;
    }

    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    RPIHAL_GPIO_initPin(LED_0, &initStruct);
    RPIHAL_GPIO_initPin(LED_1, &initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_IN;
    RPIHAL_GPIO_initPin(BTN_0, &initStruct);
    RPIHAL_GPIO_initPin(BTN_1, &initStruct);

    return 0;
}

uint32_t readInp()
{
    uint32_t r = 0;

    if(RPIHAL_GPIO_readPin(BTN_0)) r |= 0x01;
    if(RPIHAL_GPIO_readPin(BTN_1)) r |= 0x02;

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
                RPIHAL_GPIO_writePin(LED_0, 1);
            }

            if(rxBuffer[3] & 0x40)
            {
                printf("HDSP button released\n");
                RPIHAL_GPIO_writePin(LED_0, 0);
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

    if(RPIHAL_UART_readByte(&uartObj, rxBuffer + rxIndex, &nBytes) != 0) printf("RPIHAL_UART_readByte() failed!\n");

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
