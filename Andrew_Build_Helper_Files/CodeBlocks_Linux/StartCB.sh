#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CB_ROOT_DIR=~/code/codeblocks/gtk3-unicode-3.0/install

# echo CB root dir: "$CB_ROOT_DIR"
if [ -d "$CB_ROOT_DIR" ]; then
    echo CB root dir: "$CB_ROOT_DIR"

    export LD_LIBRARY_PATH=$CB_ROOT_DIR/bin 
    $CB_ROOT_DIR/bin/codeblocks
fi
