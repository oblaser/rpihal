/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

/*

Copyright (c) 2025 Oliver Blaser

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

#ifndef IG_RPIHAL_EMU_EMU_H
#define IG_RPIHAL_EMU_EMU_H

#include <stddef.h>
#include <stdint.h>

#include "../rpihal.h"
#include "../spi.h"


#ifdef __cplusplus
extern "C" {
#endif


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
void RPIHAL_EMU_setInitialGpioState(uint64_t mask);

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

#endif // IG_RPIHAL_EMU_EMU_H
