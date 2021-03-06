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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


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
    int altfunc;
} RPIHAL_GPIO_init_t;


int RPIHAL_GPIO_init();
int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct);
int RPIHAL_GPIO_readPin(int pin);
uint32_t RPIHAL_GPIO_read();
int RPIHAL_GPIO_writePin(int pin, int state);
int RPIHAL_GPIO_set(uint32_t bits);
int RPIHAL_GPIO_clr(uint32_t bits);
int RPIHAL_GPIO_togglePin(int pin);
int RPIHAL_GPIO_reset();
int RPIHAL_GPIO_resetPin(int pin);
int RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct);
int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct);
RPIHAL_regptr_t RPIHAL_GPIO_getMemBasePtr();
int RPIHAL_GPIO_isUsingGpiomem();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_GPIO_H
