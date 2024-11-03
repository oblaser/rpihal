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

#ifndef IG_RPIHAL_SYS_H
#define IG_RPIHAL_SYS_H

#include <stddef.h>
#include <stdint.h>

#include <rpihal/int.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @param [out] temperature Pointer to the variable receiving the CPU temperature in degree Celsius
 * @return __0__ on success
 */
int RPIHAL_SYS_getCpuTemp(float* temperature);

/**
 * @brief Returns the machine ID.
 *
 * Reads and parses `/etc/machine-id`.
 *
 * @return __0__ if reading failed
 */
RPIHAL_uint128_t RPIHAL_SYS_getMachineId();


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_SYS_H
