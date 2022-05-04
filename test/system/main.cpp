/*
author          Oliver Blaser
date            04.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <iostream>

//#include "rpihal/gpio.h"
#include <unistd.h>







using std::cout;
using std::endl;


#if 1

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

typedef volatile uint32_t* RPIHAL_regptr_t;

uint32_t reg_read(RPIHAL_regptr_t addr)
{
    // the last read could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    uint32_t value = *addr;
    uint32_t garbage __attribute__((unused)) = *addr;
    return value;
}

void reg_write(RPIHAL_regptr_t addr, uint32_t value)
{
    // the first write could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    *addr = value;
    *addr = value;
}

void reg_set_bits(RPIHAL_regptr_t addr, uint32_t value, uint32_t mask)
{
    uint32_t regval = reg_read(addr);
    regval &= ~mask;
    regval |= (value & mask);
    reg_write(addr, regval);
}

int main()
{
    cout << "start" << endl;

    int fd;
    volatile uint32_t* gpiomem = NULL;
    int ok = 0;

#if 0
    const char* devfile = "/dev/mem";
    const off_t mapoffs = 0x3F000000 + 0x00200000;
#else
    const char* devfile = "/dev/gpiomem";
    const off_t mapoffs = 0;
#endif

    fd = open(devfile, O_RDWR | O_SYNC | O_CLOEXEC);
    cout << "open \"" << devfile << "\"    " << fd << endl;

    if(fd >= 0)
    {
        gpiomem = (uint32_t*)mmap(0, (1024*4), PROT_READ | PROT_WRITE, MAP_SHARED, fd, mapoffs);

        cout << "gpiomem " << (void*)gpiomem << endl;

        if(gpiomem == MAP_FAILED) cout << "map failed" << endl;
        else ok = 1;
    }

    if(ok)
    {
        cout << "\nOK, lets try blink\n" << endl;

        reg_set_bits((gpiomem + 0x0004/4), 0b001000, 0b111000);
        cout << "GPIO 11 is now an output" << endl;

loop:

        reg_write((gpiomem + 0x001C/4), (1 << 11));
        //cout << "GPIO 11 is now on" << endl;

        reg_write((gpiomem + 0x0028/4), (1 << 11));
        //cout << "GPIO 11 is now off" << endl;

        goto loop;
    }

    return 0;
}

#else

int main(int argc, char** argv)
{
    cout << "init     " << GPIO_init() << endl;
    cout << "gpiomem  " << GPIO_isUsingGpiomem() << endl;

    GPIO_init_t initStruct;
    GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(10, &initStruct);
/*    GPIO_initPin(26, &initStruct);

    initStruct.mode = GPIO_MODE_OUT;
    GPIO_initPin(4, &initStruct);

    cout << 

    while(true)
    {
        GPIO_togglePin(26);
        usleep(50*1000);

        if(GPIO_readPin(4)) GPIO_writePin(10, 1);
        else GPIO_writePin(10, 0);
    }
*/
    return 0;
}
#endif
