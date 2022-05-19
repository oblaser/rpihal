/*
author          Oliver Blaser
date            19.05.2022
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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "rpihal/uart.h"
#include "zPlatformCheck.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>



static speed_t getUnixBaud(int baud, int* error);



int UART_open(UART_port_t* port, const char* name, int baud)
{
    int r = -1;
    
    if(port)
    {
        r = UART_OPENE_OK;

        port->name[0] = 0;

        if(name)
        {
            const size_t nameLen = strlen(name);
            if(nameLen < UART_NAME_SIZE) strcpy(port->name, name);
            else r = UART_OPENE_NAME;
        }

        if(!r)
        {
            port->fd = open(port->name, O_RDWR);

            if(port->fd >= 0)
            {
                struct termios tty;
                if(tcgetattr(port->fd, &tty) == 0)
                {
                    tty.c_cflag &= ~PARENB; // no parity
                    tty.c_cflag &= ~CSTOPB; // clear stop field => one stop bit
                    tty.c_cflag &= ~CSIZE;
                    tty.c_cflag |= CS8; // 8bit data word
                    tty.c_cflag &= ~CRTSCTS; // disable RTS/CTS hardware flow control
                    tty.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines (CLOCAL = 1)
                    
                    tty.c_lflag &= ~ICANON; // non-canonical mode
                    tty.c_lflag &= ~ECHO; // disable echo
                    tty.c_lflag &= ~ECHOE; // disable erasure
                    tty.c_lflag &= ~ECHONL; // disable new-line echo
                    tty.c_lflag &= ~ISIG; // disable interpretation of INTR, QUIT and SUSP
                    
                    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
                    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // disable any special handling of received bytes
                    
                    tty.c_oflag &= ~OPOST; // prevent special interpretation of output bytes (e.g. newline chars)
                    tty.c_oflag &= ~ONLCR; // prevent conversion of newline to carriage return/line feed
                    //tty.c_oflag &= ~OXTABS; // prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
                    //tty.c_oflag &= ~ONOEOT; // prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)
                    
                    // non blocking read
                    tty.c_cc[VTIME] = 0;
                    tty.c_cc[VMIN] = 0;
                    
                    port->baud = baud;
                    int unixBaudErr;
                    const speed_t unixBaud = getUnixBaud(port->baud, &unixBaudErr);

                    if(unixBaudErr == 0)
                    {
                        if(cfsetispeed(&tty, unixBaud) == 0)
                        {
                            if(cfsetospeed(&tty, unixBaud) == 0)
                            {
                                if(tcsetattr(port->fd, TCSANOW, &tty) != 0)
                                {
                                    r = UART_OPENE_TCSETATTR;
                                }
                                // else nop, set to OK above
                            }
                            else
                            {
                                r = UART_OPENE_CFSETOSPEED;
                            }
                        }
                        else
                        {
                            r = UART_OPENE_CFSETISPEED;
                        }
                    }
                    else r = UART_OPENE_BAUD;
                }
                else
                {
                    r = UART_OPENE_TCGETATTR;
                }
            }
            else
            {
                r = UART_OPENE_FD;
            }
        }

        if(!(r == UART_OPENE_OK || r == UART_OPENE_FD))
        {
            close(port->fd);
            port->fd = -1;
        }
    }
    
    return r;
}

int UART_close(UART_port_t* port)
{
    int r = -1;
    
    if(port)
    {
        if(close(port->fd) == 0)
        {
            r = 0;
            port->fd = -1;
        }
        else r = 1;
    }
    
    return r;
}

int UART_read(const UART_port_t* port, uint8_t* buffer, size_t bufferSize, size_t* nBytesRead)
{
    int r = -1;
    
    if(port)
    {
        const ssize_t rdres = read(port->fd, buffer, bufferSize);
        
        if(rdres < 0) r = 1;
        else
        {
            r = 0;
            if(nBytesRead) *nBytesRead = (size_t)rdres;
        }
    }
    
    return r;
}

int UART_write2(const UART_port_t* port, const uint8_t* data, size_t count, size_t* nBytesWritten)
{
    int r = -1;
    
    if(port)
    {
        const ssize_t wrres = write(port->fd, data, count);
        
        if(wrres < 0) r = 1;
        else
        {
            if(nBytesWritten) *nBytesWritten = (size_t)wrres;
            
            if(count == (size_t)wrres) r = 0;
            else r = 2;
        }
    }
    
    return r;
}

int UART_print2(const UART_port_t* port, const char* str, size_t* nBytesWritten)
{
    int r = -1;
    
    if(str)
    {
        const size_t len = strlen(str);
        
        r = UART_write2(port, (uint8_t*)str, len, nBytesWritten);
    }
    
    return r;
}

int UART_isOpen(const UART_port_t* port)
{
    int r = 0;
    if(port && (port->fd >= 0)) r = 1;
    return r;
}



speed_t getUnixBaud(int baud, int* error)
{
    int e = 0;
    speed_t r;

    switch (baud)
    {
    case 0:
        r = B0;
        break;

    case 50:
        r = B50;
        break;

    case 75:
        r = B75;
        break;

    case 110:
        r = B110;
        break;

    case 134:
        r = B134;
        break;

    case 150:
        r = B150;
        break;

    case 200:
        r = B200;
        break;

    case 300:
        r = B300;
        break;

    case 600:
        r = B600;
        break;

    case 1200:
        r = B1200;
        break;

    case 1800:
        r = B1800;
        break;

    case 2400:
        r = B2400;
        break;

    case 4800:
        r = B4800;
        break;

    case 9600:
        r = B9600;
        break;

    case 19200:
        r = B19200;
        break;

    case 38400:
        r = B38400;
        break;

    case 57600:
        r = B57600;
        break;

    case 115200:
        r = B115200;
        break;

    case 230400:
        r = B230400;
        break;

    case 460800:
        r = B460800;
        break;

    case 500000:
        r = B500000;
        break;

    case 576000:
        r = B576000;
        break;

    case 921600:
        r = B921600;
        break;

    case 1000000:
        r = B1000000;
        break;

    case 1152000:
        r = B1152000;
        break;

    case 1500000:
        r = B1500000;
        break;

    case 2000000:
        r = B2000000;
        break;

    case 2500000:
        r = B2500000;
        break;

    case 3000000:
        r = B3000000;
        break;

    case 3500000:
        r = B3500000;
        break;

    case 4000000:
        r = B4000000;
        break;

    default:
        e = 1;
        break;
    };

    if(error) *error = e;

    return r;
}
