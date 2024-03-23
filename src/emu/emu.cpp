/*
author          Oliver Blaser
date            23.03.2024
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

/*
    This emulator uses the OneLoneCoder Pixel Game Engine https://github.com/OneLoneCoder/olcPixelGameEngine
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

#include <array>
#include <iostream>
#include <mutex>
#include <thread>

#include "emu.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


using std::cout;
using std::endl;



//======================================================================================================================
// emu abstraction

class Gpio
{
public:
    Gpio() {}
    virtual ~Gpio() {}
};

class Pin
{
public:
    static constexpr size_t num = 40;

    enum class Type
    {
        supply5V,
        supply3V3,
        ground,
        gpio,
    };

public:
    Pin() {}
    virtual ~Pin() {}

    bool is5V() const { return m_type == Type::supply5V; }
    bool is3V3() const { return m_type == Type::supply3V3; }
    bool isGnd() const { return m_type == Type::ground; }
    bool isGpio() const { return m_type == Type::gpio; }

private:
    Type m_type;
};

static std::array<Pin, Pin::num> pins;



//======================================================================================================================
// emu thread stuff

// clang-format off
class SharedData
{
public:
    using lock_guard = std::lock_guard<std::mutex>;

public:
    SharedData() : m_booted(false), m_terminate(false) {}
    virtual ~SharedData() {}

    void setBooted(bool state = true) { lock_guard lg(m_mtxThCtrl); m_booted = state; }
    bool hasBooted() const { lock_guard lg(m_mtxThCtrl); return m_booted; }

    void terminate(bool state = true) { lock_guard lg(m_mtxThCtrl); m_terminate = state; }
    bool testTerminate() const { lock_guard lg(m_mtxThCtrl); return m_terminate; }

protected:
    mutable std::mutex m_mtxThCtrl;
    bool m_booted;
    bool m_terminate;
};
// clang-format on

static SharedData thread_pge_sd;
static std::thread thread_pge;



//======================================================================================================================
// PGE

using namespace olc;



Emu::Emu()
{
    sAppName = "rpihal emu";

    // https://pinout.xyz/
    pins[0] = Pin();
}

bool Emu::OnUserCreate()
{
#ifdef WIN32
    {
        RECT desktop, pge;
        HWND hwnd_pge = FindWindowExW(nullptr, nullptr, L"OLC_PIXEL_GAME_ENGINE", nullptr);

        GetClientRect(GetDesktopWindow(), &desktop);
        GetClientRect(hwnd_pge, &pge);

        int x = ((desktop.right - desktop.left) / 2) - ((pge.right - pge.left) / 2);
        int y = ((desktop.bottom - desktop.top) * 1 / 3) - ((pge.bottom - pge.top) / 2);
        //const int y = 10;

        if (x < 1) x = 1;
        if (y < 1) y = 1;

        SetWindowPos(hwnd_pge, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
#endif

    thread_pge_sd.setBooted(true);

    return true;
}

bool Emu::OnUserUpdate(float tElapsed)
{
    const auto mousePos = GetMousePos();

    Clear(VERY_DARK_BLUE);

    const auto headerPos = vi2d(700, 50);
    constexpr int32_t size = 30;

    for (size_t i = 0; i < pins.size(); ++i)
    {
        auto pos = headerPos + vi2d(0, (i / 2) * size);

        if (i & 1) pos.x += size;

        auto pixel = Pixel(25, 25, 25);

        if ((i == 0) || (i == 16)) pixel = Pixel(130, 100, 6);
        if ((i == 1) || (i == 3)) pixel = Pixel(150, 33, 30);
        if ((i == 5) || (i == 8) || (i == 13) || (i == 19) || (i == 24) || (i == 29) || (i == 33) || (i == 38)) pixel = Pixel(0, 0, 0);

        FillCircle(pos, (size / 2.0) * 0.7, pixel);
    }

    return !thread_pge_sd.testTerminate();
}

bool Emu::OnUserDestroy() { return true; }



//======================================================================================================================
// rpihal emu implementation

#include "../../include/rpihal/defs.h"
#include "../../include/rpihal/gpio.h"
#include "../../include/rpihal/rpihal.h"
#include "../../include/rpihal/sys.h"
#include "../../include/rpihal/uart.h"


static void emuMain()
{
    Emu emu;

#ifdef _DEBUG
    constexpr bool vSync = false;
#else
    constexpr bool vSync = true;
#endif

    if (emu.Construct(1120, 700, 1, 1, false, vSync)) { emu.Start(); }
}

int RPIHAL_init()
{
    int r = -1;

    try
    {
        thread_pge = std::thread(emuMain);
        r = 0;
    }
    catch (const std::exception& ex)
    {
        cout << "\033[30;41m                                   \033[39;49m" << endl;
        cout << "\033[30;41m  failed to start emulator thread  \033[39;49m" << endl;
        cout << "\033[30;41m                                   \033[39;49m" << endl;
    }

    return r;
}

void RPIHAL_cleanup_emu()
{
    volatile int dummy = 0;

    while (!thread_pge_sd.hasBooted()) { ++dummy; }

    thread_pge_sd.terminate();
    thread_pge.join();
}

int RPIHAL_GPIO_init() { return 0; } // nop

int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_readPin(int pin)
{
    /* TODO */
    return 0;
}

uint32_t RPIHAL_GPIO_read()
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_writePin(int pin, int state)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_set(uint32_t bits)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_clr(uint32_t bits)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_togglePin(int pin)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_reset()
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_resetPin(int pin)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct)
{
    /* TODO */
    return 0;
}

int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct)
{
    /* TODO */
    return 0;
}
