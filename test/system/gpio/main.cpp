/*
author          Oliver Blaser
date            11.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <iostream>

#include <rpihal/gpio.h>
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
    cout << "init     " << RPIHAL_GPIO_init() << endl;
    cout << "gpiomem  " << (RPIHAL_GPIO_isUsingGpiomem() ? "true" : "false") << endl;

    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    RPIHAL_GPIO_initPin(LED_0, &initStruct);
    RPIHAL_GPIO_initPin(LED_1, &initStruct);
    RPIHAL_GPIO_initPin(RELAY_0, &initStruct);
    RPIHAL_GPIO_initPin(RELAY_1, &initStruct);
    RPIHAL_GPIO_initPin(RELAY_2, &initStruct);
    RPIHAL_GPIO_initPin(RELAY_3, &initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_IN;
    RPIHAL_GPIO_initPin(BTN_0, &initStruct);
    RPIHAL_GPIO_initPin(BTN_1, &initStruct);

    if (RPIHAL_GPIO_initPin(0, &initStruct)) cout << "init pin 0 failed, which is right" << endl;
    else cout << "init pin 0 should have failed!" << endl;

    uint32_t cnt = 0;

    while (true)
    {
        RPIHAL_GPIO_togglePin(RELAY_0);
        usleep(50 * 1000);

        if (RPIHAL_GPIO_readPin(BTN_0)) RPIHAL_GPIO_writePin(LED_0, 1);
        else RPIHAL_GPIO_writePin(LED_0, 0);

        RPIHAL_GPIO_writePin(LED_1, RPIHAL_GPIO_readPin(BTN_1));

        ++cnt;
        RPIHAL_GPIO_writePin(RELAY_1, cnt & 2);
        RPIHAL_GPIO_writePin(RELAY_2, cnt & 4);
        RPIHAL_GPIO_writePin(RELAY_3, cnt & 8);
    }

    return 0;
}
