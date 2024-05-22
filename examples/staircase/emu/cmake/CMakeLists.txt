
# author        Oliver Blaser
# date          10.03.2024
# copyright     MIT - Copyright (c) 2024 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal-example-staircase-emu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(EXE rpihal-example-staircase-emu)

include_directories(../../src/)

include_directories(../../../../../rpihal/include/)

add_compile_definitions(RPIHAL_EMU)

if(_DEBUG)
    add_compile_definitions(_DEBUG)
endif()

set(SOURCES
../../src/main.c

../../../../../rpihal/src/emu/emu.cpp
)

add_executable(${EXE} ${SOURCES})
target_link_libraries(${EXE})

if(UNIX)
if(NOT APPLE)
target_link_libraries(${EXE} X11 GL pthread png stdc++fs)
endif()
endif()
