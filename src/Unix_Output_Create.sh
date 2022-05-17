#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#
#                                                                                       #
#     This bash script builds CodeBlocks on either :                                    #
#           - Windows using MSYS2 Mingw64                                               #
#           - Linux                                                                     #
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

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and to the 
    # file specified:
    exec > >(tee -i codeBlocks_Update_Dev.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -------------------------------------------------------------------------------------------------

case "$(uname)" in
  Darwin*)
    OSDetected="OSX"
    EXEEXT=""
    LIBEXT="dylib"    
    ;;
  Linux*)
    OSDetected="Linux"
    EXEEXT=""
    LIBEXT="so"    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    OSDetected="Windows"
    EXEEXT=".exe"
    LIBEXT="dll" 
    ;;
  AIX*)
    echo "AIX is not supported"
    cd $CurrentDir
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    cd $CurrentDir
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    cd $CurrentDir
    exit 0
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd $CurrentDir
    exit 0
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd $CurrentDir
    exit 0
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd $CurrentDir
    exit 0
    ;;
  *)
    echo "Unknown: ${OSTYPE} is not supported"
    cd $CurrentDir
    exit 0
    ;;
esac

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
            echo Could not find bootstrap or ../bootstrap or ../../bootstrap
            cd $CurrentDir
            exit 3
        fi
    fi
fi

echo "|    CurrentDir C::B root is:            ${PWD}                                                  |"

CB_ROOT=$PWD
CB_SRC=${CB_ROOT}/src

# ----------------------------------------------------------------------------
# Check BUILD_BITS for validity
# ----------------------------------------------------------------------------

if [ -d "${CB_SRC}\devel31_32" ] ; then  export BUILD_BITS=32 ; fi
if [ -d "${CB_SRC}\devel31_64" ] ; then  export BUILD_BITS=64 ; fi
if [ "${BUILD_BITS}" == "" ] ; then
    echo "+-------------------------------------------------------------------------------------------------+"
    echo "|                                                                                                 |"
    echo "|             +------------------------------------------------------+                            |"
    echo "|             | Error: Cannot find build type \"32\" or \"64\".      |                            |"
    echo "|             |        Cannot find src\devel31_32 or src\devel31_64  |                            |"
    echo "|             |        Please run again with a parameter             |                            |"
    echo "|             +------------------------------------------------------+                            |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    echo BUILD_BITS:${BUILD_BITS}
    cd ${CurrentDir}
    exit 4
fi

# ----------------------------------------------------------------------------
# Check if build succeeded
# ----------------------------------------------------------------------------
#case "$(OSDetected)" in
#  Windows*)
if { !  {
            [ -f "${CB_SRC}/devel31_${BUILD_BITS}/codeblocks${EXEEXT}" ]                        &&
            [ -f "${CB_SRC}/devel31_${BUILD_BITS}/libcodeblocks.${LIBEXT}" ]                    &&
            {
                [ -f "${CB_SRC}/devel31_${BUILD_BITS}/share/codeblocks/plugins/todo.${LIBEXT}" ] ||
                [ -f "${CB_SRC}/devel31_${BUILD_BITS}/share/codeblocks/plugins/libtodo.${LIBEXT}" ]
            }
        }
   } then
    echo "|                                                                                                 |"
    echo "|             +--------------------------------------------------------------------+              |"
    echo "|             | Error: Code::Blocks make error was detected.                       |              |"
    echo "|             |        Please fix the error and try again.                         |              |"
    [ ! -f "${CB_SRC}/devel31_${BUILD_BITS}/codeblocks${EXEEXT}" ]                     && echo "|             |        Missing src/devel31_${BUILD_BITS}/codeblocks${EXEEXT}!                          |              |"
    [ ! -f "${CB_SRC}/devel31_${BUILD_BITS}/libcodeblocks.${LIBEXT}" ]                 && echo "|             |        Missing src/devel31_${BUILD_BITS}/libcodeblocks.${LIBEXT}!                 |              |"
#    [ ! -f "${CB_SRC}/devel31_${BUILD_BITS}/share/codeblocks/plugins/todo.${LIBEXT}" ] && echo "|             |        Missing src/devel31_${BUILD_BITS}/share/codeblocks/plugins/todo.${LIBEXT}! |              |"
    [ ! -f "${CB_SRC}/devel31_${BUILD_BITS}/share/codeblocks/plugins/libtodo.${LIBEXT}" ] && echo "|             |        Missing src/devel31_${BUILD_BITS}/share/codeblocks/plugins/libtodo.${LIBEXT}! |              |"
    echo "|             +--------------------------------------------------------------------+              |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd $CurrentDir
    exit 5
fi

# ----------------------------------------------------------------------------

STRIP_EXE=strip.exe
CB_DEVEL_DIR=${CB_SRC}/devel31_${BUILD_BITS}
CB_OUTPUT_DIR=${CB_SRC}/output31_${BUILD_BITS}

# -----------------------------------------------------------------------------

echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                                                                                                 |"
echo "|                           Updating C::B directory build files.                                  |"
echo "|                                                                                                 |"
echo "| Detected OS:   ${OSDetected}                                                               |"
echo "| CB_ROOT:       ${CB_ROOT}                                                        |"
echo "| CB_SRC:        ${CB_SRC}                                                         |"
echo "| BUILD_BITS:    ${BUILD_BITS}                                                                      |"
echo "| PWD:           ${PWD} |"
echo "|                                                                                                 |"
echo "| CB_DEVEL_DIR:  ${CB_DEVEL_DIR}    |"
echo "| CB_OUTPUT_DIR: ${CB_OUTPUT_DIR}     |"
echo "+-------------------------------------------------------------------------------------------------+"


# ----------------------------------------------------------------------------

if [ ! -d ${CB_DEVEL_DIR} ] ; then 
    echo "+--------------------------------------------------------------------+"
    echo "| ERROR: The development directory ${CB_DEVEL_DIR} does not exist.   |"
    echo "|        Please fix the error and try again.                         |"
    echo "+--------------------------------------------------------------------+"
    cd $CurrentDir
    exit 6
fi

if [ -d ${CB_OUTPUT_DIR} ] ; then 
    echo "The output directory ${CB_OUTPUT_DIR} exists, deleting it."
    rm -rf ${CB_OUTPUT_DIR}
fi

if [ ! -d ${CB_OUTPUT_DIR} ] ; then 
    echo "Creating output directory ${CB_OUTPUT_DIR}."
    mkdir -p ${CB_OUTPUT_DIR}
fi

# ----------------------------------------------------------------------------

PreviousDir=$PWD
cd ${CB_DEVEL_DIR}
echo cp -R . "${CB_OUTPUT_DIR}"
cp -R . "${CB_OUTPUT_DIR}"
cd ${PreviousDir}

# ----------------------------------------------------------------------------

PreviousDir=$PWD
cd ${CB_OUTPUT_DIR}
if [ "${OSDetected}" == "Windows" ] ; then 
    find . -type f -name "*.exe" | xargs ${STRIP_EXE}
    find . -type f -name "*.dll" | xargs ${STRIP_EXE}
fi
if [ "${OSDetected}" == "Linux" ] ; then 
    find . -type f -name "*.so"  | xargs ${STRIP_EXE}
fi
cd ${PreviousDir}

# ----------------------------------------------------------------------------

# @rem Use the correct files for the version of windows being used
# @for /f "tokens=4-7 delims=[.] " %%i in ('ver') do @(@if %%i==Version (set WIN_Version=%%j.%%k) else (set WIN_Version=%%i.%%j))
# @if "%WIN_Version%" == "10.0"       set CB_HANDLER_WIN_DIR=Win_10
# @if "%WIN_Version%" ==  "6.3"       set CB_HANDLER_WIN_DIR=Win_7
# @if "%WIN_Version%" ==  "6.2"       set CB_HANDLER_WIN_DIR=Win_7
# @if "%WIN_Version%" ==  "5.2"       set CB_HANDLER_WIN_DIR=Win_XP
# @if "%WIN_Version%" ==  "5.1"       set CB_HANDLER_WIN_DIR=Win_XP
# @if "%CB_HANDLER_WIN_DIR%" == ""    set CB_HANDLER_WIN_DIR=Win_7
# 
# @REM Copy these files after stripping symbols otherwise CB will not start as the files will be corrupted
# @set CB_HANDLER_DIR=%CB_ROOT%\src\exchndl\%CB_HANDLER_WIN_DIR%\win%BUILD_BITS%\bin
# @if exist "%CB_HANDLER_DIR%" xcopy /y "%CB_HANDLER_DIR%\*.dll" "%CB_OUTPUT_DIR%\" > nul

# ----------------------------------------------------------------------------

cd ${CurrentDir}
