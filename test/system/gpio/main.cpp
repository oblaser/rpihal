/*
author          Oliver Blaser
date            11.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <iostream>

#include "rpihal/gpio.h"
#include <unistd.h>


// using my WP board
#define BTN_0   4
#define BTN_1   17
#define LED_0   22
#define LED_1   10
#define RELAY_0 11
#define RELAY_1 19
#define RELAY_2 26
#define RELAY_3 21


using std::cout;
using std::endl;


int main(int argc, char** argv)
{
    cout << "init     " << GPIO_init() << endl;
    cout << "gpiomem  " << (GPIO_isUsingGpiomem() ? "true" : "false") << endl;

    GPIO_init_t initStruct;
    GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(LED_0, &initStruct);
    GPIO_initPin(LED_1, &initStruct);
    GPIO_initPin(RELAY_0, &initStruct);
    GPIO_initPin(RELAY_1, &initStruct);
    GPIO_initPin(RELAY_2, &initStruct);
    GPIO_initPin(RELAY_3, &initStruct);

    initStruct.mode = GPIO_MODE_IN;
    GPIO_initPin(BTN_0, &initStruct);
    GPIO_initPin(BTN_1, &initStruct);

    if (GPIO_initPin(0, &initStruct)) cout << "init pin 0 failed, which is right" << endl;
    else cout << "init pin 0 should have failed!" << endl;

    uint32_t cnt = 0;

    while (true)
    {
        GPIO_togglePin(RELAY_0);
        usleep(50 * 1000);

        if (GPIO_readPin(BTN_0)) GPIO_writePin(LED_0, 1);
        else GPIO_writePin(LED_0, 0);

        GPIO_writePin(LED_1, GPIO_readPin(BTN_1));

        ++cnt;
        GPIO_writePin(RELAY_1, cnt & 2);
        GPIO_writePin(RELAY_2, cnt & 4);
        GPIO_writePin(RELAY_3, cnt & 8);
    }

    return 0;
}
