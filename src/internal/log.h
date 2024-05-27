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

#ifndef IG_RPIHAL_INTERNAL_LOG_H
#define IG_RPIHAL_INTERNAL_LOG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#define LOG_LEVEL_OFF (0)
#define LOG_LEVEL_ERR (1)
#define LOG_LEVEL_WRN (2)
#define LOG_LEVEL_INF (3)
#define LOG_LEVEL_DBG (4)

#ifndef RPIHAL_CONFIG_LOGLEVEL
#warning "RPIHAL_CONFIG_LOGLEVEL is not defined, defaulting to 2 (warning)"
#define RPIHAL_CONFIG_LOGLEVEL LOG_LEVEL_WRN
#endif

// clang-format off
#define ___LOG_OPT_VA_ARGS(...) , ##__VA_ARGS__ // optional args

// TODO make logging aware of RPIHAL_CONFIG_LOGLEVEL
// TODO LOG_MODULE_LEVEL and LOG_MODULE_NAME

#define LOG_ERR(msg, ...) printf("\033[91m" "[ERR] " msg "\033[39m" "\n" ___LOG_OPT_VA_ARGS(__VA_ARGS__))
#define LOG_WRN(msg, ...) printf("\033[93m" "[WRN] " msg "\033[39m" "\n" ___LOG_OPT_VA_ARGS(__VA_ARGS__))
#define LOG_INF(msg, ...) printf(           "[INF] " msg            "\n" ___LOG_OPT_VA_ARGS(__VA_ARGS__))
#define LOG_DBG(msg, ...) printf(           "[DBG] " msg            "\n" ___LOG_OPT_VA_ARGS(__VA_ARGS__))
// clang-format on


#ifdef __cplusplus
}
#endif

#endif // IG_RPIHAL_INTERNAL_LOG_H
