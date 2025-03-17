/*
author          Oliver Blaser
date            23.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

// std includes
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// prj includes
//...

// lib includes
#include <rpihal/sys.h>
#include <unistd.h>


int main(int argc, char** argv)
{
    int r = -1;
    time_t tNow, tOld = 0;

    while (1)
    {
        tNow = time(NULL);

        if ((tNow - tOld) >= 2)
        {
            tOld = tNow;

            float temp;
            const int res = RPIHAL_SYS_getCpuTemp(&temp);

            if (res == 0)
            {
                // clang-format off
                printf("CPU temp: %1.1f\xC2\xB0""C - %f\n", temp, temp);
                // clang-format on
            }
            else { printf("failed (%i)\n", res); }
        }

        usleep(50 * 1000); // sleep 50ms to save CPU capacity
    }

    return r;
}
