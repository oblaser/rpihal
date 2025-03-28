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

#ifndef IG_RPIHAL_I2C_H
#define IG_RPIHAL_I2C_H

#include <stddef.h>
#include <stdint.h>

#include "../rpihal/common/types.h"


#ifdef __cplusplus
extern "C" {
#endif


#define RPIHAL_I2C_INSTANCE_DEV_SIZE (300)


/**
 * @brief I2C instance.
 *
 * Do not write to this struct, use only the `RPIHAL_I2C_..` functions to change settings.
 */
typedef struct
{
    char dev[RPIHAL_I2C_INSTANCE_DEV_SIZE];
    int fd;
    uint8_t addr;
} RPIHAL_I2C_instance_t;


/**
 * @brief Configures the specified I2C device and initialises the instance.
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param [out] inst rpihal I2C instance
 * @param dev Path to the I2C device (e.g. `/dev/i2c-1`)
 * @param addr Address of the desired I2C slave (only 7bit is supported)
 * @return __0__ on success, negative on failure
 */
int RPIHAL_I2C_open(RPIHAL_I2C_instance_t* inst, const char* dev, uint8_t addr);

/**
 * @brief
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param inst
 * @param data
 * @param count
 * @return Number of written bytes on success, __-1__ on error
 */
ssize_t RPIHAL_I2C_write(const RPIHAL_I2C_instance_t* inst, const uint8_t* data, size_t count);

/**
 * @brief
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param inst
 * @param buffer
 * @param count
 * @return Number of read bytes on success, __-1__ on error
 */
ssize_t RPIHAL_I2C_read(const RPIHAL_I2C_instance_t* inst, uint8_t* buffer, size_t count);

/**
 * @brief
 *
 * `errno` is cleared by this function. If the function fails, `errno` might be non 0, depending on the error.
 *
 * @param [in,out] inst
 * @return __0__ on success, negative on failure
 */
int RPIHAL_I2C_close(RPIHAL_I2C_instance_t* inst);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_I2C_H
