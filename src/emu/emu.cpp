/*
author          Oliver Blaser
date            03.11.2024
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
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

#include "emu.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  RPIHAL_EMU
#include "../internal/log.h"



#define LOG_ERR_cout(msg) cout << "\033[90mEMU \033[91mERR " << msg << "\033[39m" << endl
#define LOG_INF_cout(msg) cout << "\033[90mEMU \033[39mINF " << msg << "\033[39m" << endl



using std::cout;
using std::endl;

namespace {

// clamp value y to [a;b]
template <typename T> void clamp(T& v, const T& a, const T& b) { v = (v < a ? a : (v > b ? b : v)); }

} // namespace



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
        : m_running(false), m_booted(false), m_terminate(false), m_pins(), m_cpuTemp(42.7f)
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
    void setRunning(bool state = true) { lock_guard lg(m_mtxThCtrl); m_running = state; }
    bool isRunning() const { lock_guard lg(m_mtxThCtrl); return m_running; }

    void setBooted(bool state = true) { lock_guard lg(m_mtxThCtrl); m_booted = state; }
    bool hasBooted() const { lock_guard lg(m_mtxThCtrl); return m_booted; }

    void terminate(bool state = true) { lock_guard lg(m_mtxThCtrl); m_terminate = state; }
    bool testTerminate() const { lock_guard lg(m_mtxThCtrl); return m_terminate; }

    GpioPins getPins() const { lock_guard lg(m_mtxPins); return m_pins; }
    void setPin(size_t idx, const emu::Pin& pin) { lock_guard lg(m_mtxPins); m_pins[idx] = pin; }
    void setPinState(size_t idx, bool state) { lock_guard lg(m_mtxPins); m_setPinState(idx, state); }

    std::string getModelStr() const { lock_guard lg(m_mtxGeneral); return m_modelStr; }
    void setModelStr(const std::string& str) { lock_guard lg(m_mtxGeneral); m_modelStr = str; }

    float getCpuTemp() const { lock_guard lg(m_mtxGeneral); return m_cpuTemp; }
    void setCpuTemp(float temp) { lock_guard lg(m_mtxGeneral); m_cpuTemp = temp; }
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
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
        }
        catch (...)
        {
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin);
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
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
        }
        catch (...)
        {
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin);
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
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin << " (" << ex.what() << ")");
        }
        catch (...)
        {
            LOG_ERR_cout("failed to convert pin number of GPIO " << pin);
        }
    }

protected:
    mutable std::mutex m_mtxThCtrl;
    bool m_running;
    bool m_booted;
    bool m_terminate;

    mutable std::mutex m_mtxPins;
    GpioPins m_pins;

    mutable std::mutex m_mtxGeneral;
    std::string m_modelStr;
    float m_cpuTemp;

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
    const auto cpuTemp = thread_pge_sd.getCpuTemp();

    const auto mousePos = GetMousePos();
    const auto mouseR = GetMouse(olc::Mouse::RIGHT);
    const auto mouseL = GetMouse(olc::Mouse::LEFT);
    const auto mouseWd = GetMouseWheel();

    Clear(VERY_DARK_BLUE);

    DrawString(vi2d(1, 1) * 10, thread_pge_sd.getModelStr(), WHITE, 2);



    //
    // pin header
    //
    {
        const auto headerPos = vi2d(700, 50);
        constexpr int32_t size = 30;

        const auto pcbColor = Pixel(0x5f, 0x86, 0x45);

        FillRect(headerPos - vi2d(size, size) / 2, vi2d(2 * size, 20 * size), Pixel(10, 10, 10));

        int mousePin = -1;
        size_t mousePinIdx = -1;

        for (size_t i = 0; i < pins.size(); ++i)
        {
            constexpr float radius = (size / 2.0) * 0.7;

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

            FillCircle(pos, (int32_t)(radius + 0.5), pixel);

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
                        if (mouseL.bPressed || mouseL.bReleased) // left mouse button generates click, neg or pos is depending on current state
                        {
                            const auto state = pins[mousePinIdx].getState();
                            thread_pge_sd.setGpioState(mousePin, !state);
                        }

                        if (mouseR.bReleased) // right mouse button toggles the pin
                        {
                            const auto state = pins[mousePinIdx].getState();
                            thread_pge_sd.setGpioState(mousePin, !state);
                        }

                        mouseColor = WHITE;
                    }
                    else mouseColor = GREEN;

                    if (pins[i].isId()) mouseColor = RED;
                }
                catch (...)
                {
                    mouseColor = RED;
                }

                DrawCircle(pos, (int32_t)(radius + 0.5), mouseColor);
            }
        }



        //
        // CPU temp slider
        //
        {
            static bool mouseDownWasOnSlider = false;
            static int32_t mouseOffset = 0;

            constexpr int32_t radius = 9;
            constexpr int32_t origX = 100;
            constexpr int32_t origY = 100;
            constexpr int32_t len = 200;
            const auto orig = vi2d(origX, origY);
            const auto end = orig + vi2d(0, len);
            const auto boxO = vi2d(origX - radius + 1, origY);
            const auto boxE = vi2d(origX + radius - 1, origY + len);

            const bool mouseIsInBox = !(mousePos.x < boxO.x || mousePos.x > boxE.x || mousePos.y < boxO.y || mousePos.y > boxE.y);

            constexpr float tempLo = 5;
            constexpr float tempHi = 90;
            constexpr float tempSpan = tempHi - tempLo;
            auto temp_to_normal = [&](const float& temp) { return 1 - ((temp - tempLo) / tempSpan); };
            auto normal_to_temp = [&](const float& norm) { return tempLo + (1 - norm) * tempSpan; };

            int32_t value = (int32_t)(len * temp_to_normal(cpuTemp) + 0.5f);

            if (mouseDownWasOnSlider)
            {
                value = (float)(mousePos.y) - origY - mouseOffset;
                clamp(value, 0, len);
            }
            else
            {
                if (mouseWd != 0 && mouseIsInBox) { value += (int32_t)round(mouseWd / -120.0f); }
            }

            const auto handlePos = orig + vi2d(0, value);
            const bool mouseIsOnHandle = (handlePos - mousePos).mag2() <= (radius * radius);

            if (mouseL.bPressed)
            {
                if (mouseIsOnHandle)
                {
                    mouseDownWasOnSlider = true;
                    mouseOffset = mousePos.y - handlePos.y;
                }

                if (mouseIsInBox)
                {
                    mouseDownWasOnSlider = true;
                    mouseOffset = 0;
                }
            }
            if (mouseL.bReleased) { mouseDownWasOnSlider = false; }

            thread_pge_sd.setCpuTemp(normal_to_temp((float)value / len));

            // DrawRect(boxO, boxE - boxO);
            DrawLine(orig, end, VERY_DARK_GREY);
            FillCircle(handlePos, radius, DARK_GREY);
        }
    }

    return !thread_pge_sd.testTerminate();
}

bool EmuPge::OnUserDestroy() { return true; }



// clang-format off



//######################################################################################################################
//                                                                                                                     #
//  rpihal emu implementation                                                                                          #
//                                                                                                                     #
//######################################################################################################################
// clang-format on

#include "../../include/rpihal/defs.h"
#include "../../include/rpihal/gpio.h"
#include "../../include/rpihal/rpihal.h"
#include "../../include/rpihal/sys.h"
#include "../../include/rpihal/uart.h"
#include "../internal/gpio.h"

static void emuMain()
{
    EmuPge emu_pge;

    thread_pge_sd.setRunning();

#ifdef _DEBUG
    constexpr bool vSync = true;
#else
    constexpr bool vSync = true;
#endif

    if (emu_pge.Construct(EmuPge::pgeWindowSize.x, EmuPge::pgeWindowSize.y, EmuPge::pgePixelSize, EmuPge::pgePixelSize, false, vSync)) { emu_pge.Start(); }

    thread_pge_sd.setRunning(false);
}

//======================================================================================================================
// rpihal.h

static RPIHAL_model_t rpihal_emu_rpi_model = RPIHAL_model_unknown;

class RPIHAL_EMU_dt_comp_model
{
public:
    RPIHAL_EMU_dt_comp_model() = delete;
    RPIHAL_EMU_dt_comp_model(const std::string& compatible, const std::string& model)
        : m_compatible(compatible), m_model(model)
    {}
    const std::string& compatible() const { return m_compatible; }
    const std::string& model() const { return m_model; }

private:
    std::string m_compatible;
    std::string m_model;
};

// https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#check-raspberry-pi-model-and-cpu-across-distributions
static const std::map<RPIHAL_model_t, RPIHAL_EMU_dt_comp_model> rpihal_emu_dt_comp_model_map = {
    // clang-format off
    { RPIHAL_model_unknown, RPIHAL_EMU_dt_comp_model("raspberrypi,?,brcm,?",                                                                "unknown"                                             ) },
    { RPIHAL_model_2B,      RPIHAL_EMU_dt_comp_model("raspberrypi,2-model-b,brcm,bcm2836",                                                  "Raspberry Pi 2 Model B Rev 1.1"                      ) },
    { RPIHAL_model_2B_v1_2, RPIHAL_EMU_dt_comp_model("raspberrypi,2-model-b,brcm,bcm2837" /* guessed "bcm2837" */,                          "Raspberry Pi 2 Model B Rev 1.2"                      ) },
    { RPIHAL_model_3B,      RPIHAL_EMU_dt_comp_model("raspberrypi,3-model-b,brcm,bcm2837",                                                  "Raspberry Pi 3 Model B Rev 1.2"                      ) },
    { RPIHAL_model_cm3,     RPIHAL_EMU_dt_comp_model("raspberrypi,3-compute-module,brcm,bcm2837",                                           "Raspberry Pi Compute Module 3 Rev 1.0"               ) },
    { RPIHAL_model_z2W,     RPIHAL_EMU_dt_comp_model("raspberrypi,model-zero-2-w,brcm,bcm2837",                                             "Raspberry Pi Zero 2 W Rev 1.0"                       ) },
    { RPIHAL_model_3Ap,     RPIHAL_EMU_dt_comp_model("raspberrypi,3-model-a-plus,brcm,bcm2837",                                             ""                                                    ) },
    { RPIHAL_model_3Bp,     RPIHAL_EMU_dt_comp_model("raspberrypi,3-model-b-plus,brcm,bcm2837",                                             "Raspberry Pi 3 Model B Plus Rev 1.3"                 ) },
    { RPIHAL_model_cm3p,    RPIHAL_EMU_dt_comp_model("raspberrypi,3-compute-module-plus,brcm,bcm2837" /* guessed "-plus" and "bcm2837" */,  "Raspberry Pi Compute Module 3 Plus Rev 1.0"          ) },
    { RPIHAL_model_4B,      RPIHAL_EMU_dt_comp_model("raspberrypi,4-model-b,brcm,bcm2711",                                                  "Raspberry Pi 4 Model B Rev 1.1"                      ) },
    { RPIHAL_model_400,     RPIHAL_EMU_dt_comp_model("raspberrypi,400,brcm,bcm2711",                                                        "Raspberry Pi 400 Rev 1.0"                            ) },
    { RPIHAL_model_cm4,     RPIHAL_EMU_dt_comp_model("raspberrypi,4-compute-module,brcm,bcm2711",                                           "Raspberry Pi Compute Module 4 Rev 1.0" /* guessed */ ) },
    // clang-format on
};

RPIHAL_model_t RPIHAL_getModel() { return rpihal_emu_rpi_model; }

const char* RPIHAL_dt_compatible()
{
    const char* r = "#ERROR#";

    try
    {
        r = rpihal_emu_dt_comp_model_map.at(rpihal_emu_rpi_model).compatible().c_str();
    }
    catch (...)
    {
        LOG_ERR("%s failed for %llu 0x%llx", __func__, (uint64_t)rpihal_emu_rpi_model, (uint64_t)rpihal_emu_rpi_model);
    }

    return r;
}

const char* RPIHAL_dt_model()
{
    const char* r = "#ERROR#";

    try
    {
        r = rpihal_emu_dt_comp_model_map.at(rpihal_emu_rpi_model).model().c_str();
    }
    catch (...)
    {
        LOG_ERR("%s failed for %llu 0x%llx", __func__, (uint64_t)rpihal_emu_rpi_model, (uint64_t)rpihal_emu_rpi_model);
    }

    return r;
}

int RPIHAL_EMU_init(RPIHAL_model_t model)
{
    int r = -1;

    rpihal_emu_rpi_model = model;
    thread_pge_sd.setModelStr(RPIHAL_dt_model());

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

        cout << ex.what() << endl;
    }

    return r;
}

void RPIHAL_EMU_cleanup()
{
    volatile int dummy = 0;

    while (!thread_pge_sd.hasBooted()) { ++dummy; }

    thread_pge_sd.terminate();
    thread_pge.join();
}

int RPIHAL_EMU_isRunning() { return thread_pge_sd.isRunning(); }

//======================================================================================================================
// gpio.h

static const int sysGpioLocked = 1;
constexpr /*RPIHAL_regptr_t*/ bool gpio_base = true; // dummy to make copy-paste more easy

int RPIHAL_GPIO_init() { return 0; } // nop

int RPIHAL_GPIO_initPin(int pin, const RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    if (gpio_base && initStruct && iGPIO_checkPin(pin, sysGpioLocked)) { thread_pge_sd.setGpioConfig(pin, emu::Gpio(*initStruct)); }
    else { r = 1; }

    return r;
}

int RPIHAL_GPIO_initPins(uint64_t bits, const RPIHAL_GPIO_init_t* initStruct)
{
    int r = 0;

    uint64_t mask = 0x01ull;

    while (mask)
    {
        if (mask & bits)
        {
            const int err = RPIHAL_GPIO_initPin(RPIHAL_GPIO_bittopin(mask), initStruct);
            if (err) { r = -(__LINE__); }
        }

        mask <<= 1;
    }

    return r;
}

int RPIHAL_GPIO_readPin(int pin)
{
    int r = 0;

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { r = thread_pge_sd.getGpioState(pin); }
    else { r = -1; }

    return r;
}

uint32_t RPIHAL_GPIO_read() { return (uint32_t)(RPIHAL_GPIO_read64() & 0x00000000FFFFFFFFllu); }
uint32_t RPIHAL_GPIO_readHi() { return (uint32_t)((RPIHAL_GPIO_read64() >> 32) & 0x00000000FFFFFFFFllu); }

uint64_t RPIHAL_GPIO_read64()
{
    uint64_t value = 0;

    const auto pins = thread_pge_sd.getPins();

    for (size_t i = 0; i < pins.size(); ++i)
    {
        const auto& pin = pins[i];

        if (pin.isGpio() && pin.getState())
        {
            try
            {
                value |= (1llu << emu::pinIdx_to_gpioPin(i));
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

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { thread_pge_sd.setGpioState(pin, state != 0); }
    else { r = 1; }

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

    if (gpio_base && iGPIO_checkPin(pin, sysGpioLocked)) { thread_pge_sd.setGpioState(pin, !thread_pge_sd.getGpioState(pin)); }
    else { r = -1; }

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

void RPIHAL_GPIO_defaultInitStruct(RPIHAL_GPIO_init_t* initStruct) { iGPIO_defaultInitStruct(initStruct); }
int RPIHAL_GPIO_defaultInitStructPin(int pin, RPIHAL_GPIO_init_t* initStruct) { return iGPIO_defaultInitStructPin(pin, initStruct); }

int RPIHAL_GPIO_bittopin(uint64_t bit) { return iGPIO_bittopin(bit); }
uint64_t RPIHAL_GPIO_pintobit(int pin) { return iGPIO_pintobit(pin); }

//======================================================================================================================
// sys

int RPIHAL_SYS_getCpuTemp(float* temperature)
{
    int r;

    if (temperature)
    {
        *temperature = thread_pge_sd.getCpuTemp();
        r = 0;
    }
    else r = 1;

    return r;
}
