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


if [ -d "${PWD}/src/devel30" ]; then
    CB_DEV_DIR=${PWD}/src/devel30
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
    CB_EXE_NAME=codeblocks
    CB_PREFIX=${CB_DEV_DIR}
fi    

if [ -f "${CB_DEV_DIR}/codeblocks" ]; then
    CB_EXE_DIR=${CB_DEV_DIR}
    CB_EXE_NAME=codeblocks
    CB_PREFIX=${CB_DEV_DIR}
fi    

if [ -f "${CB_DEV_DIR}/CodeBlocks" ]; then
    CB_EXE_DIR=${CB_DEV_DIR}
    CB_EXE_NAME=CodeBlocks
    CB_PREFIX=${CB_DEV_DIR}
fi 

CB_ROOT_DIR=${PWD}
echo CB_DEV_DIR: ${CB_DEV_DIR}
echo CB_EXE_DIR: ${CB_EXE_DIR}
echo CB_EXE_NAME: ${CB_EXE_NAME}
echo CB_PREFIX: ${CB_PREFIX}

if [ "${CB_EXE_DIR}"  == "" ]; then
    echo Could not find codeblock exe file in the ${CB_DEV_DIR} directory
    cd ${CurrentDir}
    exit 4
fi

cd ${CB_EXE_DIR}
#CB_EXE_PARMS="--verbose --debug-log --application-log --multiple-instance --personality=debuging --prefix=${CB_PREFIX}"
#CB_EXE_PARMS=--prefix=${PWD}/share/${CB_EXE_NAME}
CB_EXE_PARMS="--verbose --debug-log --multiple-instance --personality=debuging --prefix=${CB_PREFIX}"

echo "export LD_LIBRARY_PATH=${CB_EXE_DIR}"
echo "Runing: ./${CB_EXE_NAME} ${CB_EXE_PARMS}"

export LD_LIBRARY_PATH=${CB_EXE_DIR}
./${CB_EXE_NAME} ${CB_EXE_PARMS}

cd ${CurrentDir}
