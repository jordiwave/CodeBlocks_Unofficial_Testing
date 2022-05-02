#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CB_EXE_BUILD_ROOT_DIR=../../src/devel31

if [ -d "$CB_EXE_BUILD_ROOT_DIR" ]; then
    echo CB root dir: "$PWD/$CB_EXE_BUILD_ROOT_DIR"
    echo CB exe: "$PWD/$CB_EXE_BUILD_ROOT_DIR/bin/codeblockscd .."

    export LD_LIBRARY_PATH=$PWD/$CB_EXE_BUILD_ROOT_DIR 
    $PWD/$CB_EXE_BUILD_ROOT_DIR/bin/codeblocks
else
    echo ERROR: Cannot find CB root dir: "$PWD/$CB_EXE_BUILD_ROOT_DIR"
fi

