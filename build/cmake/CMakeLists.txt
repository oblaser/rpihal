
# author        Oliver Blaser
# date          23.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal)

set(BINNAME rpihal)



set(BINSHARED ${BINNAME}-shared)
set(BINSTATIC ${BINNAME}-static)
set(UNIT_TEST_EXE_SHARED ${BINNAME}-unit-test-shared)
set(UNIT_TEST_EXE_STATIC ${BINNAME}-unit-test-static)

include_directories(../../include/)

set(SOURCES
../../src/internal/util.c
../../src/gpio.c
../../src/int.c
../../src/rpihal.c
../../src/sys.c
../../src/uart.c
)

add_library(${BINSHARED} SHARED ${SOURCES})
add_library(${BINSTATIC} STATIC ${SOURCES})


# unit test

#link_directories(./)
#
#set(SOURCES_UNIT_TEST
#../../tests/unit/main.cpp
#)
#
#add_executable(${UNIT_TEST_EXE_SHARED} ${SOURCES_UNIT_TEST})
#target_link_libraries(${UNIT_TEST_EXE_SHARED} lib${BINSHARED}.so)
#
#add_executable(${UNIT_TEST_EXE_STATIC} ${SOURCES_UNIT_TEST})
#target_link_libraries(${UNIT_TEST_EXE_STATIC} lib${BINSTATIC}.a)
