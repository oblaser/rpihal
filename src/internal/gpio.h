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

#ifndef IG_RPIHAL_INTERNAL_GPIO_H
#define IG_RPIHAL_INTERNAL_GPIO_H


#define BCM2835_PINS_MASK (0x003FFFFFFFFFFFFFull)
// #define BCM2835_FIRST_PIN (0)
// #define BCM2835_LAST_PIN  (53)

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


#endif // IG_RPIHAL_INTERNAL_GPIO_H
