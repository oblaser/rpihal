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

#ifndef IG_RPIHAL_INT_H
#define IG_RPIHAL_INT_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


#define RPIHAL_8BIT_LSB (0x01)
#define RPIHAL_8BIT_MSB (0x80)
#define RPIHAL_8BIT_ALL (0xFF)

#define RPIHAL_16BIT_LSB (0x0001)
#define RPIHAL_16BIT_MSB (0x8000)
#define RPIHAL_16BIT_ALL (0xFFFF)

#define RPIHAL_32BIT_LSB (0x00000001)
#define RPIHAL_32BIT_MSB (0x80000000)
#define RPIHAL_32BIT_ALL (0xFFFFFFFF)

#define RPIHAL_64BIT_LSB (0x0000000000000001)
#define RPIHAL_64BIT_MSB (0x8000000000000000)
#define RPIHAL_64BIT_ALL (0xFFFFFFFFFFFFFFFF)


typedef struct
{
    uint64_t hi;
    uint64_t lo;
} RPIHAL_uint128_t;

// clang-format off
#define RPIHAL_UINT128_NULL ((RPIHAL_uint128_t)({ .hi = 0, .lo = 0 }))
// clang-format on

/**
 * @brief Compares two unsigned 128 bit integers.
 *
 * - Negative value if `lhs` is less than `rhs`
 * - Zero if `lhs` and `rhs` compare equal
 * - Positive value if `lhs` is greater than `rhs`
 */
int RPIHAL_ui128_cmp(const RPIHAL_uint128_t* lhs, const RPIHAL_uint128_t* rhs);


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_INT_H
