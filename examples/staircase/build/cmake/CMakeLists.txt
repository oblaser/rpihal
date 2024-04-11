
# author        Oliver Blaser
# date          10.03.2024
# copyright     MIT - Copyright (c) 2024 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal-example-staircase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(EXE rpihal-example-staircase)

include_directories(../../src/)

include_directories(../../../../../rpihal/include/)
link_directories(../../../../../rpihal/lib/)

if(_DEBUG)
    add_compile_definitions(_DEBUG)
endif()

set(SOURCES
../../src/main.c
)

add_executable(${EXE} ${SOURCES})
target_link_libraries(${EXE} librpihal.a)
