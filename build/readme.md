# Build
There are two methods available: [referenced](#referencedlinked) by the linking project (your application) and [stand alone](#stand-alone-export-binaries-lib-and-include-directories).



## Referenced/Linked
Added to another _CMakeLists.txt_, maybe in conjunction with git submodules. This way the released application and the emu application can use the same CMake file.
The _CMakeLists.txt_ of the application may look like this:
```cmake
cmake_minimum_required(VERSION 3.13)
project(my-app)

# detect if target platform is RasPi (actually any ARM platform, may be improved)
set(PLAT_IS_RASPI false)
#message(${CMAKE_SYSTEM_PROCESSOR})
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(armv[6-8]([A-Z]|[a-z])?)" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(PLAT_IS_RASPI true)
endif()

include_directories(../../sdk/rpihal/include)
add_definitions(-DRPIHAL_CONFIG_LOG_LEVEL=2)
if(NOT PLAT_IS_RASPI)
    add_definitions(-DRPIHAL_EMU)
    set(RPIHAL_CMAKE_CONFIG_EMU true)
endif()
add_subdirectory(../../sdk/rpihal/build/librpihal.a/ ../../sdk/rpihal/build/librpihal.a/)



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

See [rpihal-system-test](https://github.com/oblaser/rpihal-system-test) as an example.



## Stand Alone, Export Binaries (_lib_ and _include_ directories)
To build the library stand alone, use the [build.sh](./build.sh) script, which uses [CMakeLists.txt](./cmake/CMakeLists.txt). The built libraries (static and shared) are copied to the created _lib_ directory.

See [GPIO test makefile](../test/system/gpio/makefile) or [staircase example CMake](../examples/staircase/build/cmake/CMakeLists.txt) as an example.

### Build Scripts
> `dep_*` files are dependencies of the scripts and should not be called directly.

The scripts must have this directory as working directory.

They return __0__ on success and __1__ on error.
Eexcept the `pack_*` scripts don't.

```
$ ./build.sh [cleanAll] cmake make
```



---

### files to edit on version update

- `build/dep_vstr.txt`
- `include/rpihal/defs.h`
