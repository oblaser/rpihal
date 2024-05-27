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

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "internal/platform_check.h"
#include "internal/util.h"
#include "rpihal/int.h"
#include "rpihal/sys.h"

#include <dirent.h>


static uint8_t getHexDigitValue(char c);



int RPIHAL_SYS_getCpuTemp(float* temperature)
{
    int r;

    if (temperature)
    {
        FILE* fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

        if (fp)
        {
            float temp;
            const int res = fscanf(fp, "%f", &temp);

            if (res == 1)
            {
                temp /= 1000.0f;
                *temperature = temp;
                r = 0;
            }
            else r = res + 1000000;

            fclose(fp);
        }
        else r = 2;
    }
    else r = 1;

    return r;
}

RPIHAL_uint128_t RPIHAL_SYS_getUuid()
{
    static RPIHAL_uint128_t uuid = RPIHAL_UINT128_NULL;

    errno = 0;

    if ((uuid.hi == 0) && (uuid.lo == 0))
    {
        DIR* const d = opendir("/dev/disk/by-uuid");

        if (d)
        {
            char uuidStr[33]; // 128bit hex str has 32 chars
            uuidStr[0] = 0;
            size_t len = 0;

            const size_t uuidStrBufferSize = sizeof(uuidStr);
            const size_t maxLen = uuidStrBufferSize - 1; // null terminator

            // read UUID string
            while (1)
            {
                const struct dirent* const e = readdir(d);

                if (e)
                {
                    char tmpStr[sizeof(uuidStr)];
                    const char* src = e->d_name;
                    char* dst = tmpStr;
                    size_t tmpLen = 0;

                    // copy hex string (only hex digits)
                    while ((*src != 0) && (tmpLen < maxLen))
                    {
                        if (isxdigit(*src))
                        {
                            // printf("copy UUID character %c\n", *src);

                            *dst = *src;
                            ++dst;
                            ++tmpLen;
                        }
                        else if (!UTIL_isxdelimiter(*src))
                        {
                            // printf("%c is an invalid UUID character\n", *src);

                            tmpLen = 0;
                            break;
                        }

                        ++src;
                    }
                    tmpStr[tmpLen] = 0; // add null terminator

                    // actual copy, if has more bits than the current
                    if (tmpLen > len)
                    {
                        strcpy(uuidStr, tmpStr);
                        len = tmpLen;
                    }

                    // printf("fetched UUID: %s\n", tmpStr);
                    // printf("using:        %s\n", uuidStr);
                }
                else
                {
                    // if (errno) nop, errno is set
                    // else nop, end of dir entries

                    break;
                }
            }

            // printf("UUID str: %s\n", uuidStr);

            // convert UUID string
            if (len > 0)
            {
                const char* p = uuidStr + len - 1;
                size_t cnt = 0;

                while (p >= &(uuidStr[0]))
                {
                    uint64_t digitValue = getHexDigitValue(*p);

                    digitValue <<= ((cnt % 16) * 4);

                    if (cnt < 16) { uuid.lo |= digitValue; }
                    else { uuid.hi |= digitValue; }

                    // printf("%c => %16llx, cnt: %2u, %16llx %16llx\n", *p, digitValue, cnt, uuid.hi, uuid.lo);

                    ++cnt;
                    --p;
                }
            }
        }
        // else nop, errno is set
    }

    return uuid;
}



uint8_t getHexDigitValue(char c)
{
    const char digits[2][17] = { "0123456789abcdef", "0123456789ABCDEF" };

    for (int i = 0; i < 2; ++i)
    {
        for (uint8_t value = 0; value < 16; ++value)
        {
            if (digits[i][value] == c) { return value; }
        }
    }

    return (-1);
}
