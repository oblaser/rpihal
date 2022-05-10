/*
author          Oliver Blaser
date            04.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#ifndef IG_RPIHAL_GPIO_H
#define IG_RPIHAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define GPIO_MODE_IN    0
#define GPIO_MODE_OUT   1
#define GPIO_MODE_AF    2

#define GPIO_PULL_NONE  (0b00)
#define GPIO_PULL_DOWN  (0b01)
#define GPIO_PULL_UP    (0b10)

//#define GPIO_DRIVE_2mA      0
//#define GPIO_DRIVE_4mA      1
//#define GPIO_DRIVE_6mA      2
//#define GPIO_DRIVE_8mA      3
//#define GPIO_DRIVE_10mA     4
//#define GPIO_DRIVE_12mA     5
//#define GPIO_DRIVE_14mA     6
//#define GPIO_DRIVE_16mA     7

#define GPIO_AF_0   0
#define GPIO_AF_1   1
#define GPIO_AF_2   2
#define GPIO_AF_3   3
#define GPIO_AF_4   4
#define GPIO_AF_5   5

typedef volatile uint32_t* RPIHAL_regptr_t;

typedef struct
{
    int mode;
    int pull;
    //int drive;
    int altfunc;
} GPIO_init_t;

int GPIO_init();
int GPIO_initPin(int pin, const GPIO_init_t* initStruct);
int GPIO_readPin(int pin);
int GPIO_writePin(int pin, int state);
int GPIO_togglePin(int pin);
int GPIO_defaultInitStruct(GPIO_init_t* initStruct);
RPIHAL_regptr_t GPIO_getMemBasePtr();
int GPIO_isUsingGpiomem();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_GPIO_H
