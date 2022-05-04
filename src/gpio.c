/*
author          Oliver Blaser
date            04.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#include <stddef.h>
#include <stdint.h>

#include "rpihal/gpio.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

// 
// BCM defines
// 

#define BCM_REGISTER_PASSWORD       0x5A000000
#define BCM_BLOCK_SIZE              (4*1024)

#define PERI_ADR_BASE_BCM2835       0x20000000
#define PERI_ADR_BASE_BCM2836       0x3F000000
#define PERI_ADR_BASE_BCM2837       0x3F000000
#define PERI_ADR_BASE_BCM2711       0xFE000000

#define PERI_ADR_OFFSET_GPIO        0x00200000 // BCM283x and BCM2711

#define GPIO_PIN_FIRST   (0)
#define GPIO_PIN_LAST    (53)



// 
// BCM register defines
// 

#define GP_REG(offs)    (*(mem_gpio_base + offs))

#define GPFSEL0     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[0]))
#define GPFSEL1     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[1]))
#define GPFSEL2     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[2]))
#define GPFSEL3     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[3]))
#define GPFSEL4     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[4]))
#define GPFSEL5     (*(mem_gpio_base + GPFSEL_ADR_OFFSET[5]))
static const int GPFSEL_ADR_OFFSET[] = { 0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014 };

#define GPSET0      (*(mem_gpio_base + GPSET_ADR_OFFSET[0]))
#define GPSET1      (*(mem_gpio_base + GPSET_ADR_OFFSET[1]))
static const int GPSET_ADR_OFFSET[] = { 0x001C, 0x0020 };

#define GPCLR0      (*(mem_gpio_base + GPCLR_ADR_OFFSET[0]))
#define GPCLR1      (*(mem_gpio_base + GPCLR_ADR_OFFSET[1]))
static const int GPCLR_ADR_OFFSET[] = { 0x0028, 0x002C };

#define GPLEV0      (*(mem_gpio_base + GPLEV_ADR_OFFSET[0]))
#define GPLEV1      (*(mem_gpio_base + GPLEV_ADR_OFFSET[1]))
static const int GPLEV_ADR_OFFSET[] = { 0x0034, 0x0038 };

//#define GPEDS0      (*(mem_gpio_base + 0x0040))
//#define GPEDS1      (*(mem_gpio_base + 0x0044))

//#define GPREN0      (*(mem_gpio_base + 0x004C))
//#define GPREN1      (*(mem_gpio_base + 0x0050))

//#define GPFEN0      (*(mem_gpio_base + 0x0058))
//#define GPFEN1      (*(mem_gpio_base + 0x005C))

//#define GPHEN0      (*(mem_gpio_base + 0x0064))
//#define GPHEN1      (*(mem_gpio_base + 0x0068))

//#define GPLEN0      (*(mem_gpio_base + 0x0070))
//#define GPLEN1      (*(mem_gpio_base + 0x0074))

//#define GPAREN0     (*(mem_gpio_base + 0x007C))
//#define GPAREN1     (*(mem_gpio_base + 0x0080))

//#define GPAFEN0     (*(mem_gpio_base + 0x0088))
//#define GPAFEN1     (*(mem_gpio_base + 0x008C))

#define GPPUD       (*(mem_gpio_base + 0x0094))
#define GPPUDCLK0   (*(mem_gpio_base + GPPUDCLK_ADR_OFFSET[0]))
#define GPPUDCLK1   (*(mem_gpio_base + GPPUDCLK_ADR_OFFSET[1]))
static const int GPPUDCLK_ADR_OFFSET[] = { 0x0098, 0x009C };


#define FSEL_IN     (0b000)
#define FSEL_OUT    (0b001)
#define FSEL_AF0    (0b100)
#define FSEL_AF1    (0b101)
#define FSEL_AF2    (0b110)
#define FSEL_AF3    (0b111)
#define FSEL_AF4    (0b011)
#define FSEL_AF5    (0b010)
static const uint32_t FSEL_AF_LUT[] = {
    FSEL_AF0, FSEL_AF1, FSEL_AF2, FSEL_AF3, FSEL_AF4, FSEL_AF5 }; // FSEL alternate function look up table



static volatile uint32_t* mem_gpio_base = NULL; // = PERI_ADR_BASE_x + PERI_ADR_OFFSET_GPIO
static int usingGpiomem = -1;



static int checkPin(int pin);
static void initPin(int pin, const GPIO_init_t* initStruct);
static int readPin(int pin);
static void writePin(int pin, int state);



int GPIO_init()
{
    int r = 0;

    if(!mem_gpio_base)
    {
        int fd;
        off_t mmapoffs;

        fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC); // no root access needed

        if(fd >= 0)
        {
            mmapoffs = 0;
            usingGpiomem = 1;
        }
        else
        {
            mmapoffs = PERI_ADR_BASE_BCM2837 + PERI_ADR_OFFSET_GPIO; // !!! DEVICE SPECIFIC !!! DEVSPEC
            usingGpiomem = 0;

            fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC); // root access needed

            if(fd < 0) r = 1;
        }

        if(!r)
        {
            mem_gpio_base = mmap(0, BCM_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mmapoffs);

            if(mem_gpio_base == MAP_FAILED)
            {
                r = 2;
                mem_gpio_base = NULL;
            }
        }
    }

    return r;
}

int GPIO_initPin(int pin, const GPIO_init_t* initStruct)
{
    int r = 0; printf("GPIO_initPin %i %i %i\n",mem_gpio_base , initStruct , checkPin(pin));

    if(mem_gpio_base && initStruct && checkPin(pin))
    {
        initPin(pin, initStruct);
    }
    else r = 1;
printf("GPIO_initPin return %i\n", r);
    return r;
}

int GPIO_readPin(int pin)
{
    int r = 0;

    if(mem_gpio_base && checkPin(pin)) r = readPin(pin);
    else r = -1;

    return r;
}

int GPIO_writePin(int pin, int state)
{
    int r = 0;

    if(mem_gpio_base && checkPin(pin)) writePin(pin, state);
    else r = 1;

    return r;
}

int GPIO_togglePin(int pin)
{
    int r = 0;

    if(mem_gpio_base && checkPin(pin)) writePin(pin, !readPin(pin));
    else r = 1;

    return r;
}

int GPIO_defaultInitStruct(GPIO_init_t* initStruct)
{
    int r = 0;

    if(initStruct)
    {
        initStruct->mode = GPIO_MODE_IN;
        initStruct->pull = GPIO_PULL_NONE;
        initStruct->altfunc = GPIO_AF_0;
    }
    else r = 1;

    return r;
}

volatile uint32_t* GPIO_getMemBasePtr()
{
    return mem_gpio_base;
}

int GPIO_isUsingGpiomem()
{
    return usingGpiomem;
}



int checkPin(int pin)
{
    if((pin >= GPIO_PIN_FIRST) && (pin <= GPIO_PIN_LAST))
        return 1;

    return 0;
}

void initPin(int pin, const GPIO_init_t* initStruct)
{
    uint32_t fselMask = FSEL_IN;
    const int fselRegOffs = GPFSEL_ADR_OFFSET[pin / 10];

    if(initStruct->mode == GPIO_MODE_OUT) fselMask = FSEL_OUT;
    else if(initStruct->mode == GPIO_MODE_AF) fselMask = FSEL_AF_LUT[initStruct->altfunc];
    else fselMask = FSEL_IN;

    fselMask <<= ((pin % 10) * 3);

    GP_REG(fselRegOffs) &= fselMask;
    GP_REG(fselRegOffs) |= fselMask;



    const int pudclkRegOffs = GPPUDCLK_ADR_OFFSET[pin / 32];
    const uint32_t pudclkValue = 1 << (pin % 32);

    GPPUD = (uint32_t)(initStruct->pull);
    if(!usleep(5)) for(int i = 0; i < 200; ++i) fselMask += 2;
    GP_REG(pudclkRegOffs) = pudclkValue;
    if(!usleep(5)) for(int i = 0; i < 200; ++i) fselMask += 2;
    GPPUD = 0;
    if(!usleep(5)) for(int i = 0; i < 200; ++i) fselMask += 2; // shouldn't be necesary, to be tested without
    GP_REG(pudclkRegOffs) = 0;
    if(!usleep(5)) for(int i = 0; i < 200; ++i) fselMask += 2; // shouldn't be necesary, to be tested without



    // drive
}

int readPin(int pin)
{
    int r;
    const int offs = GPLEV_ADR_OFFSET[pin / 32];
    const uint32_t bit = 1 << (pin % 32);

    if(GP_REG(offs) & bit) r = 1;
    else r = 0;

    return r;
}

void writePin(int pin, int state)
{
    const uint32_t bit = 1 << (pin % 32);

    if(state) GP_REG(GPSET_ADR_OFFSET[pin / 32]) |= bit;
    else GP_REG(GPCLR_ADR_OFFSET[pin / 32]) |= bit;
}
