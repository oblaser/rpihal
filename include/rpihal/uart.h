/*
author          Oliver Blaser
date            12.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

/*

Copyright (c) 2022 Oliver Blaser

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*

Since the OS uses the BCM registers, we use the OS' library instead of registers to access the UART interface.

*/

#ifndef IG_RPIHAL_UART_H
#define IG_RPIHAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


#define RPIHAL_UART_NAME_SIZE (300) // a "normal" fs path

#define RPIHAL_UART_BAUD_50         (50)
#define RPIHAL_UART_BAUD_75         (75)
#define RPIHAL_UART_BAUD_110        (110)
#define RPIHAL_UART_BAUD_134        (134)
#define RPIHAL_UART_BAUD_150        (150)
#define RPIHAL_UART_BAUD_200        (200)
#define RPIHAL_UART_BAUD_300        (300)
#define RPIHAL_UART_BAUD_600        (600)
#define RPIHAL_UART_BAUD_1200       (1200)
#define RPIHAL_UART_BAUD_1800       (1800)
#define RPIHAL_UART_BAUD_2400       (2400)
#define RPIHAL_UART_BAUD_4800       (4800)
#define RPIHAL_UART_BAUD_9600       (9600)
#define RPIHAL_UART_BAUD_19200      (19200)
#define RPIHAL_UART_BAUD_38400      (38400)
#define RPIHAL_UART_BAUD_57600      (57600)
#define RPIHAL_UART_BAUD_115200     (115200)
#define RPIHAL_UART_BAUD_230400     (230400)
#define RPIHAL_UART_BAUD_460800     (460800)
#define RPIHAL_UART_BAUD_500000     (500000)
#define RPIHAL_UART_BAUD_576000     (576000)
#define RPIHAL_UART_BAUD_921600     (921600)
#define RPIHAL_UART_BAUD_1000000    (1000000)
#define RPIHAL_UART_BAUD_1152000    (1152000)
#define RPIHAL_UART_BAUD_1500000    (1500000)
#define RPIHAL_UART_BAUD_2000000    (2000000)
#define RPIHAL_UART_BAUD_2500000    (2500000)
#define RPIHAL_UART_BAUD_3000000    (3000000)
#define RPIHAL_UART_BAUD_3500000    (3500000)
#define RPIHAL_UART_BAUD_4000000    (4000000)

enum RPIHAL_UART_OPEN_ERROR
{
    RPIHAL_UART_OPENE_OK = 0,
    RPIHAL_UART_OPENE_NAME,
    RPIHAL_UART_OPENE_FD,
    RPIHAL_UART_OPENE_BAUD,
    RPIHAL_UART_OPENE_TCGETATTR,
    RPIHAL_UART_OPENE_CFSETISPEED,
    RPIHAL_UART_OPENE_CFSETOSPEED,
    RPIHAL_UART_OPENE_TCSETATTR,
    RPIHAL_UART_OPENE__end_
};


typedef struct
{
    // public
    char name[RPIHAL_UART_NAME_SIZE];
    int baud;
    // int? parity
    // int? nStop
    
    // private
    int fd;
} RPIHAL_UART_port_t;


//! @brief Opens and inits a serial port.
//! @param port Pointer to the ports instance
//! @param name Path of the serial port device (must not point to `port->name`)
//! @param baud Baudrate, one of `RPIHAL_UART_BAUD_..`
//! @return __0__ on success
int RPIHAL_UART_open(RPIHAL_UART_port_t* port, const char* name, int baud/*, parity, nStop*/);

int RPIHAL_UART_close(RPIHAL_UART_port_t* port);

int RPIHAL_UART_read(const RPIHAL_UART_port_t* port, uint8_t* buffer, size_t bufferSize, size_t* nBytesRead);

inline int RPIHAL_UART_readByte(const RPIHAL_UART_port_t* port, uint8_t* byte, size_t* nBytesRead)
{ return RPIHAL_UART_read(port, byte, 1, nBytesRead); }

int RPIHAL_UART_write2(const RPIHAL_UART_port_t* port, const uint8_t* data, size_t count, size_t* nBytesWritten);

inline int RPIHAL_UART_write(const RPIHAL_UART_port_t* port, const uint8_t* data, size_t count)
{ return RPIHAL_UART_write2(port, data, count, NULL); }

int RPIHAL_UART_print2(const RPIHAL_UART_port_t* port, const char* str, size_t* nBytesWritten);

inline int RPIHAL_UART_print(const RPIHAL_UART_port_t* port, const char* str)
{ return RPIHAL_UART_print2(port, str, NULL); }

//! @return TRUE (`1`), FALSE (`0`) or error (`-1`) if __port__ is `NULL`
int RPIHAL_UART_isOpen(const RPIHAL_UART_port_t* port);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_UART_H
