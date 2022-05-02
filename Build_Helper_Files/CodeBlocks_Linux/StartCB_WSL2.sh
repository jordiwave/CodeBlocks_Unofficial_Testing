#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CB_ROOT_DIR=/mnt/d/Andrew_Development/Work_Installers/CodeBLocks_Private_Experimental/src/devel31_64

# echo CB root dir: "$CB_ROOT_DIR"
if [ -d "$CB_ROOT_DIR" ]; then
    echo CB root dir: "$CB_ROOT_DIR"

    export LD_LIBRARY_PATH=/mnt/d/Andrew_Development/Work_Installers/CodeBLocks_Private_Experimental/src/devel31_64
    cd $CB_ROOT_DIR
    $CB_ROOT_DIR/codeblocks
fi
