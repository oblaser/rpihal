# raspihal

A Raspberry Pi Hardware Abstraction Layer for C/C++ projects.

> Not completed. Grows as I need it.

The system tests can also be considered as examples :wink:



## GPIO Module

The library is now aware of the hardware it's running on.

> __CAUTION!__ Unexpected behaviour may be observed on alternate functions (see [ANOM1](anomalies.md#anom1---gpio-alternate-function-registers)).

### Supported Models
`2B` and newer. Zeros, Compute Modules and the `400` could not yet be tested.

> Search for _ADDHW_ comments in code to find sections which are crucial for implementation of more hardware support.



## Emulator

The emulator can be used to run your project on your working machine. To build the emulator project
include all the same files as in the main project, plus the [emu.cpp](src/emu/emu.cpp) file (don't include any other
rpihal source file to the build, nor link to the rpihal), and define `RPIHAL_EMU` on the compiler level (not anyehere in
source nor header files). See the [staircase example](examples/staircase/emu/cmake/CMakeLists.txt#L19) as a reference.
To create a new emu project in Visual Studio use the _New > Project From Existing Code..._ funtion and create an empty
console application. Then add all the needed source files and add the include directories.

The left and right mouse button can be used to click (push a button) or toggle an input pin.

The emulator uses the OneLoneCoder [Pixel Game Engine](https://github.com/OneLoneCoder/olcPixelGameEngine) for the graphics.



---

# TODO
- add more `RPIHAL_ui128_..` functions
- add to unit test
  - `RPIHAL_GPIO_bittopin`
  - `RPIHAL_GPIO_pintobit`
  - `RPIHAL_ui128_..`
