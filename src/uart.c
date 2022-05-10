/*
author          Oliver Blaser
date            10.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "rpihal/gpio.h"

#include <fcntl.h>
#include <termios.h>


int UART_open(UART_port_t* port, const char* name, int baud)
{
    int r = -1;
    
    if(port)
    {
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

int UART_read2(const UART_port_t* port, uint8_t* buffer, size_t bufferSize, size_t* nBytesRead)
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
