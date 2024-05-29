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

#ifndef IG_RPIHAL_SPI_H
#define IG_RPIHAL_SPI_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


#define RPIHAL_SPI_CFG_FLAG_MASK       (0x000F)
#define RPIHAL_SPI_CFG_FLAG_DEFAULT    (0)      // Transfer MSB first, chip select is active low, chip select is handled by driver/hw
#define RPIHAL_SPI_CFG_FLAG_LSB_FIRST  (0x0001) // Transfer LSB first
#define RPIHAL_SPI_CFG_FLAG_CS_HIGH    (0x0002) // Chip select is active high
#define RPIHAL_SPI_CFG_FLAG_HALFDUPLEX (0x0004) // Use bidirectional half duplex mode (MIMO instead of MISO and MOSI)
#define RPIHAL_SPI_CFG_FLAG_NO_CS      (0x0008) // `SPI_NO_CS` will me set


#define RPIHAL_SPI_INSTANCE_DEV_SIZE (300)


enum RPIHAL_SPI_MODE
{
    RPIHAL_SPI_MODE_0 = 0, // CPOL=0, CPHA=0
    RPIHAL_SPI_MODE_1,     // CPOL=0, CPHA=1
    RPIHAL_SPI_MODE_2,     // CPOL=1, CPHA=0
    RPIHAL_SPI_MODE_3,     // CPOL=1, CPHA=1
};

/**
 * @brief SPI instance.
 *
 * Do not write to this struct, use only the `RPIHAL_SPI_..` functions to change settings.
 */
typedef struct
{
    char dev[RPIHAL_SPI_INSTANCE_DEV_SIZE];
    int fd;
    uint32_t speed;
    uint8_t bits;
} RPIHAL_SPI_instance_t;



/**
 * @brief Configures the specified SPI device and initialises the instance.
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * The clock speed is a maximum value, the driver selects the highest possible clock but not higher than `maxSpeed` (see
 * [elinux.org](https://elinux.org/RPi_SPI#Speed_2) for furter information).
 *
 * @param [out] inst rpihal SPI instance
 * @param dev Path to the SPI device (e.g. `/dev/spidev0.0`)
 * @param maxSpeed Max clock speed [Hz]
 * @param mode One of `RPIHAL_SPI_MODE`
 * @param flags One or more `RPIHAL_SPI_CFG_FLAG_..` ored together
 * @return __0__ on success, negative on failure
 */
int RPIHAL_SPI_open(RPIHAL_SPI_instance_t* inst, const char* dev, uint32_t maxSpeed, int mode, uint32_t flags);

/**
 * @brief
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param inst
 * @param txData
 * @param rxBuffer
 * @param count
 * @return __0__ on success, negative on failure
 */
int RPIHAL_SPI_transfer(const RPIHAL_SPI_instance_t* inst, const uint8_t* txData, uint8_t* rxBuffer, size_t count);

/**
 * @brief
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param [in,out] inst
 * @return __0__ on success, negative on failure
 */
int RPIHAL_SPI_close(RPIHAL_SPI_instance_t* inst);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_SPI_H
