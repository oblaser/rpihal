
# author        Oliver Blaser
# date          23.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal-example-print-cpu-temp)

include_directories(../../include/)
link_directories(../../lib/)

set(EXE rpihal-example-print-cpu-temp)

set(SOURCES
main.c
)

add_executable(${EXE} ${SOURCES})
target_link_libraries(${EXE} librpihal.a)
