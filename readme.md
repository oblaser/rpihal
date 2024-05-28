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



## Build

### Stand Alone, Export Binaries (_lib_ and _include_ directories)
To build the library stand alone, use the [build.sh](build/build.sh) script, which uses [CMakeLists.txt](build/cmake/CMakeLists.txt). The built libraries (static and shared) are copied to the created _lib_ directory.

The [pack_bin.sh](build/pack_bin.sh) script may then be used to deploy the compiled library.

### Linked to an Application
Added to another _CMakeLists.txt_, maybe in conjunction with git submodules. The _CMakeLists.txt_ of the application may look like this:
```cmake
cmake_minimum_required(VERSION 3.13)
project(my-app)

# detect if target platform is RasPi (actually any ARM platform, may be improved)
set(PLAT_IS_RASPI false)
#message(${CMAKE_SYSTEM_PROCESSOR})
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(armv[6-8]([A-Z]+|[a-z]+)?)" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(PLAT_IS_RASPI true)
endif()

include_directories(../../sdk/rpihal/include)
if(NOT PLAT_IS_RASPI)
    add_definitions(-DRPIHAL_EMU)
    set(RPIHAL_CMAKE_CONFIG_EMU true)
endif()
add_subdirectory(../../sdk/rpihal/build/cmake/librpihal.a/ ../../sdk/rpihal/build/cmake/librpihal.a/)



set(BINNAME myapp)

include_directories(../../src/)

set(SOURCES
../../src/middleware/gpio.cpp
../../src/middleware/util.cpp
../../src/main.cpp
)

add_executable(${BINNAME} ${SOURCES})
target_link_libraries(${BINNAME} rpihal)
```

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



---

## TODO
- add more `RPIHAL_ui128_..` functions
- add to unit test
  - `RPIHAL_GPIO_bittopin`
  - `RPIHAL_GPIO_pintobit`
  - `RPIHAL_ui128_..`
- test log module and levels
