/*
author          Oliver Blaser
date            10.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#ifndef IG_RPIHAL_UART_H
#define IG_RPIHAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


#define UART_NAME_SIZE (300) // a "normal" fs path

#define UART_BAUD_50         (50)
#define UART_BAUD_75         (75)
#define UART_BAUD_110        (110)
#define UART_BAUD_134        (134)
#define UART_BAUD_150        (150)
#define UART_BAUD_200        (200)
#define UART_BAUD_300        (300)
#define UART_BAUD_600        (600)
#define UART_BAUD_1200       (1200)
#define UART_BAUD_1800       (1800)
#define UART_BAUD_2400       (2400)
#define UART_BAUD_4800       (4800)
#define UART_BAUD_9600       (9600)
#define UART_BAUD_19200      (19200)
#define UART_BAUD_38400      (38400)
#define UART_BAUD_57600      (57600)
#define UART_BAUD_115200     (115200)
#define UART_BAUD_230400     (230400)
#define UART_BAUD_460800     (460800)
#define UART_BAUD_500000     (500000)
#define UART_BAUD_576000     (576000)
#define UART_BAUD_921600     (921600)
#define UART_BAUD_1000000    (1000000)
#define UART_BAUD_1152000    (1152000)
#define UART_BAUD_1500000    (1500000)
#define UART_BAUD_2000000    (2000000)
#define UART_BAUD_2500000    (2500000)
#define UART_BAUD_3000000    (3000000)
#define UART_BAUD_3500000    (3500000)
#define UART_BAUD_4000000    (4000000)


typedef struct
{
    // public
    char name[UART_NAME_SIZE];
    int baud;
    // int? parity
    // int? nStop
    
    // private
    int fd;
} UART_port_t;


//! @brief Opens and inits a serial port.
//! @param port Pointer to the ports instance
//! @param name Path of the serial port device
//! @param baud Baudrate
//! @return __0__ on success
int UART_open(UART_port_t* port, const char* name, int baud/*, parity, nStop*/);

int UART_close(UART_port_t* port);

int UART_read2(const UART_port_t* port, uint8_t* buffer, size_t bufferSize, size_t* nBytesRead);

int UART_read(const UART_port_t* port, uint8_t* buffer, size_t bufferSize)
{ return UART_read2(port, buffer, bufferSize, NULL); }

int UART_readByte(const UART_port_t* port, uint8_t* byte)
{ return UART_read2(port, byte, 1, NULL); }

int UART_write2(const UART_port_t* port, const uint8_t* data, size_t count, size_t* nBytesWritten);

int UART_write(const UART_port_t* port, const uint8_t* data, size_t count)
{ return UART_write2(port, data, count); }

int UART_print2(const UART_port_t* port, const char* str, size_t* nBytesWritten);

int UART_print(const UART_port_t* port, const char* str)
{ return UART_print2(port, str, NULL); }

//! @return TRUE (`1`), FALSE (`0`) or error (`-1`) if __port__ is `NULL`
int UART_isOpen(const UART_port_t* port);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_UART_H
