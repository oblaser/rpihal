/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

/*

Copyright (c) 2024 Oliver Blaser

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <stddef.h>
#include <stdint.h>

#include "internal/gpio.h"
#include "internal/platform_check.h"
#include "rpihal/gpio.h"
#include "rpihal/rpihal.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  GPIO
#include "internal/log.h"



//======================================================================================================================
//  BCM abstraction

#define BCM_REGISTER_PASSWORD (0x5A000000u)
#define BCM_BLOCK_SIZE        (4 * 1024)
// #define BCM_PAGE_SIZE      (4 * 1024)

#define PERI_ADR_BASE_BCM2835   (0x20000000u)
#define PERI_ADR_BASE_BCM2836_7 (0x3F000000u)
#define PERI_ADR_BASE_BCM2836   PERI_ADR_BASE_BCM2836_7
#define PERI_ADR_BASE_BCM2837   PERI_ADR_BASE_BCM2836_7
#define PERI_ADR_BASE_BCM2711   (0xFE000000u)

#define PERI_ADR_OFFSET_GPIO (0x00200000u) // BCM283x and BCM2711



// GPIO register offsets

#define GPFSEL0 (0x0000)
#define GPFSEL1 (0x0004)
#define GPFSEL2 (0x0008)
#define GPFSEL3 (0x000C)
#define GPFSEL4 (0x0010)
#define GPFSEL5 (0x0014)

#define GPSET0 (0x001C)
#define GPSET1 (0x0020)

#define GPCLR0 (0x0028)
#define GPCLR1 (0x002C)

#define GPLEV0 (0x0034)
#define GPLEV1 (0x0038)

#define GPEDS0 (0x0040)
#define GPEDS1 (0x0044)

#define GPREN0 (0x004C)
#define GPREN1 (0x0050)

#define GPFEN0 (0x0058)
#define GPFEN1 (0x005C)

#define GPHEN0 (0x0064)
#define GPHEN1 (0x0068)

#define GPLEN0 (0x0070)
#define GPLEN1 (0x0074)

#define GPAREN0 (0x007C)
#define GPAREN1 (0x0080)

#define GPAFEN0 (0x0088)
#define GPAFEN1 (0x008C)

#define BCM2835_GPPUD     (0x0094)
#define BCM2835_GPPUDCLK0 (0x0098)
#define BCM2835_GPPUDCLK1 (0x009C)

#define BCM2711_GPIO_PUP_PDN_CNTRL_REG0 (0xe4)
#define BCM2711_GPIO_PUP_PDN_CNTRL_REG1 (0xe8)
#define BCM2711_GPIO_PUP_PDN_CNTRL_REG2 (0xec)
#define BCM2711_GPIO_PUP_PDN_CNTRL_REG3 (0xf0)


#define FSEL_MASK (0x07)
#define FSEL_IN   (0x00)
#define FSEL_OUT  (0x01)
#define FSEL_AF0  (0x04) // ANOM1
#define FSEL_AF1  (0x05)
#define FSEL_AF2  (0x06)
#define FSEL_AF3  (0x07)
#define FSEL_AF4  (0x03)
#define FSEL_AF5  (0x02)
static const uint32_t FSEL_AF_LUT[] = { FSEL_AF0, FSEL_AF1, FSEL_AF2, FSEL_AF3, FSEL_AF4, FSEL_AF5 }; // FSEL alternate function look up table

#define BCM2835_PUD_MASK (0x03)
#define BCM2835_PUD_NONE (0x00)
#define BCM2835_PUD_DOWN (0x01)
#define BCM2835_PUD_UP   (0x02)

#define BCM2711_GPIO_PUP_PDN_MASK (0x03)
#define BCM2711_GPIO_PUP_PDN_NONE (0x00)
#define BCM2711_GPIO_PUP_PDN_UP   (0x01)
#define BCM2711_GPIO_PUP_PDN_DOWN (0x02)



// the following functions are the same for bcm2835 and bcm2711

static inline void BCM2835_wait_cycles(size_t cycleCount)
{
    while (--cycleCount) { asm volatile("nop"); }
}

// clang-format off
#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC optimize ("O0")
#else // __GNUC__
#error "compiler yet not supported, please open an issue on GitHub"
#endif // __GNUC__
// clang-format on

static uint32_t BCM2835_reg_read(RPIHAL_regptr_t addr)
{
    // the last read could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    uint32_t value = *addr;
    uint32_t garbage __attribute__((unused)) = *addr;
    return value;
}

static void BCM2835_reg_write(RPIHAL_regptr_t addr, uint32_t value)
{
    // the first write could potentially go wrong (see chapter 1.3 in BCM2835-ARM-Peripherals.pdf).
    *addr = value;
    *addr = value;
}

#ifdef __GNUC__
#pragma GCC pop_options
#endif // __GNUC__

static void BCM2835_reg_write_bits(RPIHAL_regptr_t addr, uint32_t value, uint32_t mask)
{
    uint32_t regval = BCM2835_reg_read(addr);
    regval &= ~mask;
    regval |= (value & mask);
    BCM2835_reg_write(addr, regval);
}


// end BCM abstraction
//======================================================================================================================



static RPIHAL_regptr_t gpio_base = NULL; // = PERI_ADR_BASE_x + PERI_ADR_OFFSET_GPIO
static int usingGpiomem = -1;
static int sysGpioLocked = 1; // ADDHW check for const RPIHAL_model_t hwModel = RPIHAL_getModel(); before unlocking!
                              // may be unlocked on compute modules, illegal to unlock on other models



static int initPin(int pin, const RPIHAL_GPIO_init_t* initStruct);
static int readPin(int pin);
static void writePin(int pin, int state);



int RPIHAL_GPIO_init() // TODO make internal (each function has to check gpio_base)
{
    int r = 0;

    const RPIHAL_model_t hwModel = RPIHAL_getModel();
    off_t mmapoffs;

    // ADDHW bcm2835, bcm2712
    if (RPIHAL_model_SoC_is_bcm2836(hwModel)) { mmapoffs = PERI_ADR_BASE_BCM2836 + PERI_ADR_OFFSET_GPIO; }
    else if (RPIHAL_model_SoC_is_bcm2837_any(hwModel)) { mmapoffs = PERI_ADR_BASE_BCM2837 + PERI_ADR_OFFSET_GPIO; }
    else if (RPIHAL_model_SoC_is_bcm2711(hwModel)) { mmapoffs = PERI_ADR_BASE_BCM2711 + PERI_ADR_OFFSET_GPIO; }
    else
    {
        const char* dt = RPIHAL_dt_model();
        if (!dt) { dt = RPIHAL_dt_compatible(); }
        if (!dt) { dt = "UNKNOWN MODEL"; }

        LOG_ERR("GPIO for %s is not yet supported", dt);

        return -(__LINE__);
    }

    if (!gpio_base)
    {
        int fd;

        fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC); // no root access needed

        if (fd >= 0)
        {
            mmapoffs = 0;
            usingGpiomem = 1;
        }
        else
        {
            usingGpiomem = 0;

            fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC); // root access needed

            if (fd < 0) r = -(__LINE__);
        }

        if (r == 0)
        {
            gpio_base = (RPIHAL_regptr_t)mmap(0, BCM_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mmapoffs);

            if (gpio_base == MAP_FAILED)
            {
                r = -(__LINE__);
                gpio_base = NULL;
            }
        }

        // TODO can fd be closed after mmap?
    }

    return r;
}

int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    if (gpio_base && initStruct && iGPIO_checkPin(pin, sysGpioLocked)) { r = initPin(pin, initStruct); }
    else { r = -(__LINE__); }

    return r;
}

int RPIHAL_GPIO_initPins(uint64_t bits, const RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    uint64_t mask = 0x01ull;

    while (mask)
    {
        if (mask & bits)
        {
            const int err = RPIHAL_GPIO_initPin(RPIHAL_GPIO_bittopin(mask), initStruct);
            if (err) { r = -(__LINE__); }
        }

        mask <<= 1;
    }

    return r;
}

int RPIHAL_GPIO_readPin(int pin)
{
    int r = 0;

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { r = readPin(pin); }
    else { r = -1; }

    return r;
}

uint32_t RPIHAL_GPIO_read()
{
    uint32_t value = 0;

    if (gpio_base)
    {
        RPIHAL_regptr_t addr = gpio_base + (GPLEV0 / 4);
        value = BCM2835_reg_read(addr);
    }

    return value;
}

uint32_t RPIHAL_GPIO_readHi()
{
    uint32_t value = 0;

    if (gpio_base)
    {
        RPIHAL_regptr_t addr = gpio_base + (GPLEV1 / 4);
        value = BCM2835_reg_read(addr);
    }

    return value;
}

uint64_t RPIHAL_GPIO_read64()
{
    uint64_t value;

    value = (uint64_t)RPIHAL_GPIO_readHi();
    value <<= 32;
    value |= (uint64_t)RPIHAL_GPIO_read();

    return value;
}

int RPIHAL_GPIO_writePin(int pin, int state)
{
    int r = 0;

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { writePin(pin, state); }
    else { r = -1; }

    return r;
}

int RPIHAL_GPIO_set(uint32_t bits)
{
    int r = 0;

    if (gpio_base)
    {
        // bits are masked, but no error is reported on bits outside of mask

        RPIHAL_regptr_t addr = gpio_base + (GPSET0 / 4);

        const uint32_t mask = (uint32_t)iGPIO_getUserPinsMask();
        if (mask == 0) { return -(__LINE__); }

        BCM2835_reg_write(addr, (bits & mask));
    }
    else { r = -(__LINE__); }

    return r;
}

int RPIHAL_GPIO_clr(uint32_t bits)
{
    int r = 0;

    if (gpio_base)
    {
        // bits are masked, but no error is reported on bits outside of mask

        RPIHAL_regptr_t addr = gpio_base + (GPCLR0 / 4);

        const uint32_t mask = (uint32_t)iGPIO_getUserPinsMask();
        if (mask == 0) { return -(__LINE__); }

        BCM2835_reg_write(addr, (bits & mask));
    }
    else r = -(__LINE__);

    return r;
}

int RPIHAL_GPIO_togglePin(int pin)
{
    int r = 0;

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { writePin(pin, !readPin(pin)); }
    else { r = -1; }

    return r;
}

int RPIHAL_GPIO_reset()
{
    int r = 0;

    if (gpio_base)
    {
        RPIHAL_GPIO_init_t initStruct;

        const uint64_t userPinsMask = iGPIO_getUserPinsMask();
        const uint64_t bcmPinsMask = iGPIO_getBcmPinsMask();
        if ((userPinsMask == 0) || (bcmPinsMask == 0)) { return -(__LINE__); }

        uint64_t pinBit = 0x01;
        int pin = 0;

        while (pinBit & bcmPinsMask)
        {
            // skip non user pins
            if (pinBit & userPinsMask)
            {
                int err = RPIHAL_GPIO_defaultInitStructPin(pin, &initStruct);

                if (err) { r = err; }
                else
                {
                    err = initPin(pin, &initStruct);

                    if (err) { r = err; }
                    else { writePin(pin, 0); }
                }
            }

            pinBit <<= 1;
            ++pin;
        }
    }
    else r = -(__LINE__);

    return r;
}

int RPIHAL_GPIO_resetPin(int pin)
{
    int r = 0;

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked))
    {
        RPIHAL_GPIO_init_t initStruct;

        r = RPIHAL_GPIO_defaultInitStructPin(pin, &initStruct);
        if (r) { return r; }

        r = initPin(pin, &initStruct);
        if (r) { return r; }

        writePin(pin, 0);
    }
    else { r = -(__LINE__); }

    return r;
}

void RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct) { iGPIO_defaultInitStruct(initStruct); }

int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct) { return iGPIO_defaultInitStructPin(pin, initStruct); }

RPIHAL_regptr_t RPIHAL_GPIO_getMemBasePtr() { return gpio_base; }

int RPIHAL_GPIO_isUsingGpiomem() { return usingGpiomem; }

int RPIHAL_GPIO_bittopin(uint64_t bit) { return iGPIO_bittopin(bit); }

uint64_t RPIHAL_GPIO_pintobit(int pin) { return iGPIO_pintobit(pin); }



#include <stdio.h>
void RPIHAL_GPIO_dumpAltFuncReg(uint64_t pins)
{
    if (!gpio_base) RPIHAL_GPIO_init();

    const RPIHAL_model_t hwModel = RPIHAL_getModel();

    RPIHAL_regptr_t addr;
    int shift;
    uint32_t regValue;
    const uint64_t userPinsMask = iGPIO_getUserPinsMask();

    printf("========================================\n");
    printf("%s selected pins: 0x%04x'%04x'%04x'%04x\n", __func__, (int)((pins >> 48) & 0x0FFFFull), (int)((pins >> 32) & 0x0FFFFull),
           (int)((pins >> 16) & 0x0FFFFull), (int)((pins >> 0) & 0x0FFFFull));

    for (int i = 0; i <= 5; ++i)
    {
        const uint64_t regPinsMask = (0x03FFull << (i * 10)); // 10 pins per register

        addr = gpio_base + (GPFSEL0 / 4) + i;
        regValue = BCM2835_reg_read(addr);

        if (pins & regPinsMask) printf("\033[96mGPFSEL%i: 0x%08x\033[39m\n", i, regValue);

        int jMax = 9;
        if (i == 5)
        {
            if (hwModel < RPIHAL_model_bcm2711) jMax = 3;
            else jMax = 7;
        }

        for (int j = 0; j <= jMax; ++j)
        {
            const int pin = (i * 10) + j;
            const uint64_t pinBit = RPIHAL_GPIO_pintobit(pin);

            shift = 3 * j; // shift = 3 * (pin % 10);

            const uint32_t value = (regValue >> shift) & FSEL_MASK;

            // printf("              %2i %2i     %08x %08x\n", (3 * j), (3 * (pin % 10)), (regValue >> shift), value);

            char binStr[] = "000";
            if (value & 0x01) binStr[2] = '1';
            if (value & 0x02) binStr[1] = '1';
            if (value & 0x04) binStr[0] = '1';

            const char* afStr = "error";
            // clang-format off
            switch (value)
            {
                case 0x00: afStr = "IN"; break;
                case 0x01: afStr = "OUT"; break;
                case 0x04: afStr = "AF0"; break;
                case 0x05: afStr = "AF1"; break;
                case 0x06: afStr = "AF2"; break;
                case 0x07: afStr = "AF3"; break;
                case 0x03: afStr = "AF4"; break;
                case 0x02: afStr = "AF5"; break;
                default: afStr = "unknown"; break;
            }
            // clang-format on

            if (pins & pinBit)
            {
                if (!(userPinsMask & RPIHAL_GPIO_pintobit(pin))) printf("\033[90m");

                printf("  %2i: 0b%s %s", pin, binStr, afStr);

                if (!(userPinsMask & RPIHAL_GPIO_pintobit(pin))) printf("\033[39m");
                printf("\n");
            }
        }
    }

    printf("========================================\n");
}

void RPIHAL_GPIO_dumpPullUpDnReg(uint64_t pins)
{
    const RPIHAL_model_t hwModel = RPIHAL_getModel();
    if (hwModel < RPIHAL_model_bcm2711)
    {
        LOG_ERR("%s not available on model %08x", __func__, hwModel);
        return;
    }

    if (!gpio_base) RPIHAL_GPIO_init();

    RPIHAL_regptr_t addr;
    int shift;
    uint32_t regValue;
    const uint64_t userPinsMask = iGPIO_getUserPinsMask();

    printf("========================================\n");
    printf("%s selected pins: 0x%04x'%04x'%04x'%04x\n", __func__, (int)((pins >> 48) & 0x0FFFFull), (int)((pins >> 32) & 0x0FFFFull),
           (int)((pins >> 16) & 0x0FFFFull), (int)((pins >> 0) & 0x0FFFFull));

    for (int i = 0; i <= 3; ++i)
    {
        const uint64_t regPinsMask = (0x0FFFFull << (i * 16)); // 16 pins per register

        addr = gpio_base + (BCM2711_GPIO_PUP_PDN_CNTRL_REG0 / 4) + i;
        regValue = BCM2835_reg_read(addr);

        if (pins & regPinsMask) printf("\033[96mGPIO_PUP_PDN_CNTRL_REG%i: 0x%08x\033[39m\n", i, regValue);

        int jMax = 15;
        if (i == 3) { jMax = 9; }

        for (int j = 0; j <= jMax; ++j)
        {
            const int pin = (i * 16) + j;
            const uint64_t pinBit = RPIHAL_GPIO_pintobit(pin);

            shift = 2 * j; // shift = 2 * (pin % 16);

            const uint32_t value = (regValue >> shift) & BCM2711_GPIO_PUP_PDN_MASK;

            char binStr[] = "00";
            if (value & 0x01) binStr[1] = '1';
            if (value & 0x02) binStr[0] = '1';

            const char* pudStr = "error";
            // clang-format off
            switch (value)
            {
                case 0x00: pudStr = "none"; break;
                case 0x01: pudStr = "UP"; break;
                case 0x02: pudStr = "DOWN"; break;
                case 0x03: pudStr = "\033[91mres\033[39m"; break;
                default: pudStr = "unknown"; break;
            }
            // clang-format on

            if (pins & pinBit)
            {
                if (!(userPinsMask & RPIHAL_GPIO_pintobit(pin))) printf("\033[90m");

                printf("  %2i: 0b%s %s", pin, binStr, pudStr);

                if (!(userPinsMask & RPIHAL_GPIO_pintobit(pin))) printf("\033[39m");
                printf("\n");
            }
        }
    }

    printf("========================================\n");
}



/**
 * @return 0 on success
 */
int initPin(int pin, const RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    const RPIHAL_model_t hwModel = RPIHAL_getModel();

    RPIHAL_regptr_t addr;
    int shift;
    uint32_t value;
    uint32_t mask;



    // fsel

    addr = gpio_base + (GPFSEL0 / 4) + (pin / 10);
    shift = 3 * (pin % 10);
    if (initStruct->mode == RPIHAL_GPIO_MODE_OUT) value = FSEL_OUT;
    else if (initStruct->mode == RPIHAL_GPIO_MODE_AF) value = FSEL_AF_LUT[initStruct->altfunc];
    else value = FSEL_IN;
    value <<= shift;
    mask = FSEL_MASK << shift;
    BCM2835_reg_write_bits(addr, value, mask);



    // pull up/down

    if (RPIHAL_model_SoC_peripheral_is_bcm283x(hwModel))
    {
        // 1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither to remove the
        //    current Pull-up/down)
        addr = gpio_base + (BCM2835_GPPUD / 4);
        if (initStruct->pull == RPIHAL_GPIO_PULL_UP) { value = BCM2835_PUD_UP; }
        else if (initStruct->pull == RPIHAL_GPIO_PULL_DOWN) { value = BCM2835_PUD_DOWN; }
        else { value = BCM2835_PUD_NONE; }
        BCM2835_reg_write(addr, value);

        // 2. Wait 150 cycles – this provides the required set-up time for the control signal
        BCM2835_wait_cycles(150);

        // 3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to modify – NOTE only the
        //    pads which receive a clock will be modified, all others will retain their previous state.
        addr = gpio_base + (BCM2835_GPPUDCLK0 / 4) + (pin / 32);
        shift = (pin % 32);
        value = 1u << shift;
        BCM2835_reg_write(addr, value);

        // 4. Wait 150 cycles – this provides the required hold time for the control signal
        BCM2835_wait_cycles(150);

        // 5. Write to GPPUD to remove the control signal
        addr = gpio_base + (BCM2835_GPPUD / 4);
        value = 0;
        BCM2835_reg_write(addr, value);

        // 6. Write to GPPUDCLK0/1 to remove the clock
        addr = gpio_base + (BCM2835_GPPUDCLK0 / 4) + (pin / 32);
        BCM2835_reg_write(addr, 0);
    }
    else if (RPIHAL_model_SoC_peripheral_is_bcm2711(hwModel))
    {
        addr = gpio_base + (BCM2711_GPIO_PUP_PDN_CNTRL_REG0 / 4) + (pin / 16);
        shift = (pin % 16) * 2;

        if (initStruct->pull == RPIHAL_GPIO_PULL_UP) { value = BCM2711_GPIO_PUP_PDN_UP; }
        else if (initStruct->pull == RPIHAL_GPIO_PULL_DOWN) { value = BCM2711_GPIO_PUP_PDN_DOWN; }
        else { value = BCM2711_GPIO_PUP_PDN_NONE; }

        value <<= shift;
        mask = BCM2711_GPIO_PUP_PDN_MASK << shift;
        BCM2835_reg_write_bits(addr, value, mask);
    }
    else
    {
        LOG_ERR("unknown SoC peripheral specification");
        r = -(__LINE__);
    }



    // drive



#if (LOG_MODULE_LEVEL >= LOG_LEVEL_INF)
    if (!r)
    {
        char alt[] = "AF?";
        const char* func = "###";
        const char* pull = "####";

        if (initStruct->mode == RPIHAL_GPIO_MODE_OUT) func = "OUT";
        else if (initStruct->mode == RPIHAL_GPIO_MODE_AF)
        {
            alt[2] = (char)(0x30 + initStruct->altfunc);
            func = alt;
        }
        else func = "IN";

        if (initStruct->pull == RPIHAL_GPIO_PULL_UP) { pull = "UP"; }
        else if (initStruct->pull == RPIHAL_GPIO_PULL_DOWN) { pull = "DOWN"; }
        else { pull = "none"; }

        LOG_INF("init pin %2i to %-3s with pull %-s", pin, func, pull);
    }
#endif

    return r;
}

int readPin(int pin)
{
    int r;
    RPIHAL_regptr_t addr = gpio_base + (GPLEV0 / 4) + (pin / 32);
    const uint32_t bit = 1 << (pin % 32);

    if (BCM2835_reg_read(addr) & bit) r = 1;
    else r = 0;

    return r;
}

void writePin(int pin, int state)
{
    RPIHAL_regptr_t addr = gpio_base + (pin / 32);
    const uint32_t bit = 1 << (pin % 32);

    if (state) addr += (GPSET0 / 4);
    else addr += (GPCLR0 / 4);

    BCM2835_reg_write(addr, bit);
}
