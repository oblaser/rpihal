/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

/*

Copyright (c) 2024 Oliver Blaser

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

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "internal/platform_check.h"
#include "rpihal/i2c.h"
#include "rpihal/rpihal.h"

#include <asm/ioctl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOG_MODULE_LEVEL LOG_LEVEL_DBG
#define LOG_MODULE_NAME  I2C
#include "internal/log.h"



//======================================================================================================================
// some distros lack the headers and/or proper defines for using I2C

// #include <linux/i2c-dev.h>
// #include <linux/i2c.h>

#define I2C_SLAVE 0x0703
#define I2C_SMBUS 0x0720

#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0

#define I2C_SMBUS_QUICK            0
#define I2C_SMBUS_BYTE             1
#define I2C_SMBUS_BYTE_DATA        2
#define I2C_SMBUS_WORD_DATA        3
#define I2C_SMBUS_PROC_CALL        4
#define I2C_SMBUS_BLOCK_DATA       5
#define I2C_SMBUS_I2C_BLOCK_BROKEN 6
#define I2C_SMBUS_BLOCK_PROC_CALL  7
#define I2C_SMBUS_I2C_BLOCK_DATA   8

#define I2C_SMBUS_BLOCK_MAX     32
#define I2C_SMBUS_I2C_BLOCK_MAX 32

//======================================================================================================================



int RPIHAL_I2C_open(RPIHAL_I2C_instance_t* inst, const char* dev, uint8_t addr)
{
    int fd;
    inst->dev[0] = 0;
    inst->fd = -1;

    errno = 0;

    fd = open(dev, O_RDWR);
    if (fd < 0)
    {
        LOG_ERR("failed to open \"%s\" (%s)", dev, strerror(errno));
        return -(__LINE__);
    }
    else
    {
        int ret = 0;
        errno = 0;


        ret = ioctl(fd, I2C_SLAVE, addr);
        if (ret < 0)
        {
            LOG_ERR("failed to config address 0x%02x (%s)", addr, strerror(errno));
            return -(__LINE__);
        }



        LOG_INF("%s addr: 0x%02x", dev, addr);

        strcpy(inst->dev, dev);
        inst->fd = fd;
        inst->addr = addr;
    }

    return 0;
}

int RPIHAL_I2C_rawWrite(const RPIHAL_I2C_instance_t* inst, const uint8_t* data, size_t count)
{
    errno = 0;

    const ssize_t ret = write(inst->fd, data, count);

    if ((ret < 0) || (ret != (ssize_t)count))
    {
        LOG_ERR("failed to write %u bytes to \"%s\" 0x%02x (%s, write ret: %i)", count, inst->dev, inst->addr, strerror(errno), ret);
        return -(__LINE__);
    }

    return 0;
}

int RPIHAL_I2C_rawRead(const RPIHAL_I2C_instance_t* inst, uint8_t* buffer, size_t count)
{
    errno = 0;

    const ssize_t ret = read(inst->fd, buffer, count);

    if ((ret < 0) || (ret != (ssize_t)count))
    {
        LOG_ERR("failed to read %u bytes from \"%s\" 0x%02x (%s, read ret: %i)", count, inst->dev, inst->addr, strerror(errno), ret);
        return -(__LINE__);
    }

    return 0;
}

int RPIHAL_I2C_close(const RPIHAL_I2C_instance_t* inst)
{
    errno = 0;

    if (inst->fd >= 0)
    {
        int err;

        err = close(inst->fd);
        if (err)
        {
            LOG_ERR("failed to close \"%s\" (%s)", inst->dev, strerror(errno));
            return -(__LINE__);
        }
    }
    else { LOG_WRN("device is not open"); }

    return 0;
}
