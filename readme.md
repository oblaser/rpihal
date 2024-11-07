# raspihal

A Raspberry Pi Hardware Abstraction Layer for C/C++ projects.

> Not completed. Grows as I need it.

The system tests can also be considered as examples :wink:

## GPIO Module

The library is now aware of the hardware it's running on.

> __CAUTION!__ Unexpected behaviour may be observed on alternate functions (see [ANOM1](anomalies.md#anom1---gpio-alternate-function-registers)).

### Supported Models
`2B` and newer. Zeros, Compute Modules, the `400` and the `5` could not yet be tested.

> Search for _ADDHW_ comments in code to find sections which are crucial for implementation of more hardware support.



## Portability
The main focus lies on Raspberry Pi OS, but it's attempted to make the code compatible to other distros.
> In fact _Raspberry Pi OS 32bit_ and _Raspberry Pi OS 64bit_ are different distros: _Raspbian_ and a _Debian arm64 port_. See [this article](https://www.tomshardware.com/news/raspberry-pi-os-no-longer-raspbian) on Tom's Hardware for further information.



## Build
See [build/readme.md](./build/readme.md) for the different build methods.

### Build Configuration
#### CMake
- `RPIHAL_CMAKE_CONFIG_EMU` only used in [librpihal.a/CMakeLists.txt](build/cmake/librpihal.a/CMakeLists.txt) (has to be set to 1 before the `add_subdirectory()` call in the parent CMakeLists if needed)

#### Definitions on Compiler Level
- `RPIHAL_EMU` has to be defined on compiler level for the linking code (most likely your application) and rpihal. If using _librpihal.a/CMakeLists.txt_, it has to be added before the `add_subdirectory()` call in the parent CMakeLists.

- `RPIHAL_CONFIG_LOGLEVEL`
  - **0** OFF, no logging from rpihal
  - **1** ERROR
  - **2** WARNING
  - **3** INFO
  - **4** DEBUG



## Emulator

### Build
The emulator can be used to run your project on your working machine. To build the emulator project
include all the same files as in the main project, plus the [emu.cpp](./src/emu/emu.cpp) file and the source files in [src/internal](./src/internal/) (don't include any other
rpihal source file to the build, nor link to the rpihal), and define `RPIHAL_EMU` on the compiler level (not anyehere in
source nor header files).

> To create a new emu project in Visual Studio use the _New > Project From Existing Code..._ funtion and create an empty console application. Then add all the needed source files described above and add the include directories.

See the [staircase example](./examples/staircase/emu/) or [rpihal-system-test](https://github.com/oblaser/rpihal-system-test) for reference.

### Usage
The left and right mouse button can be used to click (push a button) or toggle an input pin.

### Credits
The emulator uses the OneLoneCoder [Pixel Game Engine](https://github.com/OneLoneCoder/olcPixelGameEngine) for the graphics.



---

## TODO
- add more `RPIHAL_ui128_..` functions
- add to unit test
  - `RPIHAL_GPIO_bittopin`
  - `RPIHAL_GPIO_pintobit`
  - `RPIHAL_ui128_..`
- test log module and levels
