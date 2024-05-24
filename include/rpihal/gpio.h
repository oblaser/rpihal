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

#ifndef IG_RPIHAL_GPIO_H
#define IG_RPIHAL_GPIO_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


#define RPIHAL_GPIO_MODE_IN    0
#define RPIHAL_GPIO_MODE_OUT   1
#define RPIHAL_GPIO_MODE_AF    2

#define RPIHAL_GPIO_PULL_NONE  (0x00)
#define RPIHAL_GPIO_PULL_DOWN  (0x01)
#define RPIHAL_GPIO_PULL_UP    (0x02)

//#define RPIHAL_GPIO_DRIVE_2mA      0
//#define RPIHAL_GPIO_DRIVE_4mA      1
//#define RPIHAL_GPIO_DRIVE_6mA      2
//#define RPIHAL_GPIO_DRIVE_8mA      3
//#define RPIHAL_GPIO_DRIVE_10mA     4
//#define RPIHAL_GPIO_DRIVE_12mA     5
//#define RPIHAL_GPIO_DRIVE_14mA     6
//#define RPIHAL_GPIO_DRIVE_16mA     7

#define RPIHAL_GPIO_AF_0   0
#define RPIHAL_GPIO_AF_1   1
#define RPIHAL_GPIO_AF_2   2
#define RPIHAL_GPIO_AF_3   3
#define RPIHAL_GPIO_AF_4   4
#define RPIHAL_GPIO_AF_5   5


typedef volatile uint32_t RPIHAL_reg_t; // register type
typedef RPIHAL_reg_t* RPIHAL_regptr_t;  // register pointer

typedef struct
{
    int mode;
    int pull;
    //int drive;
    int altfunc; // unexpected behaviour may be observed (see https://elinux.org/BCM2835_datasheet_errata#p92_to_95_.26_102_to_103)
} RPIHAL_GPIO_init_t;


//! @return __0__ on success, __negative__ on error
//! 
//! Needs to be called once at the start of the app.
//! 
int RPIHAL_GPIO_init();

//! @param pin BCM GPIO pin number
//! @param initStruct Pointer to the pin settings
//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct);

//! @param pin BCM GPIO pin number
//! @return __0__ LOW / __1__ HIGH / __negative__ on error
int RPIHAL_GPIO_readPin(int pin);

uint32_t RPIHAL_GPIO_read();

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

//! @param initStruct Pointer to the pin settings which will be set to the default values
//! @return __0__ on success, __negative__ on error
int RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct);

int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct);

RPIHAL_regptr_t RPIHAL_GPIO_getMemBasePtr();

//! @return TRUE (`1`), FALSE (`0`) or unknown (`-1`)
int RPIHAL_GPIO_isUsingGpiomem();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_GPIO_H
