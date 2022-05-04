#!/bin/bash

# author        Oliver Blaser
# date          04.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

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
