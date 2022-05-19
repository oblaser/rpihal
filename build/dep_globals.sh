#!/bin/bash

# author        Oliver Blaser
# date          17.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

prjName="rpihal"
prjDisplayName="Raspberry Pi Hardware Abstraction Layer"
prjBinName=$prjName
prjDirName=$prjName

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
    echo ""                                                                                              > $1
    echo "                           \$\$                    \$\$\$"                                     >> $1
    echo "                       \$\$  \$\$                    \$\$\$"                                   >> $1
    echo "                       \$\$  \$\$                     \$\$"                                    >> $1
    echo "                           \$\$                     \$\$"                                      >> $1
    echo "\$\$ \$\$\$\$\$   \$\$ \$\$\$\$\$   \$\$\$  \$\$ \$\$\$\$\$     \$\$\$\$\$ \$\$  \$\$"         >> $1
    echo "\$\$\$\$\$\$\$\$\$  \$\$\$\$\$\$\$\$\$  \$\$\$  \$\$\$\$\$\$\$\$\$   \$\$\$\$\$\$\$\$\$  \$\$" >> $1
    echo "\$\$\$    \$\$\$ \$\$\$    \$\$\$  \$\$  \$\$\$    \$\$\$ \$\$\$    \$\$\$  \$\$"              >> $1
    echo "\$\$         \$\$      \$\$  \$\$  \$\$      \$\$ \$\$      \$\$  \$\$"                        >> $1
    echo "\$\$         \$\$      \$\$  \$\$  \$\$      \$\$ \$\$      \$\$  \$\$"                        >> $1
    echo "\$\$         \$\$\$    \$\$\$  \$\$  \$\$      \$\$ \$\$\$    \$\$\$  \$\$"                    >> $1
    echo "\$\$         \$\$\$\$\$\$\$\$\$  \$\$\$\$ \$\$      \$\$  \$\$\$\$\$\$\$\$\$ \$\$\$\$"         >> $1
    echo "\$\$         \$\$  \$\$\$\$   \$\$\$\$ \$\$      \$\$   \$\$\$\$\$ \$\$ \$\$\$\$"              >> $1
    echo "           \$\$"                                                                               >> $1
    echo "           \$\$"                                                                               >> $1
    echo "           \$\$              v$versionstr"                                                     >> $1
    echo ""                                                                                              >> $1
    echo ""                                                                                              >> $1
    echo ""                                                                                              >> $1
    echo "GitHub      https://github.com/oblaser/rpihal/"                                                >> $1
}
