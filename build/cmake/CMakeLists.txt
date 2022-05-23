
# author        Oliver Blaser
# date          23.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal)

include_directories(../../include/)
link_directories(./)

set(SHAREDLIB rpihal-shared)
set(STATICLIB rpihal-static)
set(UNIT_TEST_EXE_SHARED rpihal-unit-test-shared)
set(UNIT_TEST_EXE_STATIC rpihal-unit-test-static)


# library

set(SOURCES
../../src/internal/util.c
../../src/gpio.c
../../src/sys.c
../../src/uart.c
)

add_library(${SHAREDLIB} SHARED ${SOURCES})
add_library(${STATICLIB} STATIC ${SOURCES})


# unit test

#set(SOURCES_UNIT_TEST
#../../tests/unit/main.cpp
#)
#
#add_executable(${UNIT_TEST_EXE_SHARED} ${SOURCES_UNIT_TEST})
#target_link_libraries(${UNIT_TEST_EXE_SHARED} lib${SHAREDLIB}.so)
#
#add_executable(${UNIT_TEST_EXE_STATIC} ${SOURCES_UNIT_TEST})
#target_link_libraries(${UNIT_TEST_EXE_STATIC} lib${STATICLIB}.a)
