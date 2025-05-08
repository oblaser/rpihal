/*
author          Oliver Blaser
date            03.05.2025
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

/*

Copyright (c) 2025 Oliver Blaser

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


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_INTERNAL_GPIO_H



#ifdef iGPIO_DEFINE_FUNCTIONS
#undef iGPIO_DEFINE_FUNCTIONS

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "rpihal/gpio.h"
#include "rpihal/rpihal.h"

#undef LOG_MODULE_LEVEL
#undef LOG_MODULE_NAME
#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  GPIO
#include "../internal/log.h"



#define BCM283x_PINS_MASK (0x003FFFFFFFFFFFFFull) // where x = 5, 6, 7
// #define BCM283x_FIRST_PIN (0)
// #define BCM283x_LAST_PIN  (53)

#define BCM2711_PINS_MASK (0x03FFFFFFFFFFFFFFull)
// #define BCM2711_FIRST_PIN (0)
// #define BCM2711_LAST_PIN  (57)

#define USER_PINS_MASK_26pin_rev1    (0x0000000003E6CF93ull)
#define USER_PINS_MASK_26pin_rev2_P1 (0x000000000BC6CF9Cull) // GPIO pin header P1
#define USER_PINS_MASK_26pin_rev2_P5 (0x00000000F0000000ull) // addon GPIO pin header P5
#define USER_PINS_MASK_26pin_rev2    (USER_PINS_MASK_26pin_rev2_P1 | USER_PINS_MASK_26pin_rev2_P5)

// 26pin rev2 and 40pin are pin compatible (on the first 26 pins)
// https://elinux.org/RPi_Low-level_peripherals#General_Purpose_Input.2FOutput_.28GPIO.29
// https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-and-the-40-pin-header

#define USER_PINS_MASK_40pin (0x000000000FFFFFFFull)
// #define FIRST_USER_PIN_40pin (0)
// #define LAST_USER_PIN_40pin  (27)



int iGPIO_checkPin(int pin, int sysGpioLocked)
{
    int r = 0;

    // bit of the pin
    uint64_t bit = 0x01;
    bit <<= pin;

    uint64_t mask;

    if (sysGpioLocked) { mask = iGPIO_getUserPinsMask(); }
    else { mask = iGPIO_getBcmPinsMask(); }

    if (mask & bit) r = 1;

    if (!r)
    {
        if (sysGpioLocked) { LOG_WRN("system GPIOs are locked"); }
        LOG_ERR("invalid pin: %i", pin);
    }

    return r;
}

uint64_t iGPIO_getUserPinsMask()
{
    uint64_t mask = 0;

    const RPIHAL_model_t hwModel = RPIHAL_getModel();

    if (RPIHAL_model_header_is_26pin(hwModel))
    {
#if 0 // ADDHW
            if (RPIHAL_model_t has to be a bit field, so the rev can be read out of it) { mask = USER_PINS_MASK_26pin_rev; }
            else if () { mask = USER_PINS_MASK_26pin_rev; }
            else { mask = 0; }
#else
        LOG_WRN("%s is not implemented for this RasPi model", __func__);
        mask = 0;
#endif
    }
    else if (RPIHAL_model_header_is_40pin(hwModel)) { mask = USER_PINS_MASK_40pin; }
    else { mask = 0; }

    return mask;
}

uint64_t iGPIO_getBcmPinsMask()
{
    uint64_t mask = 0;

    const RPIHAL_model_t hwModel = RPIHAL_getModel();

    if (RPIHAL_model_SoC_peripheral_is_bcm283x(hwModel)) { mask = BCM283x_PINS_MASK; }
    else if (RPIHAL_model_SoC_peripheral_is_bcm2711(hwModel)) { mask = BCM2711_PINS_MASK; }
    else { mask = 0; }

    return mask;
}

void iGPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct)
{
    initStruct->mode = RPIHAL_GPIO_MODE_IN;
    initStruct->pull = RPIHAL_GPIO_PULL_DOWN;
    // initStruct->drive = RPIHAL_GPIO_DRIVE_2mA;
    initStruct->altfunc = RPIHAL_GPIO_AF_0;
}

int iGPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    const RPIHAL_model_t hwModel = RPIHAL_getModel();

    initStruct->mode = RPIHAL_GPIO_MODE_IN;

    int lastPin;
    if (RPIHAL_model_SoC_peripheral_is_bcm283x(hwModel)) { lastPin = 53; }
    else if (RPIHAL_model_SoC_peripheral_is_bcm2711(hwModel)) { lastPin = 57; }
    else
    {
        lastPin = 57;
        r = 1;
    }

    if (((pin >= 0) && (pin <= 8)) || ((pin >= 34) && (pin <= 36)) || ((pin >= 46) && (pin <= lastPin))) { initStruct->pull = RPIHAL_GPIO_PULL_UP; }
    else if (((pin >= 9) && (pin <= 27)) || ((pin >= 30) && (pin <= 33)) || ((pin >= 37) && (pin <= 43))) { initStruct->pull = RPIHAL_GPIO_PULL_DOWN; }
    else initStruct->pull = RPIHAL_GPIO_PULL_NONE;

    initStruct->altfunc = RPIHAL_GPIO_AF_0;

    return r;
}

int iGPIO_bittopin(uint64_t bit)
{
    const int end = 64; // the bit parameter has a 64bit wide type

    for (int pin = 0; pin < end; ++pin)
    {
        if (bit & (1ull << pin)) { return pin; }
    }

    return -1;
}

#undef LOG_MODULE_LEVEL
#undef LOG_MODULE_NAME
#undef BCM283x_PINS_MASK
#undef BCM2711_PINS_MASK
#undef USER_PINS_MASK_26pin_rev1
#undef USER_PINS_MASK_26pin_rev2_P1
#undef USER_PINS_MASK_26pin_rev2_P5
#undef USER_PINS_MASK_26pin_rev2
#undef USER_PINS_MASK_40pin

#endif // iGPIO_DEFINE_FUNCTIONS
