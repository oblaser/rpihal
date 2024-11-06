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
#include <stdio.h>
#include <string.h>

#include "internal/platform_check.h"
#include "rpihal/rpihal.h"

#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  RPIHAL
#include "internal/log.h"



#define DEVICETREE_BUFFER_SIZE (300)
static char deviceTreeCompatibleBuffer[DEVICETREE_BUFFER_SIZE];
static char deviceTreeModelBuffer[DEVICETREE_BUFFER_SIZE];
static const char* deviceTreeCompatiblePtr = NULL;
static const char* deviceTreeModelPtr = NULL;

static ssize_t readAllText(const char* filename, char* buffer, size_t bufsz);
static void readDeviceTreeCompatible();
static void readDeviceTreeModel();



RPIHAL_model_t RPIHAL_getModel()
{
    static RPIHAL_model_t model = RPIHAL_model_unknown;

    const char* const p = RPIHAL_dt_compatible();

    if ((model == RPIHAL_model_unknown) && p)
    {
        const char* const boardMake = p;
        const char* const boardModel = strchr(p, ',') + 1;
        const char* const cpuMakeModel = strchr(boardModel, ',') + 1;

        if (boardModel)
        {
            // https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#check-raspberry-pi-model-and-cpu-across-distributions

            if (strncmp(boardModel, "2-model-b,", 10) == 0)
            {
                model = RPIHAL_model_2B;

                // TODO check CPU
                // model = RPIHAL_model_2B_v1_2;
            }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_3B; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_cm3; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_z2W; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_3Ap; }
            else if (strncmp(boardModel, "3-model-b-plus,", 15) == 0) { model = RPIHAL_model_3Bp; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_cm3p; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_4B; }
            else if (strncmp(boardModel, "400,", 4) == 0) { model = RPIHAL_model_400; }
            else if (strncmp(boardModel, ",", ) == 0) { model = RPIHAL_model_cm4; }
        }

        if (model == RPIHAL_model_unknown) { LOG_ERR("unknown board: %s", p); }
    }

    return model;
}

const char* RPIHAL_getModelStr(uint64_t modelId)
{
    static char str[] = "RPIHAL_getModelStr() not implemented";

    // TODO implement
    (void)modelId;

    return str;
}

const char* RPIHAL_dt_compatible()
{
    if (!deviceTreeCompatiblePtr) { readDeviceTreeCompatible(); }
    return deviceTreeCompatiblePtr;
}

const char* RPIHAL_dt_model()
{
    if (!deviceTreeModelPtr) { readDeviceTreeModel(); }
    return deviceTreeModelPtr;
}



ssize_t readAllText(const char* filename, char* buffer, size_t bufsz)
{
    ssize_t r = (-1);

    FILE* fp = fopen(filename, "rb");

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        const long sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if ((sz >= 0) && (sz < (bufsz - 1)))
        {
            const size_t nRead = fread(buffer, 1, (size_t)sz, fp);

            if (nRead == (size_t)sz)
            {
                buffer[sz] = 0;
                r = (ssize_t)nRead;
            }
            else { LOG_ERR("failed to read \"%s\", size: %li, nRead: %llu", filename, sz, (uint64_t)nRead); }
        }
        else { LOG_ERR("failed to read \"%s\", invalid fseek/tell", filename); }

        fclose(fp);
    }
    else { LOG_ERR("failed to open \"%s\"", filename); }

    return r;
}

void readDeviceTreeCompatible()
{
    const ssize_t sz = readAllText("/proc/device-tree/compatible", deviceTreeCompatibleBuffer, DEVICETREE_BUFFER_SIZE);

    if (sz > 0)
    {
        // replace null characters with ',', except for the terminating null
        for (ssize_t i = 0; i < sz; ++i)
        {
            if (deviceTreeCompatibleBuffer[i] == '\0') { deviceTreeCompatibleBuffer[i] = ','; }
        }

        deviceTreeCompatiblePtr = deviceTreeCompatibleBuffer;
    }
}

void readDeviceTreeModel()
{
    const ssize_t sz = readAllText("/proc/device-tree/model", deviceTreeModelBuffer, DEVICETREE_BUFFER_SIZE);

    if (sz > 0) { deviceTreeModelPtr = deviceTreeModelBuffer; }
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
