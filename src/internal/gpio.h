/*
author          Oliver Blaser
date            03.11.2024
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

#ifndef IG_RPIHAL_INTERNAL_GPIO_H
#define IG_RPIHAL_INTERNAL_GPIO_H

#include <stdint.h>

#include <rpihal/gpio.h>


#ifdef __cplusplus
extern "C" {
#endif


//! @param pin BCM GPIO pin number
//! @param sysGpioLocked Has to be TRUE (`1`), might only be unlocked (set to FALSE) on compute modules
//! @return Boolean value TRUE (`1`) if it's allowed to access the pin, otherwhise FALSE (`0`)
int iGPIO_checkPin(int pin, int sysGpioLocked);

//! @return 0 if the hardware model is not supported, otherwise the USER_PINS_MASK_x
uint64_t iGPIO_getUserPinsMask();

//! @return 0 if the hardware model is not supported, otherwise the BCMx_PINS_MASK
uint64_t iGPIO_getBcmPinsMask();

void iGPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct);

int iGPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct);

int iGPIO_bittopin(uint64_t bit);

uint64_t iGPIO_pintobit(int pin);



#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_INTERNAL_GPIO_H
