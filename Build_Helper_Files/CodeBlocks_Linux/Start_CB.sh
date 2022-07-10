#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

CurrentDir=${PWD}
# [ -f "Makefile" ] && make clean

if [ ! -f "bootstrap" ]; then
    if [ -f "../bootstrap" ]; then
        cd ..
    else
        if [ -f "../../bootstrap" ]; then
            cd ../..
        else
            echo Could not find bootstrap or ../bootstrap or ../../bootstrap
            cd $CurrentDir
            exit 2
        fi
    fi
fi


if [ -d "${PWD}/src/devel30_64" ]; then
    CB_DEV_DIR=${PWD}/src/devel30_64
fi
if [ -d "${PWD}/src/devel30" ]; then
    CB_DEV_DIR=${PWD}/src/devel30
fi
if [ -d "${PWD}/src/devel31_64" ]; then
    CB_DEV_DIR=${PWD}/src/devel31_64
fi
if [ -d "${PWD}/src/devel31" ]; then
    CB_DEV_DIR=${PWD}/src/devel31
fi
if [ -d "${PWD}/src/devel32_64" ]; then
    CB_DEV_DIR=${PWD}/src/devel32_64
fi
if [ -d "${PWD}/src/devel32" ]; then
    CB_DEV_DIR=${PWD}/src/devel32
fi
if [ ! -d "$CB_DEV_DIR" ]; then
    echo Could not find ${PWD}/src/devel3* directory.
    cd $CurrentDir
    exit 3
fi

if [ -f "${CB_DEV_DIR}/bin/codeblocks" ]; then
    CB_EXE_DIR=${CB_DEV_DIR}/bin
fi    

if [ -f "${CB_DEV_DIR}/codeblocks" ]; then
    CB_EXE_DIR=${CB_DEV_DIR}
fi    

if [ "${CB_EXE_DIR}"  == "" ]; then
    echo Could not find codeblock exe file in the ${CB_DEV_DIR} directory
    cd ${CurrentDir}
    exit 4
fi
echo CodeBlocks exe dir: ${CB_EXE_DIR}

cd ${CB_EXE_DIR}
echo starting: ./codeblocks  -v --prefix ${PWD}
export LD_LIBRARY_PATH=${PWD}
./codeblocks -v --prefix ${PWD}

cd ${CurrentDir}