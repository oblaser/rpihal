#!/bin/bash

# author        Oliver Blaser
# date          04.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

# Usage:
# ./build.sh --help



source dep_globals.sh

repoDirName=rpihal
exeName=rpihal-system-test
echoTitle="build $exeName"
cmakeDirName=cmake



errCnt=0
function procErrorCode()
{
    if [ $1 -ne 0 ]; then ((++errCnt)); fi;
}

function printHelp()
{
    echo "Usage:"
    echo "  $0 arg1 [arg2 [arg3 [arg4 [arg5]]]]"
    echo ""
    echo "run script in $repoDirName/build/"
    echo ""
    echo "args:"
    echo "  -h help     print help"
    echo "  cleanAll    delete all but CMakeLists.txt"
    echo "  cmake       cmake ."
    echo "  make        make"
    echo "  clean       make clean"
    echo "  run         execute"
}

function cmd_cmake_clean()
{
    rm -rf $cmakeDirName/CMakeFiles/
    procErrorCode $?

    rm -f $cmakeDirName/CMakeCache.txt
    procErrorCode $?

    rm -f $cmakeDirName/cmake_install.cmake
    procErrorCode $?

    rm -f $cmakeDirName/Makefile
    procErrorCode $?

    rm -f $cmakeDirName/$exeName
    procErrorCode $?
}

function cmd_cmake()
{
    cd ./$cmakeDirName
    procErrorCode $?
    cmake .
    procErrorCode $?
    cd ..
    procErrorCode $?
}

function cmd_make()
{
    cd ./$cmakeDirName
    procErrorCode $?
    make
    procErrorCode $?
    cd ..
    procErrorCode $?
}

function cmd_clean()
{
    cd ./$cmakeDirName
    procErrorCode $?
    make clean
    procErrorCode $?
    cd ..
    procErrorCode $?
}

function procArg()
{
    ptintTitle "$echoTitle - $1" 4
    
    if [ "$1" == "cleanAll" ]; then cmd_cmake_clean
    elif [ "$1" == "cmake" ]; then cmd_cmake
    elif [ "$1" == "make" ]; then cmd_make
    elif [ "$1" == "clean" ]; then cmd_clean
    elif [ "$1" == "run" ]
    then
        copyAssets
        cd ./$cmakeDirName
        procErrorCode $?
        ./$exeName
        procErrorCode $?
        cd ..
        procErrorCode $?
        
    else
        printHelp
    fi
}



procArg $1
if [ $errCnt -eq 0 ] && [ "$2" != "" ]
then
    procArg $2
    if [ $errCnt -eq 0 ] && [ "$3" != "" ]
    then
        procArg $3
        if [ $errCnt -eq 0 ] && [ "$4" != "" ]
        then
            procArg $4
            if [ $errCnt -eq 0 ] && [ "$5" != "" ]
            then
                procArg $5
            fi
        fi
    fi
fi



exitCode=0
if [ $errCnt -ne 0 ]
then
    exitCode=1
    ptintTitle "$echoTitle - failed" 1
else
    ptintTitle "$echoTitle - OK" 2
fi

exit $exitCode
