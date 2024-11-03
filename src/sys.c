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
                temp /= 1e3f;
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

RPIHAL_uint128_t RPIHAL_SYS_getMachineId()
{
    RPIHAL_uint128_t machineId = RPIHAL_UINT128_NULL;

    FILE* fp = fopen("/etc/machine-id", "r");

    if (fp)
    {
        char machineIdStr[100];
        const int res = fscanf(fp, "%99s", machineIdStr); // the argument array must have room for at least width+1 characters

        if (res == 1)
        {
            const size_t len = strlen(machineIdStr);
            const char* p = machineIdStr + len - 1;
            size_t cnt = 0;

            while (p >= &(machineIdStr[0]))
            {
                uint64_t digitValue = getHexDigitValue(*p);

                digitValue <<= ((cnt % 16) * 4);

                if (cnt < 16) { machineId.lo |= digitValue; }
                else { machineId.hi |= digitValue; }

                // printf("%c => %16llx, cnt: %2u, %16llx %16llx\n", *p, digitValue, cnt, machineId.hi, machineId.lo);

                ++cnt;
                --p;
            }
        }

        fclose(fp);
    }

    return machineId;
}



uint8_t getHexDigitValue(char c)
{
    static const char digits[2][17] = { "0123456789abcdef", "0123456789ABCDEF" };

    for (int i = 0; i < 2; ++i)
    {
        for (uint8_t value = 0; value < 16; ++value)
        {
            if (digits[i][value] == c) { return value; }
        }
    }

    return (-1);
}
