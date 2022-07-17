#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CB_EXE_BUILD_ROOT_DIR=../../src/devel32_64

if [ ! -d "${CB_EXE_BUILD_ROOT_DIR}" ]; then
    CB_EXE_BUILD_ROOT_DIR=../../src/devel32
fi

if [ -d "${CB_EXE_BUILD_ROOT_DIR}/bin" ]; then
    echo "ERROR: You built using the makefile process. Run the \'StartCB_Makefile.sh\' shell script instead!!!"
else

    if [ -d "${CB_EXE_BUILD_ROOT_DIR}" ]; then
        echo CB root dir: "${PWD}/${CB_EXE_BUILD_ROOT_DIR}"
        echo CB exe: "${PWD}/${CB_EXE_BUILD_ROOT_DIR}/CodeBlocks"
    
        cd ${CB_EXE_BUILD_ROOT_DIR}
        echo "Running: ./CodeBlocks --verbose --debug-log --multiple-instance --personality=debuging --prefix=${PWD}"
        ./CodeBlocks --verbose --debug-log --multiple-instance --personality=debuging --prefix=${PWD}
    else
        echo ERROR: Cannot find CB root dir: "${PWD}/${CB_EXE_BUILD_ROOT_DIR}"
    fi
fi

