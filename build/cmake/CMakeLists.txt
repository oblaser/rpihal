
# author        Oliver Blaser
# date          04.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal)

include_directories(../../include/)

set(SOURCES
../../test/system/main.cpp
../../src/gpio.c
)

add_executable(rpihal-system-test ${SOURCES})
