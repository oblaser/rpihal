
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

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../include/)

set(SOURCES_INTERNAL
../../../src/internal/gpio.c
../../../src/internal/util.c
)

if(RPIHAL_CMAKE_CONFIG_EMU)

    # note that the RPIHAL_EMU symbol has to be defined on project level
    set(SOURCES 
    ${SOURCES_INTERNAL}
    ../../../src/emu/emu.cpp
    )

else() # RPIHAL_CMAKE_CONFIG_EMU

    set(SOURCES
    ${SOURCES_INTERNAL}
    ../../../src/gpio.c
    ../../../src/i2c.c
    ../../../src/int.c
    ../../../src/rpihal.c
    ../../../src/spi.c
    ../../../src/sys.c
    ../../../src/uart.c
    )

endif() # RPIHAL_CMAKE_CONFIG_EMU



add_library(${BINNAME} STATIC ${SOURCES})
set_target_properties(${BINNAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../../lib/)
target_compile_options(${BINNAME} PRIVATE -Wall -Werror=return-type -Werror=discarded-qualifiers -Werror=int-conversion -Werror=implicit-function-declaration)

if(RPIHAL_CMAKE_CONFIG_EMU AND UNIX AND NOT APPLE)
    target_link_libraries(${BINNAME} X11 GL pthread png)
endif()
