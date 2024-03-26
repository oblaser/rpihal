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

struct PinMap
{
    PinMap() = delete;
    constexpr PinMap(int p, size_t i)
        : pin(p), idx(i)
    {}

    int pin;
    size_t idx;
};

// clang-format off
constexpr std::array<PinMap, 28> pinmap = {
    
    // https://pinout.xyz/
    // https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-and-the-40-pin-header

    PinMap( 0, 27 - 1),
    PinMap( 1, 28 - 1),
    PinMap( 2,  3 - 1),
    PinMap( 3,  5 - 1),
    PinMap( 4,  7 - 1),
    PinMap( 5, 29 - 1),
    PinMap( 6, 31 - 1),
    PinMap( 7, 26 - 1),
    PinMap( 8, 24 - 1),
    PinMap( 9, 21 - 1),
    PinMap(10, 19 - 1),
    PinMap(11, 23 - 1),
    PinMap(12, 32 - 1),
    PinMap(13, 33 - 1),
    PinMap(14,  8 - 1),
    PinMap(15, 10 - 1),
    PinMap(16, 36 - 1),
    PinMap(17, 11 - 1),
    PinMap(18, 12 - 1),
    PinMap(19, 35 - 1),
    PinMap(20, 38 - 1),
    PinMap(21, 40 - 1),
    PinMap(22, 15 - 1),
    PinMap(23, 16 - 1),
    PinMap(24, 18 - 1),
    PinMap(25, 22 - 1),
    PinMap(26, 37 - 1),
    PinMap(27, 13 - 1)
};
// clang-format on

size_t gpioPin_to_pinIdx(int pin)
{
    size_t idx = -1;

    for (const auto& pm : emu::pinmap)
    {
        if (pm.pin == pin)
        {
            idx = pm.idx;
            break;
        }
    }

    if (idx >= Pin::nPins) { throw std::runtime_error("GPIO " + std::to_string(pin) + " is not a user pin"); }

    return idx;
}

int pinIdx_to_gpioPin(size_t idx)
{
    int pin = -1;

    for (const auto& pm : emu::pinmap)
    {
        if (pm.idx == idx)
        {
            pin = pm.pin;
            break;
        }
    }

    if (pin < 0) { throw std::runtime_error("pin index " + std::to_string(idx) + " is not a user GPIO pin"); }

    return pin;
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

    GpioPins getPins() const { lock_guard lg(m_mtxPins); const auto r = m_pins; return r; }
    void setPin(size_t idx, const emu::Pin& pin) { lock_guard lg(m_mtxPins); m_pins[idx] = pin; }
    void setPinState(size_t idx, bool state) { lock_guard lg(m_mtxPins); m_setPinState(idx, state); }
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
        catch (const std::exception& ex)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
        }
        catch (...)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin);
        }

        return r;
    }

    //! @return (0 | 1) or negative if failed
    int getGpioState(int pin) const
    {
        lock_guard lg(m_mtxPins);

        int r = -1;

        try
        {
            const bool state = m_pins[emu::gpioPin_to_pinIdx(pin)].getState();

            r = (state ? 1 : 0);
        }
        catch (const std::exception& ex)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
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
            m_setPinState(emu::gpioPin_to_pinIdx(pin), state);
        }
        catch (const std::exception& ex)
        {
            LOG_ERR("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
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

    void m_setPinState(size_t idx, bool state) { m_pins[idx].setState(state); }
};


static SharedData thread_pge_sd;
static std::thread thread_pge;



//======================================================================================================================
// PGE

using namespace olc;

const olc::vi2d EmuPge::pgeWindowSize = vi2d(1120, 700);

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
    const auto mouseR = GetMouse(olc::Mouse::RIGHT);
    const auto mouseL = GetMouse(olc::Mouse::LEFT);

    Clear(VERY_DARK_BLUE);

    const auto headerPos = vi2d(700, 50);
    constexpr int32_t size = 30;

    const auto pcbColor = Pixel(0x5f, 0x86, 0x45);

    FillRect(headerPos - vi2d(size, size) / 2, vi2d(2 * size, 20 * size), Pixel(10, 10, 10));

    int mousePin = -1;
    size_t mousePinIdx = -1;

    for (size_t i = 0; i < pins.size(); ++i)
    {
        constexpr double radius = (size / 2.0) * 0.7;

        auto pos = headerPos + vi2d(0, (i / 2) * size);
        const auto& pin = pins[i];

        if (i & 1) pos.x += size;

        auto pixel = RED;

        if (pin.is5V()) pixel = Pixel(150, 33, 30);
        else if (pin.is3V3()) pixel = Pixel(130, 100, 6);
        else if (pin.isGnd()) pixel = Pixel(0, 0, 0);
        // else if (pin.isId()) pixel = Pixel(160, 160, 160);
        else if (pin.isGpio())
        {
            const auto& gpio = pin.gpio();

            if (gpio.isInput()) pixel = (gpio.read() ? CYAN : CYAN / 3);
            else if (gpio.isOutput()) pixel = (gpio.read() ? GREEN : GREEN / 3);
            else if (gpio.isAltFunc()) pixel = Pixel(94, 0, 181);
        }
        else pixel = Pixel(50, 50, 50);

        FillCircle(pos, radius + 0.5, pixel);

        // const auto pinRectSize = vi2d(5, 5);
        // FillRect(pos - pinRectSize / 2, pinRectSize, Pixel(255, 215, 0));

        // mouse is on pin
        if ((pos - mousePos).mag2() <= (radius * radius))
        {
            auto mouseColor = RED;

            try
            {
                mousePin = emu::pinIdx_to_gpioPin(i);
                mousePinIdx = i;

                if (pins[i].isGpio() && pins[i].gpio().isInput())
                {
                    if (mouseL.bPressed) { thread_pge_sd.setGpioState(mousePin, true); }
                    if (mouseL.bReleased) { thread_pge_sd.setGpioState(mousePin, false); }

                    if (mouseR.bReleased)
                    {
                        const auto state = pins[mousePinIdx].getState();
                        thread_pge_sd.setGpioState(mousePin, !state);
                    }

                    mouseColor = WHITE;
                }
                else mouseColor = MAGENTA;

                if (pins[i].isId()) mouseColor = RED;
            }
            catch (...)
            {
                mouseColor = RED;
            }

            DrawCircle(pos, radius + 0.5, mouseColor);
        }
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
#include "../internal/gpio.h"


static const int sysGpioLocked = 1;
static constexpr /*RPIHAL_regptr_t*/ bool gpio_base = true; // dummy


static int checkPin(int pin);

static void emuMain()
{
    EmuPge emu_pge;

#ifdef _DEBUG
    constexpr bool vSync = true;
#else
    constexpr bool vSync = true;
#endif

    if (emu_pge.Construct(EmuPge::pgeWindowSize.x, EmuPge::pgeWindowSize.y, EmuPge::pgePixelSize, EmuPge::pgePixelSize, false, vSync)) { emu_pge.Start(); }
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

    if (gpio_base && initStruct && checkPin(pin)) thread_pge_sd.setGpioConfig(pin, emu::Gpio(*initStruct));
    else r = 1;

    return r;
}

int RPIHAL_GPIO_readPin(int pin)
{
    int r = 0;

    if (gpio_base && checkPin(pin)) r = thread_pge_sd.getGpioState(pin);
    else r = -1;

    return r;
}

uint32_t RPIHAL_GPIO_read()
{
    uint32_t value = 0;

    auto pins = thread_pge_sd.getPins();

    for (size_t i = 0; i < pins.size(); ++i)
    {
        const auto& pin = pins[i];

        if (pin.isGpio() && pin.getState())
        {
            try
            {
                value |= (1u << emu::pinIdx_to_gpioPin(i));
            }
            catch (...)
            {
                // nop
            }
        }
    }

    return value;
}

int RPIHAL_GPIO_writePin(int pin, int state)
{
    int r = 0;

    if (gpio_base && checkPin(pin)) thread_pge_sd.setGpioState(pin, state != 0);
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
    int r = 0;

    if (gpio_base && checkPin(pin)) thread_pge_sd.setGpioState(pin, !thread_pge_sd.getGpioState(pin));
    else r = -1;

    return r;
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

    if (sysGpioLocked)
    {
        if ((pin >= FIRST_USER_PIN) && (pin <= LAST_USER_PIN)) r = 1;
    }
    else
    {
        if ((pin >= FIRST_PIN) && (pin <= LAST_PIN)) r = 1;
    }

    return r;
}
