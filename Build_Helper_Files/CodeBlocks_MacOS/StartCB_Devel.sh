#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Check where we are running from and go to the C::B source root directory 
# ----------------------------------------------------------------------------

if [ ! -f "bootstrap" ]; then
    if [ -f "../bootstrap" ]; then
        cd ..
    else
        if [ -f "../../bootstrap" ]; then
            cd ../..
        else
            if [ -f "../../../bootstrap" ]; then
                cd ../../..
            else
                echo Could not find bootstrap or ../bootstrap or ../../bootstrap or ../../../bootstrap
                cd ${InitialDir}
                exit 3
            fi
        fi
    fi
fi

echo "|    CurrentDir C::B root is:            ${PWD}                                                  |"

CB_ROOT=$PWD
CB_SRC=${CB_ROOT}/src

# ----------------------------------------------------------------------------
# Check BUILD_BITS for validity
# ----------------------------------------------------------------------------

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel30 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel30_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel30_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel31_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel31_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel31_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel31_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel32_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel32_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel32_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel32_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

if [ "${CB_DEVEL_DIR}" == "" ] ; then
    echo "+-------------------------------------------------------------------------------------------------+"
    echo "|                                                                                                 |"
    echo "|        +-------------------------------------------------------------+                          |"
    echo "|        | Error: Cannot find src\devel3x  directory, where x in 0,1,2 |                          |"
    echo "|        |        Please fix and try again                             |                          |"
    echo "|        +-------------------------------------------------------------+                          |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    echo BUILD_BITS:${BUILD_BITS}
    cd ${InitialDir}
    exit 4
fi

if [ -d "${CB_DEVEL_DIR}/bin" ]; then
    echo "ERROR: You built using the makefile process. Run the \'StartCB_Makefile.sh\' shell script instead!!!"
else

    if [ -d "${CB_DEVEL_DIR}" ]; then
        echo CB root dir: "${PWD}/${CB_DEVEL_DIR}"
        echo CB exe: "${PWD}/${CB_DEVEL_DIR}/CodeBlocks"
    
        cd ${CB_DEVEL_DIR}
        echo "Running: ./CodeBlocks --verbose --debug-log --multiple-instance --personality=debuging --prefix=${PWD}"
        ./CodeBlocks --verbose --debug-log --multiple-instance --personality=debuging --prefix=${PWD}
    else
        echo ERROR: Cannot find CB root dir: "${CB_DEVEL_DIR}"
    fi
fi

