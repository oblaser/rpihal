/*
author          Oliver Blaser
date            17.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

// std includes
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// prj includes
#include "ionoPi.h"

// lib includes
#include <rpihal/gpio.h>


#define BTN_STAIRS_PIN      IONOPI_DI3
#define BTN_BROOM_1_PIN     IONOPI_DI4
#define BTN_BROOM_2_PIN     IONOPI_DI5

#define BTN_STAIRS_BIT      (1 << BTN_STAIRS_PIN)
#define BTN_BROOM_1_BIT     (1 << BTN_BROOM_1_PIN)
#define BTN_BROOM_2_BIT     (1 << BTN_BROOM_2_PIN)

#define LIGHT_STAIRS_PIN    IONOPI_O2
#define LIGHT_BROOM_1_PIN   IONOPI_O3
#define LIGHT_BROOM_2_PIN   IONOPI_O4

enum { STAIRS_TIME = 10 };


static int initGpio();


int main(int argc, char** argv)
{
    int r = -1;
    uint32_t inp, inp_old, inp_pos; //, inp_neg;
    time_t tOld = 0, tNow;
    int tmr_stairs = 0;

    if(initGpio() != 0) return 1;

    r = 0;
    inp_old = GPIO_read();

    while(1)
    {
        // edge detection (hardware debounce needed, e.g. RC filter)
        inp = GPIO_read();
        inp_pos = ~inp_old & inp;
        //inp_neg = inp_old & ~inp;
        inp_old = inp;

        if(inp_pos & BTN_BROOM_1_BIT) GPIO_togglePin(LIGHT_BROOM_1_PIN);
        if(inp_pos & BTN_BROOM_2_BIT) GPIO_togglePin(LIGHT_BROOM_2_PIN);

        if(inp_pos & BTN_STAIRS_BIT)
        {
            GPIO_writePin(LIGHT_STAIRS_PIN, 1);
            tmr_stairs = STAIRS_TIME;
        }

        if(tmr_stairs == 0)
        {
            GPIO_writePin(LIGHT_STAIRS_PIN, 0);
            tmr_stairs = -1;
        }

        tNow = time(NULL);
        if(tOld != tNow)
        {
            tOld = tNow;
            if(tmr_stairs > 0) --tmr_stairs;
        }
    }

    return r;
}


int initGpio()
{
    if(GPIO_init() != 0)
    {
        printf("GPIO init failed!\n");
        return 1;
    }

    GPIO_init_t initStruct;
    GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(LIGHT_STAIRS_PIN, &initStruct);
    GPIO_initPin(LIGHT_BROOM_1_PIN, &initStruct);
    GPIO_initPin(LIGHT_BROOM_2_PIN, &initStruct);

    initStruct.mode = GPIO_MODE_IN;
    GPIO_initPin(BTN_STAIRS_PIN, &initStruct);
    GPIO_initPin(BTN_BROOM_1_PIN, &initStruct);
    GPIO_initPin(BTN_BROOM_2_PIN, &initStruct);

    return 0;
}
