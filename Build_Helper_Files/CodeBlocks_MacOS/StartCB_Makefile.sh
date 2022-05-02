#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CB_EXE_BUILD_ROOT_DIR=../../CodeBlocks.app/Contents/MacOS/

if [ -d "$CB_EXE_BUILD_ROOT_DIR" ]; then
    echo CB root dir: "$PWD/$CB_EXE_BUILD_ROOT_DIR"
    echo CB exe: "$PWD/$CB_EXE_BUILD_ROOT_DIR/codeblocks"

    cd $CB_EXE_BUILD_ROOT_DIR
    ./codeblocks -v
else
    echo ERROR: Cannot find CB root dir: "$PWD/$CB_EXE_BUILD_ROOT_DIR"
fi

