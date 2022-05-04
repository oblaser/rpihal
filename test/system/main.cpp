/*
author          Oliver Blaser
date            04.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <iostream>

#include "rpihal/gpio.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    cout << "init     " << GPIO_init() << endl;
    cout << "gpiomem  " << GPIO_isUsingGpiomem() << endl;

    return 0;
}
