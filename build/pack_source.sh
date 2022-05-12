#!/bin/bash

# author        Oliver Blaser
# date          12.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser

source dep_globals.sh

packedDir="./packed"
outDirName="${prjDirName}_src"
outDir="$packedDir/$outDirName"
archive="$packedDir/${prjDirName}_src_v$versionstr.tar.gz"



rm -rf $outDir

mkdir -p $outDir/$prjDirName
mkdir $outDir/$prjDirName/include
mkdir $outDir/$prjDirName/src

cp -r ../include/* $outDir/$prjDirName/include
cp -r ../src/* $outDir/$prjDirName/src

writeReadmeFile $outDir/$prjDirName/readme.txt
cp ../license.txt $outDir/$prjDirName

rm -f $archive
tar -czf $archive -C $outDir $prjDirName
