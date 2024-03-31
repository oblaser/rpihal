/*
author          Oliver Blaser
date            23.03.2024
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


// int RPIHAL_...();



//======================================================================================================================
// Emulator

// These functions may be called from tha actual RasPi code, they just return success.

int RPIHAL_EMU_init();

//! @brief Cleans up resources used by the emulator.
//!
//! Only needed if the enulator is used, but can be called from PI code. So no compile switch is needed.
//!
void RPIHAL_EMU_cleanup();

int RPIHAL_EMU_isRunning();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_RPIHAL_H
