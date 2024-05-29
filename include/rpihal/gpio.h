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

#ifndef IG_RPIHAL_GPIO_H
#define IG_RPIHAL_GPIO_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


enum RPIHAL_GPIO_MODE
{
    RPIHAL_GPIO_MODE_IN = 0,
    RPIHAL_GPIO_MODE_OUT,
    RPIHAL_GPIO_MODE_AF,
};

enum RPIHAL_GPIO_PULL
{
    RPIHAL_GPIO_PULL_NONE = 0,
    RPIHAL_GPIO_PULL_UP,
    RPIHAL_GPIO_PULL_DOWN,
};

enum RPIHAL_GPIO_DRIVE
{
    RPIHAL_GPIO_DRIVE_2mA = 0,
    RPIHAL_GPIO_DRIVE_4mA,
    RPIHAL_GPIO_DRIVE_6mA,
    RPIHAL_GPIO_DRIVE_8mA,
    RPIHAL_GPIO_DRIVE_10mA,
    RPIHAL_GPIO_DRIVE_12mA,
    RPIHAL_GPIO_DRIVE_14mA,
    RPIHAL_GPIO_DRIVE_16mA,
};

enum RPIHAL_GPIO_AF
{
    RPIHAL_GPIO_AF_0 = 0,
    RPIHAL_GPIO_AF_1,
    RPIHAL_GPIO_AF_2,
    RPIHAL_GPIO_AF_3,
    RPIHAL_GPIO_AF_4,
    RPIHAL_GPIO_AF_5,
};


typedef volatile uint32_t RPIHAL_reg_t; // register type
typedef RPIHAL_reg_t* RPIHAL_regptr_t;  // register pointer

typedef struct
{
    int mode;
    int pull;
    // int drive;
    int altfunc; // ANOM1
} RPIHAL_GPIO_init_t;


//! @return __0__ on success, __negative__ on error
//!
//! Needs to be called once at the start of the app.
//!
int RPIHAL_GPIO_init();

//! @param pin BCM GPIO pin number
//! @param initStruct Pin configuration
//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct);

/**
 * @brief Configures the pins according to `initStruct`.
 *
 * @param bits Bits coresponding to the pins to be configured
 * @param initStruct Pin configuration
 * @return __0__ on success, __negative__ on error
 */
int RPIHAL_GPIO_initPins(uint64_t bits, const RPIHAL_GPIO_init_t* initStruct);

//! @param pin BCM GPIO pin number
//! @return __0__ LOW / __1__ HIGH / __negative__ on error
int RPIHAL_GPIO_readPin(int pin);

uint32_t RPIHAL_GPIO_read();
uint32_t RPIHAL_GPIO_readHi();
uint64_t RPIHAL_GPIO_read64();

//! @param pin BCM GPIO pin number
//! @param state Boolean value representing the pin states HIGH (`1`) and LOW (`0`)
//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_writePin(int pin, int state);

//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_set(uint32_t bits);

//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_clr(uint32_t bits);

//! @param pin BCM GPIO pin number
//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_togglePin(int pin);

//! @return __0__ on success, __negative__ on error
//!
//! Resets all user pins to their default setups.
//!
int RPIHAL_GPIO_reset();

//! @param pin BCM GPIO pin number
//! @return __0__ on success, __negative__ on error
//!
//! Resets the specified pin to it's default setup.
//!
int RPIHAL_GPIO_resetPin(int pin);

//! @param [out] initStruct
void RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct);

//! @brief Get the default configuration (config after reset) for the specified pin.
//!
//! @param pin
//! @param [out] initStruct
//!
int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct);

RPIHAL_regptr_t RPIHAL_GPIO_getMemBasePtr();

//! @return TRUE (`1`), FALSE (`0`) or unknown (`-1`)
int RPIHAL_GPIO_isUsingGpiomem();

//! @brief Returns the GPIO pin number of the setted bit.
//!
//! The index of the first setted bit beginning at the LSB is returned. If <tt>bit</tt> is *0* or the bit is out of
//! range, `-1` is returned.
//!
int RPIHAL_GPIO_bittopin(uint64_t bit);

uint64_t RPIHAL_GPIO_pintobit(int pin);


/**
 * @brief Prints the alternate function registers to stdout.
 *
 * Needed during investigation of ANOM1.
 *
 * @param pins **-1** for all pins
 */
void RPIHAL_GPIO_dumpAltFuncReg(uint64_t pins);

/**
 * @brief Prints the pull up/down config registers to stdout.
 *
 * Only avaulable since BCM2711 (Pi4), nop for older models.
 *
 * @param pins **-1** for all pins
 */
void RPIHAL_GPIO_dumpPullUpDnReg(uint64_t pins);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_GPIO_H
