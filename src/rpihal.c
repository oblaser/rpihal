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

    const char* const dtModel = RPIHAL_dt_model();

    if ((model == RPIHAL_model_unknown) && dtModel)
    {
        // see /doc/raspberrypi-models.md

        if (strncmp(dtModel, "Raspberry Pi ", 13) == 0)
        {
            const char* const modelStr = dtModel + 13;

            if (strncmp(modelStr, "2 Model B Rev ", 14) == 0)
            {
                if (strncmp(modelStr + 14, "1.2", 4) == 0) { model = RPIHAL_model_2B_v1_2; }                  // to be tested
                else { model = RPIHAL_model_2B; }                                                             // to be tested
            }                                                                                                 //
            else if (strncmp(modelStr, "3 Model B Rev ", 14) == 0) { model = RPIHAL_model_3B; }               // test OK
            else if (strncmp(modelStr, "Compute Module 3 Rev ", 21) == 0) { model = RPIHAL_model_cm3; }       // to be tested
            else if (strncmp(modelStr, "Zero 2 W Rev ", 13) == 0) { model = RPIHAL_model_z2W; }               // test OK
            else if (strncmp(modelStr, "3 Model A Plus Rev ", 19) == 0) { model = RPIHAL_model_3Ap; }         // test OK
            else if (strncmp(modelStr, "3 Model B Plus Rev ", 19) == 0) { model = RPIHAL_model_3Bp; }         // test OK
            else if (strncmp(modelStr, "Compute Module 3 Plus Rev ", 26) == 0) { model = RPIHAL_model_cm3p; } // to be tested
            else if (strncmp(modelStr, "4 Model B Rev ", 14) == 0) { model = RPIHAL_model_4B; }               // test OK
            else if (strncmp(modelStr, "400 Rev ", 8) == 0) { model = RPIHAL_model_400; }                     // test OK
            else if (strncmp(modelStr, "Compute Module 4 Rev ", 21) == 0) { model = RPIHAL_model_cm4; }       // guessed, to be tested
        }



        const char* const dtCompatible = RPIHAL_dt_compatible();

        if (dtCompatible)
        {
            const char* const boardMake = dtCompatible;
            const char* const boardModel = strchr(boardMake, ',') + 1;
            const char* const cpuMake = strchr(boardModel, ',') + 1;
            const char* const cpuModel = strchr(cpuMake, ',') + 1;

            if ((strncmp(boardMake, "raspberrypi,", 12) != 0) || (strncmp(cpuMake, "brcm,", 5) != 0)) { LOG_WRN("unknown manufacturers: %s", dtCompatible); }

            if ((RPIHAL_model_SoC_is_bcm2835(model) && (strncmp(cpuModel, "bcm2835", 8) != 0)) ||
                (RPIHAL_model_SoC_is_bcm2836(model) && (strncmp(cpuModel, "bcm2836", 8) != 0)) ||
                (RPIHAL_model_SoC_is_bcm2837_any(model) && (strncmp(cpuModel, "bcm2837", 8) != 0)) ||
                (RPIHAL_model_SoC_is_bcm2711(model) && (strncmp(cpuModel, "bcm2711", 8) != 0)) ||
                (RPIHAL_model_SoC_is_bcm2712(model) && (strncmp(cpuModel, "bcm2712", 8) != 0)))
            {
                LOG_WRN("mismatch: detected board model: %llu 0x%016llx, read CPU model: %s", (uint64_t)model, (uint64_t)model, cpuModel);
            }
        }
    }



    if (model == RPIHAL_model_unknown)
    {
        const char* const dtCompatible = RPIHAL_dt_compatible();
        LOG_ERR("unknown board: %s - %s", (dtCompatible ? dtCompatible : "<null>"), (dtModel ? dtModel : "<null>"));
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
        // replace null characters with ',', except for the terminating null (the last char in the file is null and readAllText appends a null, so the buffer
        // contains two null chars at the end)
        for (ssize_t i = 0; i < (sz - 1); ++i)
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
