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



// 
// GPIO register offsets
// 

#define GPFSEL0     0x0000
#define GPFSEL1     0x0004
#define GPFSEL2     0x0008
#define GPFSEL3     0x000C
#define GPFSEL4     0x0010
#define GPFSEL5     0x0014

#define GPSET0      0x001C
#define GPSET1      0x0020

#define GPCLR0      0x0028
#define GPCLR1      0x002C

#define GPLEV0      0x0034
#define GPLEV1      0x0038

#define GPEDS0      0x0040
#define GPEDS1      0x0044

#define GPREN0      0x004C
#define GPREN1      0x0050

#define GPFEN0      0x0058
#define GPFEN1      0x005C

#define GPHEN0      0x0064
#define GPHEN1      0x0068

#define GPLEN0      0x0070
#define GPLEN1      0x0074

#define GPAREN0     0x007C
#define GPAREN1     0x0080

#define GPAFEN0     0x0088
#define GPAFEN1     0x008C

#define GPPUD       0x0094
#define GPPUDCLK0   0x0098
#define GPPUDCLK1   0x009C


#define FSEL_MASK   (0b111)
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



static RPIHAL_regptr_t mem_gpio_base = NULL; // = PERI_ADR_BASE_x + PERI_ADR_OFFSET_GPIO
static int usingGpiomem = -1;
static int sysGpioLocked = 1;



static int checkPin(int pin);
static void initPin(int pin, const GPIO_init_t* initStruct);
static int readPin(int pin);
static void writePin(int pin, int state);



//! @return __0__ on success
//! 
//! Needs to be called once at the start of the app.
//! 
int GPIO_init()
{
    int r = 0;

    if (!mem_gpio_base)
    {
        int fd;
        off_t mmapoffs;

        fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC); // no root access needed

        if (fd >= 0)
        {
            mmapoffs = 0;
            usingGpiomem = 1;
        }
        else
        {
            mmapoffs = PERI_ADR_BASE_BCM2837 + PERI_ADR_OFFSET_GPIO; // !!! DEVICE SPECIFIC !!! DEVSPEC
            usingGpiomem = 0;

            fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC); // root access needed

            if (fd < 0) r = 1;
        }

        if (!r)
        {
            mem_gpio_base = (RPIHAL_regptr_t)mmap(0, BCM_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mmapoffs);

            if (mem_gpio_base == MAP_FAILED)
            {
                r = 2;
                mem_gpio_base = NULL;
            }
        }
    }

    return r;
}

//! @param pin BCM GPIO pin number
//! @param initStruct Pointer to the pin settings
//! @return __0__ on success
int GPIO_initPin(int pin, const GPIO_init_t* initStruct)
{
    int r = 0;

    if (mem_gpio_base && initStruct && checkPin(pin))
    {
        initPin(pin, initStruct);
    }
    else r = 1;

    return r;
}

//! @param pin BCM GPIO pin number
//! @return __0__ on success
int GPIO_readPin(int pin)
{
    int r = 0;

    if (mem_gpio_base && checkPin(pin)) r = readPin(pin);
    else r = -1;

    return r;
}

//! @param pin BCM GPIO pin number
//! @param state Boolean value representing the pin states HIGH (`1`) and LOW (`0`)
//! @return __0__ on success
int GPIO_writePin(int pin, int state)
{
    int r = 0;

    if (mem_gpio_base && checkPin(pin)) writePin(pin, state);
    else r = 1;

    return r;
}

//! @param pin BCM GPIO pin number
//! @return __0__ on success
int GPIO_togglePin(int pin)
{
    int r = 0;

    if (mem_gpio_base && checkPin(pin)) writePin(pin, !readPin(pin));
    else r = 1;

    return r;
}

//! @param initStruct Pointer to the pin settings which will be set to the default values
//! @return __0__ on success
int GPIO_defaultInitStruct(GPIO_init_t* initStruct)
{
    int r = 0;

    if (initStruct)
    {
        initStruct->mode = GPIO_MODE_IN;
        initStruct->pull = GPIO_PULL_NONE;
        initStruct->altfunc = GPIO_AF_0;
    }
    else r = 1;

    return r;
}

RPIHAL_regptr_t GPIO_getMemBasePtr()
{
    return mem_gpio_base;
}

//! @return TRUE (`1`), FALSE (`0`) or unknown (`-1`)
int GPIO_isUsingGpiomem()
{
    return usingGpiomem;
}



uint32_t BCM2835_reg_read(RPIHAL_regptr_t addr)
{
    // the last read could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    uint32_t value = *addr;
    uint32_t garbage __attribute__((unused)) = *addr;
    return value;
}
void BCM2835_reg_write(RPIHAL_regptr_t addr, uint32_t value)
{
    // the first write could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    *addr = value;
    *addr = value;
}
void BCM2835_reg_write_bits(RPIHAL_regptr_t addr, uint32_t value, uint32_t mask)
{
    uint32_t regval = BCM2835_reg_read(addr);
    regval &= ~mask;
    regval |= (value & mask);
    BCM2835_reg_write(addr, regval);
}



//! @param pin BCM GPIO pin number
//! @return Boolean value TRUE (`1`) if it's allowed to access the pin, otherwhise FALSE (`0`)
int checkPin(int pin)
{
    // !!! DEVICE SPECIFIC !!! DEVSPEC - valid for RasPi (2|3|4) B[+]

    int r = 0;

    if (sysGpioLocked)
    {
        if ((pin >= 2) && (pin <= 27)) r = 1;
    }
    else
    {
        if ((pin >= 0) && (pin <= 53)) r = 1;
    }

    return r;
}

void initPin(int pin, const GPIO_init_t* initStruct)
{
    RPIHAL_regptr_t addr;
    int shift;
    uint32_t value;
    uint32_t mask;



    // fsel

    addr = mem_gpio_base + (GPFSEL0 / 4) + (pin / 10);
    shift = 3 * (pin % 10);
    if (initStruct->mode == GPIO_MODE_OUT) value = FSEL_OUT;
    else if (initStruct->mode == GPIO_MODE_AF) value = FSEL_AF_LUT[initStruct->altfunc];
    else value = FSEL_IN;
    value <<= shift;
    mask = FSEL_MASK << shift;
    BCM2835_reg_write_bits(addr, value, mask);



    // pud

    addr = mem_gpio_base + (GPPUD / 4);
    value = (uint32_t)(initStruct->pull);
    mask = 0b11;
    BCM2835_reg_write_bits(addr, value, mask);

    if (!usleep(5)) for (int i = 0; i < 200; ++i) shift += 2;

    addr = mem_gpio_base + (GPPUDCLK0 / 4) + (pin / 32);
    shift = (pin % 32);
    value = 1 << shift;
    mask = 1 << shift;
    BCM2835_reg_write_bits(addr, value, mask);

    if (!usleep(5)) for (int i = 0; i < 200; ++i) shift += 2;

    addr = mem_gpio_base + (GPPUD / 4);
    value = 0;
    mask = 0b11;
    BCM2835_reg_write_bits(addr, value, mask);

    if (!usleep(5)) for (int i = 0; i < 200; ++i) shift += 2; // shouldn't be necesary, to be tested without

    addr = mem_gpio_base + (GPPUDCLK0 / 4) + (pin / 32);
    BCM2835_reg_write(addr, 0);

    if (!usleep(5)) for (int i = 0; i < 200; ++i) shift += 2; // shouldn't be necesary, to be tested without



    // drive
}

int readPin(int pin)
{
    int r;
    RPIHAL_regptr_t addr = mem_gpio_base + (GPLEV0 / 4) + (pin / 32);
    const uint32_t bit = 1 << (pin % 32);

    if (BCM2835_reg_read(addr) & bit) r = 1;
    else r = 0;

    return r;
}

void writePin(int pin, int state)
{
    RPIHAL_regptr_t addr = mem_gpio_base + (pin / 32);
    const uint32_t bit = 1 << (pin % 32);

    if (state) addr += (GPSET0 / 4);
    else addr += (GPCLR0 / 4);

    BCM2835_reg_write(addr, bit);
}
