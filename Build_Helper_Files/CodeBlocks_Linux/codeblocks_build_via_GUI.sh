#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#
#                                                                                       #
#     This bash script builds CodeBlocks via the CodeBLocks GUI on either :             #
#           - Windows using MSYS2 Mingw64                                               #
#           - Future - Linux                                                            #
#                                                                                       #
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Set build variables
# ----------------------------------------------------------------------------
CurrentDir=${PWD}
# MUST configure soft link for wx-config
#WXWIDGET_VERSION=3.2.1
#WXWIDGET_DIR=32
#WXWIDGET_VERSION=3.1.7
#WXWIDGET_DIR=31
WXWIDGET_VERSION=3.0.5
WXWIDGET_DIR=30
# MUST configure soft link for wx-config
failureDetected="no"

# --------------------------------------------------------------------------------------#

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i CodeBlocks_${WXWIDGET_VERSION}_$(uname)_GUI_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# --------------------------------------------------------------------------------------#

CHECK_MARK="\033[2K\033[0;32m\xE2\x9C\x94\033[0m"
CROSS_MARK="\033[2K\033[0;31m\xE2\x9C\x98\033[0m"
# 0 reset, 1 bold, 32 green
GREEN_START="\033[0;1;32m"
# 0 reset, 1 bold, 5 flash slow, 31 red
RED_START="\033[0;1;5;31m"
COLOR_REVERT="\033[0m"

if [ "$(id -u)" == "0" ]; then
    echo -e "\\r${RED_START}You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

function display_build_failed()
{
    echo
    echo -e "\\r${RED_START}***************************************************************************************************"
    echo -e "\\r${RED_START}*                                                                                                 *"
    echo -e "\\r${RED_START}*  ######   #     #  #####  #      ######       #######     #     #####  #      #######  ######   *"
    echo -e "\\r${RED_START}*  #     #  #     #    #    #      #     #      #          # #      #    #      #        #     #  *"
    echo -e "\\r${RED_START}*  #     #  #     #    #    #      #     #      #         #   #     #    #      #        #     #  *"
    echo -e "\\r${RED_START}*  ######   #     #    #    #      #     #      #####    #     #    #    #      #####    #     #  *"
    echo -e "\\r${RED_START}*  #     #  #     #    #    #      #     #      #        #######    #    #      #        #     #  *"
    echo -e "\\r${RED_START}*  #     #  #     #    #    #      #     #      #        #     #    #    #      #        #     #  *"
    echo -e "\\r${RED_START}*  ######    #####   #####  #####  ######       #        #     #  #####  #####  #######  ######   *"
    echo -e "\\r${RED_START}*                                                                                                 *"
    echo -e "\\r${RED_START}***************************************************************************************************"
    echo -e "\\r${COLOR_REVERT}"
}
function display_build_pass()
{
    echo
    echo -e "\\r${GREEN_START}+--------------------------------------------------------------------------------------+"
    echo -e "\\r${GREEN_START}|                                                                                      |"
    echo -e "\\r${GREEN_START}|               *******           **           ********       ********                 |"
    echo -e "\\r${GREEN_START}|               **    **         ****         **             **                        |"
    echo -e "\\r${GREEN_START}|               **    **        **  **        **             **                        |"
    echo -e "\\r${GREEN_START}|               *******        **    **       *********      *********                 |"
    echo -e "\\r${GREEN_START}|               **            **********             **             **                 |"
    echo -e "\\r${GREEN_START}|               **            **      **             **             **                 |"
    echo -e "\\r${GREEN_START}|               **            **      **      ********       ********                  |"
    echo -e "\\r${GREEN_START}|                                                                                      |"
    echo -e "\\r${GREEN_START}+--------------------------------------------------------------------------------------+"
    echo -e "\\r${COLOR_REVERT}"
    echo
}
# -----------------------------------------------------------------------------
StartExecutionDir=${PWD}

if [ ! -f "bootstrap" ]; then
    if [ -f "../bootstrap" ]; then
        cd ..
    else
        if [ -f "../../bootstrap" ]; then
            cd ../..
        else
            echo -e "\\r${RED_START}Could not find bootstrap or ../bootstrap or ../../bootstrap"
            echo -e "\\r${COLOR_REVERT}"
            cd ${StartExecutionDir}
            exit 2
        fi
    fi
fi
CB_ROOT_DIR=${PWD}

# -----------------------------------------------------------------------------

case "$(uname)" in
  Darwin*)
    CB_WORKSPACE_FILENAME=CodeBlocks_Unix_MacOS.workspace
    CB_VARIABLE_SET=cb_mac64
    BUILD_BITS=64
    EXEEXT=""
    LIBEXT="dylib"
    OSDetected="OSX"
    ;;
  Linux*)
    CB_WORKSPACE_FILENAME=CodeBlocks_Unix_MacOS.workspace
    CB_VARIABLE_SET=cb_linux64
    BUILD_BITS=64
    EXEEXT=""
    LIBEXT="so"
    OSDetected="Linux"
    ;;
  MINGW*)
    CB_WORKSPACE_FILENAME=CodeBlocks_Windows.workspacee
    EXEEXT=".exe"
    LIBEXT="dll" 
    OSDetected="Windows"
    if [ "${MSYSTEM}" == "MINGW32" ] ; then 
        CB_VARIABLE_SET=cb_win32
        BUILD_BITS=32
    fi
    if [ "${MSYSTEM}" == "MINGW64" ] ; then
        CB_VARIABLE_SET=cb_win64
        BUILD_BITS=64
    fi
    if [ "x${BUILD_BITS}" == "x" ]; then 
        echo -e "\\r${RED_START}Unknown MSYSTEM found: $MSYSTEM"
        echo -e "\\r${COLOR_REVERT}"
        cd ${StartExecutionDir}
        exit 1
    fi
    ;;
  msys* | cygwin* | WindowsNT)
    echo -e "\\r${RED_START}Only MINGW32 or MINGW64 is supported on Windows"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  AIX*)
    echo -e "\\r${RED_START}AIX is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  bsd*)
    echo -e "\\r${RED_START}BSD is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  bsd*)
    echo -e "\\r${RED_START}BSD is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  FreeBSD*)
    echo -e "\\r${RED_START}FreeBSD is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  solaris*)
    echo -e "\\r${RED_START}SOLARIS is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  SunOS*)
    echo -e "\\r${RED_START}SunOS is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
  *)
    echo -e "\\r${RED_START}unknown: $(uname) is not supported"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 0
    ;;
esac


if [ -d "src" ]; then
    cd src
    CB_SRC_DIR=${PWD}
else
    echo -e "\\r${RED_START}Could not find the C::B src directory!!!"
    echo -e "\\r${COLOR_REVERT}"
    cd ${StartExecutionDir}
    exit 1
fi

#rem ----------------------------------------------------------------------------
#rem Setup C::B root folder for C::B *binaries* (!)
#rem ----------------------------------------------------------------------------
if [ "${OSDetected}" == "Windows" ]; then
    if [ "x${CB_PROG_FILE_DIR}" == "x" ]; then 
        if [ -f "/c/Program Files/CodeBlocks_Experimental/codeblocks.exe" ]; then 
            CB_PROG_FILE_DIR="/c/Program Files/CodeBlocks_Experimental"
        else
            if [ -f "/c/Program Files/CodeBlocks/codeblocks.exe" ]; then 
                CB_PROG_FILE_DIR="/c/Program Files/CodeBlocks"
            else
                if [ -f "/c/Program Files (x86)/CodeBlocks/codeblocks.exe" ]; then 
                    CB_PROG_FILE_DIR="/c/Program Files (x86)/CodeBlocks"
                fi
            fi
        fi
    fi

    if [ ! -f "${CB_PROG_FILE_DIR}/codeblocks.exe" ]; then 
        echo
        echo
        echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
        echo -e "\\r${RED_START}|     Error: Could not find ${CB_PROG_FILE_DIR}/codeblocks.exe.   |"
        echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
        echo -e "\\r${COLOR_REVERT}"
        echo
        cd ${StartExecutionDir}
        exit 2
    fi
else
    CB_PROG_FILE_DIR="/c/Program Files/CodeBlocks"
fi

if [ ! -f "${CB_SRC_DIR}/${CB_WORKSPACE_FILENAME}" ]; then
    echo
    echo
    echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
    echo -e "\\r${RED_START}|     Error: Could not find the project file: ${CB_SRC_DIR}/${CB_WORKSPACE_FILENAME}   |"
    echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
    echo -e "\\r${COLOR_REVERT}"
    echo
    cd ${StartExecutionDir}
    exit 3
fi

if [ "${OSDetected}" == "Windows" ]; then
    if [ ! -d "${WX_CB_BUILD_DIR}" ]; then
        echo
        echo
        echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
        echo -e "\\r${RED_START}|     Error: Could not find the wxWidget directory: ${WX_CB_BUILD_DIR}|"
        echo -e "\\r${RED_START}+--------------------------------------------------------------------------------------+"
        echo -e "\\r${COLOR_REVERT}"
        echo
        cd ${StartExecutionDir}
        exit 4
    fi
fi

# -----------------------------------------------------------------------------
reset
echo
echo +"============================================================================================+"
echo "|                                                                                            |"
echo "|    Code::Blocks GUI build script for $(uname) x${BUILD_BITS} and wxWidgets ${WXWIDGET_VERSION}         |"
echo "|                                                                                            |"
echo "+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +"
echo "|                                                                                            |"
echo "|  Code::Blocks GUI build script running on: ${OSDetected}                           |"
echo "|                                                                                            |"
if [ "${OSDetected}" == "Windows" ]; then
    echo "|  Code::Block dir: ${CB_PROG_FILE_DIR}                                 |"
    echo "|                                                                                            |"
fi    
echo "|  CB Build root : ${CB_ROOT_DIR}     |"
echo "|  Build dir : ${CB_SRC_DIR}     |"
echo "|                                                                                            |"
if [ "${OSDetected}" == "Windows" ]; then
    echo "|  WxWidgets dir: ${WX_CB_BUILD_DIR}                      |"
    echo "|                                                                                            |"
fi    
echo "|  Current dir : ${PWD}    |"
echo "|                                                                                            |"
echo "+============================================================================================+"
echo
echo

# -----------------------------------------------------------------------------

#rem --------------------------------------------------------------------------
#rem Ask the user if they want to do a quick clean of previous build directories
#rem --------------------------------------------------------------------------
OBJS_DIR_COUNT=$(ls -1q ${CB_SRC_DIR}/.objsl3* 2>/dev/null | wc -l 2>/dev/null)
DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC_DIR}/devel3* 2>/dev/null | wc -l 2>/dev/null)
OUTPUT_DIR_COUNT=$(ls -1q ${CB_SRC_DIR}/output3* 2>/dev/null | wc -l 2>/dev/null)
if  [ ${OBJS_DIR_COUNT}   -gt 0 ] || 
    [ ${DEVEL_DIR_COUNT}  -gt 0 ] || 
    [ ${OUTPUT_DIR_COUNT} -gt 0 ]  ; then
    read -r -p 'Do you want to delete the previous build directories [Y/N]?'
    if [ "${REPLY}" == "y" ] || [ "${REPLY}" == "Y" ]; then
        TMP_DIR=${PWD}
        cd ${CurrentDir}
        echo source ./codeblocks_cleanup.sh
        source ./codeblocks_cleanup.sh
        cd ${TMP_DIR}
        unset TMP_DIR
    fi
fi  

#rem ----------------------------------------------------------------
#rem Ask the user if they want to run the update.bat after the build
#rem ----------------------------------------------------------------
read -r -p 'Do you want to run the codeblocks_update_devel.sh after the compilation finishes [Y/N]?'
if [ "${REPLY}" == "y" ] || [ "${REPLY}" == "Y" ]; then
    CB_FINISH_RUN_UPDATE_DEV="Yes"
else
    CB_FINISH_RUN_UPDATE_DEV="No"
fi  

# -----------------------------------------------------------------------------

echo "Start at                                : "$(date +"%d-%b-%Y %T")

#rem Check and set build type
if [ "$1" == "r" ] || [ "$1" == "-r" ] || [ "$1" == "rebuild" ] || [ "$1" == "-rebuild" ]; then 
    BUILD_TYPE=--rebuild
else
    BUILD_TYPE=--build
fi
   
# rem Configure the CB_PARAMS
if [ "${OSDetected}" == "Windows" ]; then
    CB_PARAMS="--no-dde --multiple-instance --verbose --no-splash-screen --debug-log"
else    
    CB_PARAMS="--multiple-instance --verbose --no-splash-screen --debug-log"
fi    
#rem debugging:
#rem CB_PARAMS=${CB_PARAMS} --no-batch-window-close
#rem FUTURE: CB_PARAMS=${CB_PARAMS}  --batch-headless-build
#rem Private build: 
#       CB_PARAMS="${CB_PARAMS} --app-log-filename=Codeblocks_app_${BUILD_BITS}.log --debug-log-filename=Codeblocks_debug_${BUILD_BITS}.log"
# CB_PARAMS="${CB_PARAMS} --variable-set=wx-${WXWIDGET_DIR}"
CB_TARGET="--target=All ${BUILD_TYPE} ${CB_SRC_DIR}/${CB_WORKSPACE_FILENAME}"

START_SECONDS=${SECONDS}

echo
echo Building Code::Blocks. Please wait for the Code::Blocks compilation to finish.
echo
if [ "${OSDetected}" == "Windows" ]; then
    echo RUNNING: "${CB_PROG_FILE_DIR}/codeblocks.exe ${CB_PARAMS} ${CB_TARGET}"
    echo
    "${CB_PROG_FILE_DIR}/codeblocks.exe ${CB_PARAMS} ${CB_TARGET}"
else
    RUN_CMD="/usr/bin/codeblocks ${CB_PARAMS} ${CB_TARGET}"
    echo RUNNING: ${RUN_CMD}
    ${RUN_CMD}
    echo
fi

END_SECONDS=${SECONDS}
DIFF_SECONDS=$(( ${END_SECONDS} - ${START_SECONDS} ))

CODEBLOCKS_NAME=codeblocks
if [ -f "devel${WXWIDGET_DIR}_${BUILD_BITS}/CodeBlocks${EXEEXT}" ] ; then
    CODEBLOCKS_NAME=CodeBlocks
fi
echo "CODEBLOCKS_NAME=${CODEBLOCKS_NAME}"

if  [[
        ! -f "devel${WXWIDGET_DIR}_${BUILD_BITS}/${CODEBLOCKS_NAME}${EXEEXT}"
        &&
        ! -f "devel${WXWIDGET_DIR}_${BUILD_BITS}/share/${CODEBLOCKS_NAME}/plugins/ToolsPlus.${LIBEXT}"
    ]]; then
    echo
    echo
    echo -e "\\r${RED_START}+----------------------------------------------------+"
    echo -e "\\r${RED_START}| Error: Code::Blocks compile error was detected.    |"
    echo -e "\\r${RED_START}|         Please fix the error and try again.        |"
    echo -e "\\r${RED_START}+     -     -     -     -     -     -     -     -    +"
    echo -e "\\r${RED_START}|        Failed build time: " $(date -d "1970-01-01 + ${DIFF_SECONDS} seconds" "+%H:%M:%S") "               |"
    echo -e "\\r${RED_START}+----------------------------------------------------+${COLOR_REVERT}"
    cd ${StartExecutionDir}
    display_build_failed
    exit 5
fi

#rem ----------------------------------------------------------------
#rem Run the codeblocks_update_devel.sh if the user wanted it to run
#rem ----------------------------------------------------------------
if [ "${CB_FINISH_RUN_UPDATE_DEV}" == "Yes" ]; then
    echo
    echo Running "./codeblocks_update_devel.sh"
    TMP_DIR=${PWD}
    cd ${CurrentDir}
    source ./codeblocks_update_devel.sh
    cd ${TMP_DIR}
    unset TMP_DIR
    echo
    echo
fi
display_build_pass
echo
echo "- - - - - -- - - - - - - - - - - - - -"
echo "Codeblocks build duration : " $(date -d "1970-01-01 + ${DIFF_SECONDS} seconds" "+%H:%M:%S")
echo "- - - - - -- - - - - - - - - - - - - -"
echo

cd ${StartExecutionDir}
