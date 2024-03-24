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
#include <queue>
#include <thread>

#include "emu.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"



#define LOG_ERR(msg) cout << "\033[90mEMU \033[91mERR " << msg << "\033[39m" << endl
#define LOG_INF(msg) cout << "\033[90mEMU \033[39mINF " << msg << "\033[39m" << endl



using std::cout;
using std::endl;



//======================================================================================================================
// emu abstraction

#include "../../include/rpihal/gpio.h"

namespace emu {

class Gpio
{
public:
    Gpio()
        : m_config(), m_state(false)
    {
        m_config.mode = RPIHAL_GPIO_MODE_IN;
        m_config.pull = RPIHAL_GPIO_PULL_NONE;
        m_config.altfunc = RPIHAL_GPIO_AF_0;
    }

    Gpio(const RPIHAL_GPIO_init_t& config, bool state = false)
        : m_config(config), m_state(state)
    {}

    virtual ~Gpio() {}

    bool isInput() const { return (m_config.mode == RPIHAL_GPIO_MODE_IN); }
    bool isOutput() const { return (m_config.mode == RPIHAL_GPIO_MODE_OUT); }
    bool isAltFunc() const { return (m_config.mode == RPIHAL_GPIO_MODE_AF); }

    bool read() const { return m_state; }
    void write(bool state) { m_state = state; }

    bool getState() const { return m_state; }
    void setState(bool state) { m_state = state; }

private:
    RPIHAL_GPIO_init_t m_config;
    bool m_state;
};

class Pin
{
public:
    static constexpr size_t nPins = 40;

    enum class Type
    {
        uninit, // uinitialised Pin object or uininitialised GPIO pin
        supply5V,
        supply3V3,
        ground,
        gpio,
        id,
    };

public:
    Pin()
        : m_type(Type::uninit), m_gpio()
    {}

    Pin(const Gpio& gpio)
        : m_type(Type::gpio), m_gpio(gpio)
    {}

    Pin(Pin::Type type, const Gpio& gpio = Gpio())
        : m_type(type), m_gpio(gpio)
    {}

    virtual ~Pin() {}

    bool isUninit() const { return m_type == Type::uninit; }
    bool is5V() const { return m_type == Type::supply5V; }
    bool is3V3() const { return m_type == Type::supply3V3; }
    bool isGnd() const { return m_type == Type::ground; }
    bool isGpio() const { return m_type == Type::gpio; }
    bool isId() const { return m_type == Type::id; }

    Gpio& gpio() { return m_gpio; }
    const Gpio& gpio() const { return m_gpio; }

    bool getState() const { return m_gpio.getState(); }
    void setState(bool state) { m_gpio.setState(state); }

private:
    Type m_type;
    Gpio m_gpio;
};

size_t gpioPin_to_pinIdx(int pin)
{
    size_t idx = 0;

    // https://pinout.xyz/
    // https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-and-the-40-pin-header

    // convert GPIO pin number to GPIO pinheader pin number
    if (pin == 0) idx = 27;
    else if (pin == 1) idx = 28;
    else if (pin == 2) idx = 3;
    else if (pin == 3) idx = 5;
    else if (pin == 4) idx = 7;
    else if (pin == 5) idx = 29;
    else if (pin == 6) idx = 31;
    else if (pin == 7) idx = 26;
    else if (pin == 8) idx = 24;
    else if (pin == 9) idx = 21;
    else if (pin == 10) idx = 19;
    else if (pin == 11) idx = 23;
    else if (pin == 12) idx = 32;
    else if (pin == 13) idx = 33;
    else if (pin == 14) idx = 8;
    else if (pin == 15) idx = 10;
    else if (pin == 16) idx = 36;
    else if (pin == 17) idx = 11;
    else if (pin == 18) idx = 12;
    else if (pin == 19) idx = 35;
    else if (pin == 20) idx = 38;
    else if (pin == 21) idx = 40;
    else if (pin == 22) idx = 15;
    else if (pin == 23) idx = 16;
    else if (pin == 24) idx = 18;
    else if (pin == 25) idx = 22;
    else if (pin == 26) idx = 37;
    else if (pin == 27) idx = 13;
    else { throw std::runtime_error("GPIO " + std::to_string(pin) + " is not a user pin"); }

    // convert GPIO pinheader pin number to array index
    return idx - 1;
}

} // namespace emu



//======================================================================================================================
// emu thread stuff

class SharedData
{
public:
    using lock_guard = std::lock_guard<std::mutex>;

    using GpioPins = std::array<emu::Pin, emu::Pin::nPins>;

public:
    SharedData()
        : m_booted(false), m_terminate(false), m_pins()
    {
        using emu::Pin;

        for (auto& pin : m_pins) { pin = Pin(); }

        m_pins[1] = Pin(Pin::Type::supply5V);
        m_pins[3] = Pin(Pin::Type::supply5V);

        m_pins[0] = Pin(Pin::Type::supply3V3);
        m_pins[16] = Pin(Pin::Type::supply3V3);

        m_pins[5] = Pin(Pin::Type::ground);
        m_pins[8] = Pin(Pin::Type::ground);
        m_pins[13] = Pin(Pin::Type::ground);
        m_pins[19] = Pin(Pin::Type::ground);
        m_pins[24] = Pin(Pin::Type::ground);
        m_pins[29] = Pin(Pin::Type::ground);
        m_pins[33] = Pin(Pin::Type::ground);
        m_pins[38] = Pin(Pin::Type::ground);

        m_pins[26] = Pin(Pin::Type::id);
        m_pins[27] = Pin(Pin::Type::id);
    }
    virtual ~SharedData() {}

    // clang-format off
    void setBooted(bool state = true) { lock_guard lg(m_mtxThCtrl); m_booted = state; }
    bool hasBooted() const { lock_guard lg(m_mtxThCtrl); return m_booted; }

    void terminate(bool state = true) { lock_guard lg(m_mtxThCtrl); m_terminate = state; }
    bool testTerminate() const { lock_guard lg(m_mtxThCtrl); return m_terminate; }

    GpioPins getPins() const { lock_guard lg(m_mtxPins); return m_pins; }
    void setPin(size_t idx, const emu::Pin& pin) { lock_guard lg(m_mtxPins); m_pins[idx] = pin; }
    void setPinState(size_t idx, bool state) { lock_guard lg(m_mtxPins); m_pins[idx].setState(state); }
    // clang-format on

    int setGpioConfig(int pin, const emu::Gpio& gpio)
    {
        lock_guard lg(m_mtxPins);

        int r = -1;

        try
        {
            m_pins[emu::gpioPin_to_pinIdx(pin)] = gpio;

            r = 0;
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
        }

        return r;
    }

    int getGpioState(int pin) const
    {
        lock_guard lg(m_mtxPins);

        int r = -1;

        try
        {
            const bool state = m_pins[emu::gpioPin_to_pinIdx(pin)].getState();

            r = (state ? 1 : 0);
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
        }

        return r;
    }

    void setGpioState(int pin, bool state)
    {
        lock_guard lg(m_mtxPins);

        try
        {
            setPinState(emu::gpioPin_to_pinIdx(pin), state);
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
        }
    }

protected:
    mutable std::mutex m_mtxThCtrl;
    bool m_booted;
    bool m_terminate;

    mutable std::mutex m_mtxPins;
    GpioPins m_pins;
};


static SharedData thread_pge_sd;
static std::thread thread_pge;



//======================================================================================================================
// PGE

using namespace olc;



EmuPge::EmuPge() { sAppName = "rpihal emu"; }

bool EmuPge::OnUserCreate()
{
#ifdef WIN32
    {
        RECT desktop, pge;
        HWND hwnd_pge = FindWindowExW(nullptr, nullptr, L"OLC_PIXEL_GAME_ENGINE", nullptr);

        GetClientRect(GetDesktopWindow(), &desktop);
        GetClientRect(hwnd_pge, &pge);

        int x = ((desktop.right - desktop.left) / 2) - ((pge.right - pge.left) / 2);
        int y = ((desktop.bottom - desktop.top) * 1 / 3) - ((pge.bottom - pge.top) / 2);
        // const int y = 10;

        if (x < 1) x = 1;
        if (y < 1) y = 1;

        SetWindowPos(hwnd_pge, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
#endif

    thread_pge_sd.setBooted(true);

    return true;
}

bool EmuPge::OnUserUpdate(float tElapsed)
{
    auto pins = thread_pge_sd.getPins();

    const auto mousePos = GetMousePos();

    Clear(VERY_DARK_BLUE);

    const auto headerPos = vi2d(700, 50);
    constexpr int32_t size = 30;

    const auto pcbColor = Pixel(0x5f, 0x86, 0x45);

    FillRect(headerPos - vi2d(size, size) / 2, vi2d(2 * size, 20 * size), Pixel(10, 10, 10));

    for (size_t i = 0; i < pins.size(); ++i)
    {
        auto pos = headerPos + vi2d(0, (i / 2) * size);
        const auto& pin = pins[i];

        if (i & 1) pos.x += size;

        auto pixel = RED;

        if (pin.is5V()) pixel = Pixel(150, 33, 30);
        else if (pin.is3V3()) pixel = Pixel(130, 100, 6);
        else if (pin.isGnd()) pixel = Pixel(0, 0, 0);
        else if (pin.isId()) pixel = Pixel(160, 160, 160);
        else if (pin.isGpio())
        {
            const auto& gpio = pin.gpio();

            if (gpio.isInput()) pixel = (gpio.read() ? CYAN : CYAN - 50);
            else if (gpio.isOutput()) pixel = (gpio.read() ? GREEN : GREEN - 50);
            else if (gpio.isAltFunc()) pixel = Pixel(94, 0, 181);
        }
        else pixel = Pixel(50, 50, 50);

        FillCircle(pos, (size / 2.0) * 0.7, pixel);

        // const auto pinRectSize = vi2d(5, 5);
        // FillRect(pos - pinRectSize / 2, pinRectSize, Pixel(255, 215, 0));
    }

    return !thread_pge_sd.testTerminate();
}

bool EmuPge::OnUserDestroy() { return true; }



//======================================================================================================================
// rpihal emu implementation

#include "../../include/rpihal/defs.h"
#include "../../include/rpihal/gpio.h"
#include "../../include/rpihal/rpihal.h"
#include "../../include/rpihal/sys.h"
#include "../../include/rpihal/uart.h"


static const int sysGpioLocked = 1;


static int checkPin(int pin);

static void emuMain()
{
    EmuPge emu_pge;

#ifdef _DEBUG
    constexpr bool vSync = false;
#else
    constexpr bool vSync = true;
#endif

    if (emu_pge.Construct(1120, 700, 1, 1, false, vSync)) { emu_pge.Start(); }
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
    int r = 0;

    if (initStruct && checkPin(pin)) { thread_pge_sd.setGpioConfig(pin, emu::Gpio(*initStruct)); }
    else r = 1;

    return r;
}

int RPIHAL_GPIO_readPin(int pin)
{
    int r = 0;

    if (checkPin(pin))
    {
        try
        {
            auto pins = thread_pge_sd.getGpioPins();

            pins = thread_pge_sd.getGpioPins();

            r = (pins[emu::gpioPin_to_pinIdx(pin)].read() ? 1 : 0);

            thread_pge_sd.setGpioPins(pins);
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
            r = -1;
        }
    }
    else r = -1;

    return r;
}

uint32_t RPIHAL_GPIO_read()
{
    uint32_t value = 0;

    auto pins = thread_pge_sd.getGpioPins();

    for (size_t i = 0; i < pins.size(); ++i)
    {
        const auto& pin = pins[i];

        if (pin.isGpio())
        {
            if (pin.read()) value |= (1u << i);
        }
    }

    thread_pge_sd.setGpioPins(pins);

    return value;
}

int RPIHAL_GPIO_writePin(int pin, int state)
{
    int r = 0;

    if (checkPin(pin))
    {
        try
        {
            auto pins = thread_pge_sd.getGpioPins();

            pins = thread_pge_sd.getGpioPins();

            pins[emu::gpioPin_to_pinIdx(pin)].write(state != 0);

            thread_pge_sd.setGpioPins(pins);
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
            r = -1;
        }
    }
    else r = 1;

    return r;
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

//! @param pin BCM GPIO pin number
//! @return Boolean value TRUE (`1`) if it's allowed to access the pin, otherwhise FALSE (`0`)
int checkPin(int pin)
{
    int r = 0;

    // defines from gpio.c
    // !!! DEVICE SPECIFIC !!! DEVSPEC - valid for RasPi (2|3|4) B[+] / 3 A+
#define USER_PINS_MASK (0x0FFFFFFC)
#define FIRST_PIN      0
#define FIRST_USER_PIN 2
#define LAST_USER_PIN  27
#define LAST_PIN       53

    if (sysGpioLocked)
    {
        if ((pin >= FIRST_USER_PIN) && (pin <= LAST_USER_PIN)) r = 1;
    }
    else
    {
        if ((pin >= FIRST_PIN) && (pin <= LAST_PIN)) r = 1;
    }

#undef USER_PINS_MASK
#undef FIRST_PIN
#undef FIRST_USER_PIN
#undef LAST_USER_PIN
#undef LAST_PIN

    return r;
}
