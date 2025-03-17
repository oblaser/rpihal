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

#ifndef IG_RPIHAL_RPIHAL_H
#define IG_RPIHAL_RPIHAL_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



// Don't compare or use the value of these enum constants. Only use these enum symbols and the functions provided below!
// The effective value (and thus the meaning of the value) of these symbols may change in future releases, as
// requirements to these may change.
//
// 0b0000'0000'000s'ssss'mmmm'0000'0000'0000
//
// bit 21..31       reserved
// bit 16..20       SoC model       mask: 0x001F0000
// bit 12..15       model           mask: 0x0000F000
// bit  0..11       reserved
typedef enum
{
    // clang-format off

    RPIHAL_model_unknown = 0,

    // BCM2835
    RPIHAL_model_bcm2835 =      0x00010000,
#if 0 // not yet supported // ADDHW
    RPIHAL_model_1A =           0x00011000, // `1A`                 Raspberry Pi 1 Model A (rev?)
    RPIHAL_model_1Ap =          0x00012000, // `1A+`                Raspberry Pi 1 Model A+
    RPIHAL_model_1B =           0x00013000, // `1B`                 Raspberry Pi 1 Model B (rev?)
    RPIHAL_model_1Bp =          0x00014000, // `1B+`                Raspberry Pi 1 Model B+
#error "rev needs to be encoded in the model number"    // see "ADDHW" in `iGPIO_getUserPinsMask()`
    RPIHAL_model_z =            0x00015000, // `Zero`               Raspberry Pi Zero
    RPIHAL_model_zW =           0x00016000, // `Zero W`/`Zero WH`   Raspberry Pi Zero W/WH
    RPIHAL_model_cm1 =          0x00017000, // `CM1`                Raspberry Pi Compute Module 1
#endif

    // BCM2836
    RPIHAL_model_bcm2836 =      0x00020000,
    RPIHAL_model_2B =           0x00021000, // `2B`                 Raspberry Pi 2 Model B (pre v1.2)

    // BCM2837
    RPIHAL_model_bcm2837 =      0x00030000,
    RPIHAL_model_2B_v1_2 =      0x00031000, // `2B v1.2`            Raspberry Pi 2 Model B v1.2 ; doesn't matter to the HAL, peripheral base address is the same
    RPIHAL_model_3B =           0x00032000, // `3B`                 Raspberry Pi 3 Model B
    RPIHAL_model_cm3 =          0x00033000, // `CM3`                Raspberry Pi Compute Module 3
    RPIHAL_model_z2W =          0x00034000, // `Zero 2 W`           Raspberry Pi Zero 2 W

    // BCM2837B0
    RPIHAL_model_bcm2837b0 =    0x00040000,
    RPIHAL_model_3Ap =          0x00041000, // `3A+`                Raspberry Pi 3 Models A+
    RPIHAL_model_3Bp =          0x00042000, // `3B+`                Raspberry Pi 3 Models B+
    RPIHAL_model_cm3p =         0x00043000, // `CM3+`               Raspberry Pi Compute Module 3+

    // BCM2711
    RPIHAL_model_bcm2711 =      0x00050000,
    RPIHAL_model_4B =           0x00051000, // `4B`                 Raspberry Pi 4 Model B
    RPIHAL_model_400 =          0x00052000, // `400`                Raspberry Pi 400
    RPIHAL_model_cm4 =          0x00053000, // `CM4`                Raspberry Pi Compute Module 4
    RPIHAL_model_cm4s =         0x00054000, // `CM4S`               Raspberry Pi Compute Module 4S

    // BCM2712
    RPIHAL_model_bcm2712 =      0x00060000,
    RPIHAL_model_5 =            0x00061000, // `5`                  Raspberry Pi 5
    RPIHAL_model_500 =          0x00062000, // `500`                Raspberry Pi 500
    RPIHAL_model_cm5 =          0x00063000, // `CM5`                Raspberry Pi Compute Module 5

    RPIHAL_model_bcm_NEXT_ =    0x00070000,

    // clang-format on
} RPIHAL_model_t;


RPIHAL_model_t RPIHAL_getModel();

const char* RPIHAL_dt_compatible();
const char* RPIHAL_dt_model();

static inline int RPIHAL_model_SoC_is_bcm2835(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2835) && (model < RPIHAL_model_bcm2836) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2836(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2836) && (model < RPIHAL_model_bcm2837) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837) && (model < RPIHAL_model_bcm2837b0) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837b0(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837b0) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837_any(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2711(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2711) && (model < RPIHAL_model_bcm2712) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2712(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2712) ? 1 : 0); }

// clang-format off

// bcm2835, bcm2836, bcm2837
static inline int RPIHAL_model_SoC_peripheral_is_bcm283x(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2835) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }

static inline int RPIHAL_model_SoC_peripheral_is_bcm2711(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2711) && (model < RPIHAL_model_bcm2712) ? 1 : 0); }

//static inline int RPIHAL_model_SoC_peripheral_is_bcm2712(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2712) && (model < RPIHAL_model_bcm_NEXT_) ? 1 : 0); }

// clang-format on

static inline int RPIHAL_model_header_is_26pin(RPIHAL_model_t model)
{
    // ADDHW
    // return ((model ==RPIHAL_model_1A) || (model ==RPIHAL_model_1B));

    return 0;
}

static inline int RPIHAL_model_header_is_40pin(RPIHAL_model_t model)
{
    return (
        // ADDHW
        // (model ==RPIHAL_model_1Ap) || (model ==RPIHAL_model_1Bp) || (model ==RPIHAL_model_z) || (model ==RPIHAL_model_zW) || (model == RPIHAL_model_5) ||
        (model == RPIHAL_model_2B) || (model == RPIHAL_model_2B_v1_2) || (model == RPIHAL_model_3B) || (model == RPIHAL_model_z2W) ||
        (model == RPIHAL_model_3Ap) || (model == RPIHAL_model_3Bp) || (model == RPIHAL_model_4B) || (model == RPIHAL_model_400));
}



//======================================================================================================================
// Emulator

/**
 * @brief Initialises the emulator and starts it's thread.
 *
 * @param model The Raspberry Pi model which should be emulated
 * @retval 0 on success
 */
int RPIHAL_EMU_init(RPIHAL_model_t model);

/**
 * @brief Sets the default GPIO pin states according to the hardware the Pi would be connected to.
 *
 * If needed, has to be called right after `RPIHAL_EMU_init()` succeeded.
 */
void RPIHAL_ENU_setInitialGpioState(uint64_t mask);

/**
 * @brief Cleans up resources used by the emulator.
 */
void RPIHAL_EMU_cleanup();

/**
 * @brief Checks if the emulator thread is running.
 *
 * @retval `true` (non 0) if the emulator is running
 * @retval `false` `0` if running on Pi, or if the emulator is not running
 */
int RPIHAL_EMU_isRunning();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_RPIHAL_H
