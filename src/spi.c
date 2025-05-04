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
#include "rpihal/rpihal.h"
#include "rpihal/spi.h"

#include <asm/ioctl.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  SPI
#include "internal/log.h"



int RPIHAL_SPI_open(RPIHAL_SPI_instance_t* inst, const char* dev, uint32_t maxSpeed, uint32_t config)
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



        // ioctl SPI mode

        uint32_t modeFlags = 0;

        // modeFlags |= SPI_LOOP;

        if (config & RPIHAL_SPI_CFG_CPHA) { modeFlags |= SPI_CPHA; }
        if (config & RPIHAL_SPI_CFG_CPOL) { modeFlags |= SPI_CPOL; }

        if (config & RPIHAL_SPI_CFG_LSB_FIRST) { modeFlags |= SPI_LSB_FIRST; }

        if (config & RPIHAL_SPI_CFG_CS_HIGH) { modeFlags |= SPI_CS_HIGH; }
        if (config & RPIHAL_SPI_CFG_HALFDUPLEX) { modeFlags |= SPI_3WIRE; }

        if (config & RPIHAL_SPI_CFG_NO_CS) { modeFlags |= SPI_NO_CS; }

        const uint32_t modeFlagsReq = modeFlags;
        ret = ioctl(fd, SPI_IOC_WR_MODE32, &modeFlags);
        if (ret < 0)
        {
            LOG_ERR("failed to config mode (%s)", strerror(errno));
            return -(__LINE__);
        }

        if (modeFlagsReq != modeFlags) { LOG_WRN("device doesn't support mode 0x%x\n", modeFlagsReq); }



        // ioctl SPI bits per word

        uint8_t nBits = 8; // RasPi kernel driver only supports 8bit mode, even on SPI1,2

        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &nBits);
        if (ret < 0)
        {
            LOG_ERR("failed to config tx bits per word (%s)", strerror(errno));
            return -(__LINE__);
        }

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &nBits);
        if (ret < 0)
        {
            LOG_ERR("failed to config rx bits per word (%s)", strerror(errno));
            return -(__LINE__);
        }



        // ioctl SPI speed

        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &maxSpeed);
        if (ret < 0)
        {
            LOG_ERR("failed to config tx speed (%s)", strerror(errno));
            return -(__LINE__);
        }

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &maxSpeed);
        if (ret < 0)
        {
            LOG_ERR("failed to config rx speed (%s)", strerror(errno));
            return -(__LINE__);
        }



        LOG_INF("opened \"%s\" mode: %04x, speed: %uHz", dev, modeFlags, maxSpeed);

        inst->speed = maxSpeed;
        inst->bits = nBits;

        strncpy(inst->dev, dev, RPIHAL_SPI_INSTANCE_DEV_SIZE);
        inst->dev[RPIHAL_SPI_INSTANCE_DEV_SIZE - 1] = 0;

        inst->fd = fd;
    }

    return 0;
}

int RPIHAL_SPI_transfer(const RPIHAL_SPI_instance_t* inst, const uint8_t* txData, uint8_t* rxBuffer, size_t count)
{
    errno = 0;

    struct spi_ioc_transfer transfer;

    memset(&transfer, 0, sizeof(transfer));

    transfer.tx_buf = (uintptr_t)txData;
    transfer.rx_buf = (uintptr_t)rxBuffer;
    transfer.len = count;
    transfer.delay_usecs = 0;
    transfer.speed_hz = inst->speed;
    transfer.bits_per_word = inst->bits;

    const int ret = ioctl(inst->fd, SPI_IOC_MESSAGE(1), &transfer);

    if ((ret < 0) || (ret != (int)count))
    {
        LOG_ERR("failed to transfer %u bytes (%s, ioctl ret: %i)", count, strerror(errno), ret);
        return -(__LINE__);
    }

    return 0;
}

int RPIHAL_SPI_close(RPIHAL_SPI_instance_t* inst)
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
        else
        {
            LOG_INF("closed \"%s\"", inst->dev);
            inst->dev[0] = 0;
            inst->fd = -1;
        }
    }
    else { LOG_WRN("device is not open"); }

    return 0;
}
