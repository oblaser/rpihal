/*
author          Oliver Blaser
date            04.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <iostream>

#include "rpihal/gpio.h"
#include <unistd.h>


using std::cout;
using std::endl;



int main(int argc, char** argv)
{
    cout << "init     " << GPIO_init() << endl;
    cout << "gpiomem  " << GPIO_isUsingGpiomem() << endl;

    GPIO_init_t initStruct;
    GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(10, &initStruct);
    GPIO_initPin(26, &initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(4, &initStruct);

    while(true)
    {
        GPIO_togglePin(26);
        usleep(50*1000);

        if(GPIO_readPin(4)) GPIO_writePin(10, 1);
        else GPIO_writePin(10, 0);
    }

    return 0;
}
