
# author        Oliver Blaser
# date          22.05.2024
# copyright     MIT - Copyright (c) 2024 Oliver Blaser


################################################################################
#
# This file is not to be used in conjunction with the build.sh script, it's
# meant to be added to a projects CMakeLists.txt file (add_subdirectory).
#
################################################################################


cmake_minimum_required(VERSION 3.13)

project(rpihal)

set(BINNAME rpihal)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../include/)

if(RPIHAL_CMAKE_CONFIG_EMU)

    # note that the RPIHAL_EMU symbol has to be defined on project level
    set(SOURCES 
        ../../src/emu/emu.cpp
    )

else() # RPIHAL_CMAKE_CONFIG_EMU

    set(SOURCES
        ../../src/gpio.c
        ../../src/i2c.c
        ../../src/int.c
        ../../src/rpihal.c
        ../../src/spi.c
        ../../src/sys.c
        ../../src/uart.c
    )

endif() # RPIHAL_CMAKE_CONFIG_EMU



add_library(${BINNAME} STATIC ${SOURCES})

set_target_properties(${BINNAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../lib/)

target_compile_options(${BINNAME} PRIVATE
    -Wall
    -Werror=format
    -Werror=overflow
    -Werror=return-type
    -Wdouble-promotion

    $<$<COMPILE_LANGUAGE:C>:
        -Werror=discarded-qualifiers
        -Werror=implicit
        -Werror=incompatible-pointer-types
        -Werror=int-conversion
        -Werror=pointer-sign
    >

    $<$<COMPILE_LANGUAGE:CXX>:
        -Werror=reorder
        -Werror=switch
    >
)

if(RPIHAL_CMAKE_CONFIG_EMU AND UNIX AND NOT APPLE)
    target_link_libraries(${BINNAME} X11 GL pthread png)
endif()
