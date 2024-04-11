/*
author          Oliver Blaser
date            17.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ionoPi.h"

#include <rpihal/gpio.h>
#include <rpihal/rpihal.h>
#include <rpihal/sys.h>


#define LED_STATUS IONOPI_L1

#define BTN_STAIRS_PIN  IONOPI_DI3
#define BTN_BROOM_1_PIN IONOPI_DI4
#define BTN_BROOM_2_PIN IONOPI_DI5
#define BTN_BROOM_3_PIN IONOPI_DI6

#define BTN_STAIRS_BIT  (1 << BTN_STAIRS_PIN)
#define BTN_BROOM_1_BIT (1 << BTN_BROOM_1_PIN)
#define BTN_BROOM_2_BIT (1 << BTN_BROOM_2_PIN)
#define BTN_BROOM_3_BIT (1 << BTN_BROOM_3_PIN)

#define LIGHT_STAIRS_PIN  IONOPI_O2
#define LIGHT_BROOM_1_PIN IONOPI_O3
#define LIGHT_BROOM_2_PIN IONOPI_O4
#define LIGHT_BROOM_3_PIN IONOPI_O1


static const int STAIRS_TIME = 10;


static int initGpio();


int main(int argc, char** argv)
{
    int r = -1;

    uint32_t inp, inp_old, inp_pos, inp_neg;
    time_t tOld = 0;
    int tmr_stairs = 0;

    RPIHAL_EMU_init();

    if (initGpio() != 0) return 1;

    r = 0;
    inp_old = RPIHAL_GPIO_read();

#ifdef RPIHAL_EMU
    while (!RPIHAL_EMU_isRunning()) {}

    while (RPIHAL_EMU_isRunning()) // for convenience, exit if the emulator window is closed
#else
    while (1)
#endif
    {
        // edge detection (hardware debounce needed, e.g. RC filter)
        inp = RPIHAL_GPIO_read();
        inp_pos = ~inp_old & inp;
        inp_neg = inp_old & ~inp;
        inp_old = inp;

        if (inp_pos & BTN_BROOM_1_BIT) RPIHAL_GPIO_togglePin(LIGHT_BROOM_1_PIN);
        if (inp_pos & BTN_BROOM_2_BIT) RPIHAL_GPIO_togglePin(LIGHT_BROOM_2_PIN);

        if (inp_pos & BTN_BROOM_3_BIT)
        {
            printf("room 3 switched ON\n");

            RPIHAL_GPIO_writePin(LIGHT_BROOM_3_PIN, 1);
        }

        if (inp_neg & BTN_BROOM_3_BIT)
        {
            printf("room 3 switched OFF\n");

            RPIHAL_GPIO_writePin(LIGHT_BROOM_3_PIN, 0);
        }

        if (inp_pos & BTN_STAIRS_BIT)
        {
            RPIHAL_GPIO_writePin(LIGHT_STAIRS_PIN, 1);
            tmr_stairs = STAIRS_TIME;
        }

        if (tmr_stairs == 0)
        {
            RPIHAL_GPIO_writePin(LIGHT_STAIRS_PIN, 0);
            tmr_stairs = -1;
        }



        static float oldTemp = 0;
        float temp;
        RPIHAL_SYS_getCpuTemp(&temp);

        if (oldTemp != temp)
        {
            oldTemp = temp;

            printf("CPU temp: %.2f%sC\n", temp,
#ifdef _WIN32
                   "\xF8"
#else
                   "\xC2\xB0"
#endif
            );
        }



        const time_t tNow = time(NULL);

        if (tOld != tNow)
        {
            tOld = tNow;

            RPIHAL_GPIO_togglePin(LED_STATUS);

            if (tmr_stairs > 0) --tmr_stairs;
        }
    }

    RPIHAL_EMU_cleanup();

    return r;
}


int initGpio()
{
    if (RPIHAL_GPIO_init() != 0)
    {
        printf("GPIO init failed!\n");
        return 1;
    }

    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    RPIHAL_GPIO_initPin(LED_STATUS, &initStruct);
    RPIHAL_GPIO_initPin(LIGHT_STAIRS_PIN, &initStruct);
    RPIHAL_GPIO_initPin(LIGHT_BROOM_1_PIN, &initStruct);
    RPIHAL_GPIO_initPin(LIGHT_BROOM_2_PIN, &initStruct);
    RPIHAL_GPIO_initPin(LIGHT_BROOM_3_PIN, &initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_IN;
    RPIHAL_GPIO_initPin(BTN_STAIRS_PIN, &initStruct);
    RPIHAL_GPIO_initPin(BTN_BROOM_1_PIN, &initStruct);
    RPIHAL_GPIO_initPin(BTN_BROOM_2_PIN, &initStruct);
    RPIHAL_GPIO_initPin(BTN_BROOM_3_PIN, &initStruct);

    return 0;
}
