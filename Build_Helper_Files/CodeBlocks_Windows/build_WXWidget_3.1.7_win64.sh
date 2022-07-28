#!/bin/bash

# --------------------------------------------------------------------------------------#
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
# --------------------------------------------------------------------------------------#
#     This bash script builds wxWidgets monolithinc for Code::Blocks build on :         #
#           - Windows using MSYS2 Mingw64                                               #
# ------------------------------------------------------------------------------------------------------------
# GIT NOTES:
#     git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets github_wxWidget_trunk
#     git clone -b v3.1.7 --single-branch --recurse-submodules https://github.com/wxWidgets/wxWidgets github_wxWidget_3.1.7
#
#     YOU NEED TO EDIT the following files to change the "#define wxUSE_GRAPHICS_DIRECT2D 0" from 0 to 1:
#        include/wx/msw/setup.h
#        lib/gcc_dll/mswu/wx\setup.h
#
# https://forums.wxwidgets.org/viewtopic.php?t=42817
# mingw32-make     -f makefile.gcc BUILD=release CPP="gcc -E -D_M_IX86 -m32" CFLAGS="-m32" CXXFLAGS="-m32 -std=gnu++11" CPPFLAGS="-m32" LDFLAGS="-m32" WINDRES="windres --use-temp-file -F pe-i386" setup_h
# mingw32-make -j4 -f makefile.gcc BUILD=release CPP="gcc -E -D_M_IX86 -m32" CFLAGS="-m32" CXXFLAGS="-m32 -std=gnu++11" CPPFLAGS="-m32" LDFLAGS="-m32" WINDRES="windres --use-temp-file -F pe-i386"
# ------------------------------------------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Set build variables
# ----------------------------------------------------------------------------
#DEBUG_SCRIPT="No"
DEBUG_SCRIPT="Yes"
WXWIDGET_VERSION=3.1.7
WXWIDGET_RELEASE_VER=31
BUILD_BITS=64
BUILD_TYPE=release
BUILD_FLAGS="SHARED=1 MONOLITHIC=1 BUILD=${BUILD_TYPE} UNICODE=1 VENDOR=cb CXXFLAGS=-std=gnu++17"

# save for later use
StartDir=${PWD}

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i wxWidget_${WX_VERSION}_$(uname)_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# ------------------------------------------------------------------------------------------------------------
# Detect and process OS info
# ------------------------------------------------------------------------------------------------------------
case "$(uname)" in
  Darwin*)
    OSDetected="OSX"
    ;;
  Linux*)
    OSDetected="Linux"
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    OSDetected="Windows"
    ;;

  AIX*)
    echo "AIX is not supported"
    cd ${StartDir}
    exit 1
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${StartDir}
    exit 1
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${StartDir}
    exit 1
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd ${StartDir}
    exit 1
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd ${StartDir}
    exit 1
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd ${StartDir}
    exit 1
    ;;
  *)
    echo "unknown: ${OSTYPE} is not supported"
    cd ${StartDir}
    exit 1
    ;;
esac

# -------------------------------------------------------------------------------------------------

echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                Code::Blocks build script running on " $(uname) "                     |"

if [ "${GITHUB_ACTIONS}" == "true" ] ; then
    # D:\a\CodeBlocks_Unofficial_Testing\CodeBlocks_Unofficial_Testing\Libraries\wxWidgets-${WXWIDGET_VERSION}_win64'    
    WX_CB_BUILD_DIR=$(cygpath -u ${GITHUB_WORKSPACE})/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
    echo "|      You are running under GITHUB ACTIONS.                              |"

    if [ "${NUMBER_OF_PROCESSORS}" ==  2 ] ; then
        echo "|       Setting BUILD_MAKE_CONCURENCY=-j1  as NUMBER_OF_PROCESSORS ==2    |"
        BUILD_MAKE_CONCURENCY=-j1
    else
        # using self-hosted local build
        echo "|       Local self hosted build, so setting BUILD_MAKE_CONCURENCY=-j      |"
        BUILD_MAKE_CONCURENCY=-j
    fi
else
    BUILD_MAKE_CONCURENCY=-j
    echo "|       Non GITHUB ACTIONS build, so setting BUILD_MAKE_CONCURENCY=-j     |"
fi

echo "|                               Windows $(expr substr $(uname -s) 12 4)                                                      |"
echo "+-------------------------------------------------------------------------------------------------+"
if [ ! -d "${WX_CB_BUILD_DIR}" ]; then
    echo unset ${WX_CB_BUILD_DIR}
    unset WX_CB_BUILD_DIR
fi

if [ "${WX_CB_BUILD_DIR}" == "" ]; then
    WX_DIR_FIND=Libraries/github_wxWidget_${WXWIDGET_VERSION}
    if [ -d "${PWD}/../${WX_DIR_FIND}" ]; then
        WX_CB_BUILD_DIR=${PWD}/../${WX_DIR_FIND}
    else
        if [ -d "${PWD}/../../${WX_DIR_FIND}" ]; then
            WX_CB_BUILD_DIR=${PWD}/../../${WX_DIR_FIND}
        else
            if [ -d "${PWD}/../../../${WX_DIR_FIND}" ]; then
                WX_CB_BUILD_DIR=${PWD}/../../../${WX_DIR_FIND}
            else
                if [ -d "${PWD}/../../../../${WX_DIR_FIND}" ]; then
                    WX_CB_BUILD_DIR=${PWD}/../../../../${WX_DIR_FIND}
                else
                    if [ -d "${PWD}/../../../../../${WX_DIR_FIND}" ]; then
                        WX_CB_BUILD_DIR=${PWD}/../../../../../${WX_DIR_FIND}
                    fi
                fi
            fi
        fi
    fi
fi

if [ "${WX_CB_BUILD_DIR}" == "" ]; then
    WX_DIR_FIND=Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
    if [ -d "${PWD}/../${WX_DIR_FIND}" ]; then
        WX_CB_BUILD_DIR=${PWD}/../${WX_DIR_FIND}
    else
        if [ -d "${PWD}/../../${WX_DIR_FIND}" ]; then
            WX_CB_BUILD_DIR=${PWD}/../../${WX_DIR_FIND}
        else
            if [ -d "${PWD}/../../../${WX_DIR_FIND}" ]; then
                WX_CB_BUILD_DIR=${PWD}/../../../${WX_DIR_FIND}
            else
                if [ -d "${PWD}/../../../../${WX_DIR_FIND}" ]; then
                    WX_CB_BUILD_DIR=${PWD}/../../../../${WX_DIR_FIND}
                else
                    if [ -d "${PWD}/../../../../../${WX_DIR_FIND}" ]; then
                        WX_CB_BUILD_DIR=${PWD}/../../../../../${WX_DIR_FIND}
                    fi
                fi
            fi
        fi
    fi
fi

echo "+-------------------------------------------------------------------------------------------------+"

# -------------------------------------------------------------------------------------------------
# Check wxWidget directory exists
# -------------------------------------------------------------------------------------------------
if [ ! -d "$WX_CB_BUILD_DIR" ]; then
    echo "|                                                                                                 |"
    echo "|              +-----------------------------------------------------------------+                |"
    echo "|              | Error: NO WX_CB_BUILD_DIR environment variable set!             |                |"
    echo "|              |        Please export WX_CB_BUILD_DIR environment and try again. |                |"
    echo "|              |        WX_CB_BUILD_DIR is wxWidget src root dir                 |                |"
    echo "|              +-----------------------------------------------------------------+                |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd ${StartDir}
    exit 2
fi
export WX_CB_BUILD_DIR=$(realpath ${WX_CB_BUILD_DIR})

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    # ------------------------------------------------------------------------------------------------------------
    # Setup ANSI color control variables
    # ------------------------------------------------------------------------------------------------------------
    ECHO_E="-e"
    ECHO_N="-e"
    CR="\\r"
    ERASE_LINE="\033[2K"
    CHECK_MARK="\xE2\x9C\x94"
    CROSS_MARK="\xE2\x9C\x98"
    # 0 reset, 1 bold, 32 green
    GREEN_START="\033[0;1;32m"
    # 0 reset, 1 bold, 5 blink slow, 31 red
    RED_START="\033[0;1;5;31m"
    COLOR_REVERT="\033[0m"
fi


case "$OSDetected" in
  OSX* | Linux*)
    #unset WX_CB_BUILD_DIR
    #prefixDir=$PWD/src/devel31
    #configOptions="--prefix=$prefixDir --with-contrib-plugins=all"
    echo "Currently Linux is not supported - future work to be done. OS detected: $(uname)"
    cd ${StartDir}
    exit 4
    ;;
  Windows*)
    ;;
  *)
    echo "OSDetected not detected for $(uname)"
    cd ${StartDir}
    exit 5
    ;;
esac

# ----------------------------------------------------------------------------
# Detect GCC directory 
# ----------------------------------------------------------------------------
GCC_ROOT=$(dirname $(which g++))
if [ ! -d "${GCC_ROOT}" ]; then
    echo "|                                                                                                 |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                    |     Error: NO GCC found     |                              |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                                                                                 |"
    cd ${StartDir}
    exit 6
fi

# ------------------------------------------------------------------------------------------------------------
# Update Path to include GCC root folder and "bin" subfolder
# ------------------------------------------------------------------------------------------------------------
PATH=${GCC_ROOT}:${PATH}
if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
    echo PATH=${PATH}
fi

echo
echo "+=================================================================================================+"
echo "|                  WxWidgets ${WXWIDGET_VERSION}   win${BUILD_BITS}  build script                                          |"
echo "+=================================================================================================+"
echo "|                                                                                                 |"
echo "| GCC_ROOT:        ${GCC_ROOT}                                                                   |"
echo "| WX_CB_BUILD_DIR: ${WX_CB_BUILD_DIR}                          |"
echo "| BUILD_BITS:      ${BUILD_BITS}                                                                             |"
echo "| BUILD_TYPE:      ${BUILD_TYPE}                                                                        |"
echo "| BUILD_FLAGS:     ${BUILD_FLAGS}                          |"
echo "| PWD:             ${PWD} |"
echo "|                                                                                                 |"
echo "+=================================================================================================+"
echo

# ------------------------------------------------------------------------------------------------------------
# Start building
# ------------------------------------------------------------------------------------------------------------
start_datetime=$(date +%s)
failureDetected="no"

cd ${WX_CB_BUILD_DIR}/build/MSW

echo "+-------------------------------------------------------------------------------------------------+"
echo "|  Start at : "$(date +"%d-%b-%Y %T") "                                                               |"
echo "+-------------------------------------------------------------------------------------------------+"

PreviousDir=${PWD}
echo "|  Update wxWidget setup.h #define wxUSE_GRAPHICS_DIRECT2D from 0 to 1                            |"
#Update the following files to change the "#define wxUSE_GRAPHICS_DIRECT2D 0" from 0 to 1:
#        include\wx\msw\setup.h
#        lib\gcc_dll\mswu\wx\setup.h

if [ -d "${WX_CB_BUILD_DIR}/include/wx/msw" ]; then
    cd ${WX_CB_BUILD_DIR}/include/wx/msw
    if [ -f setup.h ] ; then
        echo "| sed update File: ${PWD}/setup.h |"
        sed -i 's/#define wxUSE_GRAPHICS_DIRECT2D 0/#define wxUSE_GRAPHICS_DIRECT2D 1/g' setup.h
    fi 
fi 

if [ -d "${WX_CB_BUILD_DIR}/lib/gcc_dll/mswu/wx" ]; then
    cd ${WX_CB_BUILD_DIR}/lib/gcc_dll/mswu/wx
    if [ -f setup.h ] ; then
        echo "| sed update File: ${PWD}/setup.h |"
        sed -i 's/#define wxUSE_GRAPHICS_DIRECT2D 0/#define wxUSE_GRAPHICS_DIRECT2D 1/g' setup.h
    fi 
fi
echo "+-------------------------------------------------------------------------------------------------+"
cd ${PreviousDir}

echo
COMMAND="${GCC_ROOT}/mingw32-make ${BUILD_MAKE_CONCURENCY} -f makefile.gcc ${BUILD_FLAGS} SHELL=cmd.exe clean"
if [ "${DEBUG_SCRIPT}" == "Yes" ] ; then
    echo "+-------------------------------------------------------------------------------------------------+"
    echo "|  BUILD STAGE:    ${BUILD_TYPE} clean                                                                  |"
    echo "|  ${COMMAND}                             |"
    echo "+-------------------------------------------------------------------------------------------------+"
fi
echo "PATH: ${PATH}"
echo "|  BUILD STAGE:    ${BUILD_TYPE} clean                                                                  |"
${COMMAND}
status=$?
if [ $status == 0 ] ; then
    echo
    echo "+-------------------------------------------------------------------------------------------------+"
    echo "|  BUILD STAGE:    ${BUILD_TYPE} setup.h                                                                |"
    echo "|  mingw32-make ${BUILD_MAKE_CONCURENCY} -f makefile.gcc ${BUILD_FLAGS} SHELL=cmd.exe setup_h                              |"
    echo "+-------------------------------------------------------------------------------------------------+"
    if [ ! -d ${WX_CB_BUILD_DIR}/lib/gcc_dll/mswu/wx ] ; then 
        # Work arround for https://github.com/wxWidgets/wxWidgets/issues/22435
        mkdir -p ${WX_CB_BUILD_DIR}/lib/gcc_dll/mswu/wx
    fi
    mingw32-make ${BUILD_MAKE_CONCURENCY} -f makefile.gcc ${BUILD_FLAGS} SHELL=cmd.exe setup_h
    status=$?
    if [ $status == 0 ] ; then
        echo
        echo "+-------------------------------------------------------------------------------------------------+"
        echo "|  BUILD STAGE:    ${BUILD_TYPE} make                                                                   |"
        echo "|  mingw32-make ${BUILD_MAKE_CONCURENCY} -f makefile.gcc ${BUILD_FLAGS} SHELL=cmd.exe                                   |"
        echo "+-------------------------------------------------------------------------------------------------+"
        mingw32-make ${BUILD_MAKE_CONCURENCY} -f makefile.gcc ${BUILD_FLAGS} SHELL=cmd.exe
        status=$?
        if [ $status == 0 ] ; then
            if [ "${BUILD_TYPE}" == "release" ] ; then
                DLL_COUNT=$(ls -1q ${WX_CB_BUILD_DIR}/lib/gcc_dll/wxmsw${WXWIDGET_RELEASE_VER}u_*gcc_cb.dll | wc -l)
            else
                DLL_COUNT=$(ls -1q ${WX_CB_BUILD_DIR}/lib/gcc_dll/wxmsw${WXWIDGET_RELEASE_VER}ud_*gcc_cb.dll | wc -l)
            fi
            if [ ${DLL_COUNT} -eq 2 ] ; then
                echo "CB wxWidget DLL count: ${DLL_COUNT} (good)"
                failureDetected="no"
            else
                echo ${ECHO_E} "CB wxWidget detected DLL count ${DLL_COUNT} should have been 2!!! See ${WX_CB_BUILD_DIR}/lib/gcc_dll/wxmsw%{WXWIDGET_RELEASE_VER}u*_*gcc_cb.dll"
                ls -la ${WX_CB_BUILD_DIR}/lib/gcc_dll/wxmsw${WXWIDGET_RELEASE_VER}u*_*gcc_cb.dll
                failureDetected="yes"
            fi
        else
            echo ${ECHO_E} "${ERASE_LINE}${RED_START}| ${CROSS_MARK}   mingw32-make wxWidgets failed   mingw32-make wxWidgets failed   mingw32-make wxWidgets failed  |"
            failureDetected="yes"
        fi
    else
        echo ${ECHO_E} "${ERASE_LINE}${RED_START}| ${CROSS_MARK}    mingw32-make setup_h failed    mingw32-make setup_h failed    mingw32-make setup_h failed  |"
        failureDetected="yes"
    fi
else
    echo ${ECHO_E} "${ERASE_LINE}${RED_START}| ${CROSS_MARK}    mingw32-make clean failed      mingw32-make clean failed      mingw32-make clean failed    |"
    failureDetected="yes"
fi
echo ${ECHO_E} "${COLOR_REVERT}+-------------------------------------------------------------------------------------------------+"

# ------------------------------------------------------------------------------------------------------------
# Build finished - pass or fail
# ------------------------------------------------------------------------------------------------------------

durationTotalTime=$(($(date +%s)-start_datetime))
echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                                                                                                 |"
echo "| Finished at  : "$(date +"%d-%b-%Y %T")" ,  duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")"                                      |"
echo "|                                                                                                 |"
echo "+-------------------------------------------------------------------------------------------------+"
echo

cd ${StartDir}

if test "x$failureDetected" = "xyes" ; then
    echo
    echo ${ECHO_E} "${CR}${RED_START}***************************************************************************************************"
    echo ${ECHO_E} "${CR}${RED_START}*                                                                                                 *"
    echo ${ECHO_E} "${CR}${RED_START}*  ######   #     #  #####  #      ######       #######     #     #####  #      #######  ######   *"
    echo ${ECHO_E} "${CR}${RED_START}*  #     #  #     #    #    #      #     #      #          # #      #    #      #        #     #  *"
    echo ${ECHO_E} "${CR}${RED_START}*  #     #  #     #    #    #      #     #      #         #   #     #    #      #        #     #  *"
    echo ${ECHO_E} "${CR}${RED_START}*  ######   #     #    #    #      #     #      #####    #     #    #    #      #####    #     #  *"
    echo ${ECHO_E} "${CR}${RED_START}*  #     #  #     #    #    #      #     #      #        #######    #    #      #        #     #  *"
    echo ${ECHO_E} "${CR}${RED_START}*  #     #  #     #    #    #      #     #      #        #     #    #    #      #        #     #  *"
    echo ${ECHO_E} "${CR}${RED_START}*  ######    #####   #####  #####  ######       #        #     #  #####  #####  #######  ######   *"
    echo ${ECHO_E} "${CR}${RED_START}*                                                                                                 *"
    echo ${ECHO_E} "${CR}${RED_START}***************************************************************************************************"
    echo ${ECHO_E} "${CR}${COLOR_REVERT} "
    exit 7
else
    echo
    echo ${ECHO_E} "${CR}${GREEN_START}+--------------------------------------------------------------------------------------+"
    echo ${ECHO_E} "${CR}${GREEN_START}|                                                                                      |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               *******           **           ********       ********                 |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               **    **         ****         **             **                        |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               **    **        **  **        **             **                        |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               *******        **    **       *********      *********                 |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               **            **********             **             **                 |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               **            **      **             **             **                 |"
    echo ${ECHO_E} "${CR}${GREEN_START}|               **            **      **      ********       ********                  |"
    echo ${ECHO_E} "${CR}${GREEN_START}|                                                                                      |"
    echo ${ECHO_E} "${CR}${GREEN_START}+--------------------------------------------------------------------------------------+"
    echo ${ECHO_E} "${CR}${COLOR_REVERT} "
    exit 0
fi
# -----------------------------------------------------------------

