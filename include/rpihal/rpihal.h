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


typedef enum // see https://www.raspberrypi.com/documentation/computers/processors.html
{
    RPIHAL_model_unknown = 0,

    // BCM2835
    RPIHAL_model_bcm2835 = 1000,
#if 0 // not yet supported // ADDHW
    RPIHAL_model_1A = 1010,  // `1A`                Raspberry Pi 1 Model A (rev?)
    RPIHAL_model_1Ap = 1020, // `1A+`               Raspberry Pi 1 Model A+
    RPIHAL_model_1B = 1030,  // `1B`                Raspberry Pi 1 Model B (rev?)
    RPIHAL_model_1Bp = 1040, // `1B+`               Raspberry Pi 1 Model B+
    RPIHAL_model_z = 1050,   // `Zero`              Raspberry Pi Zero
    RPIHAL_model_zW = 1060,  // `Zero W`/`Zero WH`  Raspberry Pi Zero W/WH
    RPIHAL_model_cm1 = 1070, // `CM1`               Raspberry Pi Compute Module 1
#endif

    // BCM2836
    RPIHAL_model_bcm2836 = 2000,
    RPIHAL_model_2B = 2010, // `2B`                 Raspberry Pi 2 Model B (pre v1.2)

    // BCM2837
    RPIHAL_model_bcm2837 = 3000,
    RPIHAL_model_2B_v1_2 = 3010, // `2B v1.2`       Raspberry Pi 2 Model B v1.2
    RPIHAL_model_3B = 3020,      // `3B`            Raspberry Pi 3 Model B
    RPIHAL_model_cm3 = 3030,     // `CM3`           Raspberry Pi Compute Module 3
    RPIHAL_model_z2W = 3040,     // `Zero 2 W`      Raspberry Pi Zero 2 W

    // BCM2837B0
    RPIHAL_model_bcm2837b0 = 3500,
    RPIHAL_model_3Ap = 3510,  // `3A+`              Raspberry Pi 3 Models A+
    RPIHAL_model_3Bp = 3520,  // `3B+`              Raspberry Pi 3 Models B+
    RPIHAL_model_cm3p = 3530, // `CM3+`             Raspberry Pi Compute Module 3+

    // BCM2711
    RPIHAL_model_bcm2711 = 4000,
    RPIHAL_model_4B = 4010,  // `4B`                Raspberry Pi 4 Model B
    RPIHAL_model_400 = 4020, // `400`               Raspberry Pi 400
    RPIHAL_model_cm4 = 4030, // `CM4`               Raspberry Pi Compute Module 4

    // BCM2712
    RPIHAL_model_bcm2712 = 5000,
#if 0 // ADDHW // peripherals should be the same as BCM2711, to be tested/researched (UART differs)
    RPIHAL_model_5 = 5010, // `5`                   Raspberry Pi 5
#endif

} RPIHAL_model_t; // TODO change to compareable (SoC is most significant bits) bitfield
                  // make 64bit and 16bit msbits reserved zero, then SoC as "msbits", ...


RPIHAL_model_t RPIHAL_getModel();

/**
 * Temporary hack, needed until auto detection in `RPIHAL_getModel()` is implemented.
 */
void RPIHAL___setModel___(RPIHAL_model_t model);

static inline int RPIHAL_model_SoC_is_bcm2835(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2835) && (model < RPIHAL_model_bcm2836) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2836(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2836) && (model < RPIHAL_model_bcm2837) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837) && (model < RPIHAL_model_bcm2837b0) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837b0(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837b0) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2837_any(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2837) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2711(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2711) && (model < RPIHAL_model_bcm2712) ? 1 : 0); }
static inline int RPIHAL_model_SoC_is_bcm2712(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2712) ? 1 : 0); }

// clang-format off
static inline int RPIHAL_model_SoC_peripheral_is_bcm2835(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2835) && (model < RPIHAL_model_bcm2711) ? 1 : 0); }
static inline int RPIHAL_model_SoC_peripheral_is_bcm2711(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2711) && (model < RPIHAL_model_bcm2712) ? 1 : 0); }
//static inline int RPIHAL_model_SoC_peripheral_is_bcm2712(RPIHAL_model_t model) { return ((model >= RPIHAL_model_bcm2712) ? 1 : 0); }
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
 * Only needed if the enulator is used, but can be called from PI code. So no compile switch is needed.
 *
 * @param model The Raspberry Pi model which should be emulated
 * @retval 0 on success
 */
int RPIHAL_EMU_init(RPIHAL_model_t model);

/**
 * @brief Cleans up resources used by the emulator.
 *
 * Only needed if the enulator is used, but can be called from PI code. So no compile switch is needed.
 */
void RPIHAL_EMU_cleanup();

/**
 * @brief Checks if the emulator is running.
 *
 * @retval `true` (non 0) if the emulator is running
 * @retval `false` `0` if running on Pi, or if the emulator is not running
 */
int RPIHAL_EMU_isRunning();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_RPIHAL_H
