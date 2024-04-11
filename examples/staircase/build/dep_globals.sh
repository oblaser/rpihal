#!/bin/bash

# author        Oliver Blaser
# date          10.03.2024
# copyright     MIT - Copyright (c) 2024 Oliver Blaser

prjName="rpihal-example-staircase"
prjDisplayName="rpihal example staircase"
prjBinName=$prjName
prjDirName=$prjName
repoDirName="staircase"
copyrightYear="2024"

versionstr=$(head -n 1 dep_vstr.txt)

function ptintTitle()
{
    if [ "$2" = "" ]
    then
        echo "  --=====#   $1   #=====--"
    else
        echo -e "\033[3$2m  --=====#   \033[9$2m$1\033[3$2m   #=====--\033[39m"
    fi
}

# pass output filename as argument
function writeReadmeFile()
{
    echo ""                                 > $1
    echo "${prjDisplayName} v${versionstr}" >> $1
    echo ""                                 >> $1
    echo "MIT - Copyright (c) ${copyrightYear} Oliver Blaser" >> $1
    echo ""                                 >> $1
    echo "This is just an example project, see https://github.com/oblaser/rpihal/tree/main/examples/staircase" >> $1
}
