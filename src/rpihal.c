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

#include <stddef.h>
#include <stdint.h>

#include "internal/platform_check.h"
#include "rpihal/rpihal.h"



static RPIHAL_model_t ___model = RPIHAL_model_unknown;
void RPIHAL___setModel___(RPIHAL_model_t model) { ___model = model; }

RPIHAL_model_t RPIHAL_getModel()
{
    static RPIHAL_model_t model = RPIHAL_model_unknown;

    model = ___model;

    while (RPIHAL_model_unknown == model)
    {
        char cpuInfoTxt[1024 * 3]; // the result of "Raspberry Pi 4 Model B Rev 1.5" needs 918 bytes

        // const int fd = open("/proc/cpuinfo", O_RDONLY);
        // if (fd < 0) break;
        //
        // close(fd);

        // TODO detect model



        break; // exit
    }

    return model;
}



//======================================================================================================================
// Emulator

int RPIHAL_EMU_init(RPIHAL_model_t model)
{
    (void)model;
    return 0;
}

void RPIHAL_EMU_cleanup() { return; }
int RPIHAL_EMU_isRunning() { return 0; }
