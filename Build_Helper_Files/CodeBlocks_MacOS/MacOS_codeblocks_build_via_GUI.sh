#!/bin/bash

reset
echo MacOS - build CODE::BLOCKS and WxWidgets if needed

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Set build variables
# ----------------------------------------------------------------------------
DEBUG_SCRIPT="Yes"

#WX_VERSION=3.1.7
#WX_GITHUB_TAG=3.1.7
#WX_DIR_VERSION=31
WX_VERSION=3.2.1
WX_GITHUB_TAG=3.2.1
WX_DIR_VERSION=32

InitialDir=${PWD}
failureDetected="no"

export CXX=clang++
export CC=clang

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i Codeblocks_GUI_wx${WX_VERSION}_$(uname)_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -----------------------------------------------------------------------------

function ErrNowxWidget()
{
    echo
    echo
    echo "+--------------------------------------------------------------------+"
    echo "| Error: Could not find the following directory:                     |"
    echo "|         ${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}  |"
    echo "+--------------------------------------------------------------------+"
    echo 
    echo
    cd ${InitialDir}
    exit 1
}

function ErrNoCB()
{
    echo
    echo
    echo "+----------------------------------------------------+"
    echo "| Error: C::B root folder not found.                 |"
    echo "|        Please fix the batch file and try again.    |"
    echo "+----------------------------------------------------+"
    echo 
    echo
    cd ${InitialDir}
    exit 1
}
function ErrProjectFile()
{
    echo
    echo
    echo "+--------------------------------------------------------------------+"
    echo "|     Error: Could not find the following file:                      |"
    echo "|             ${CB_ROOT_DIR}/src/CodeBlocks_MacOS.workspace file.    |"
    echo "+--------------------------------------------------------------------+"
    echo 
    echo
    cd ${InitialDir}
    exit 1
}

function CompileError()
{
    echo
    echo
    echo "+----------------------------------------------------+"
    echo "| Error: Code::Blocks compile error was detected.    |"
    echo "|        Please fix the error and try again.         |"
    echo "+----------------------------------------------------+"
    echo
    echo
    cd ${InitialDir}
    exit 1
}

function UpdateDevelFailure()
{
    echo
    echo
    echo "+-----------------------------------------------------------------------------------+"
    echo "| Error: Could not update the devel${WX_VERSION} via the codeblocks_update_devel.sh!    |"
    echo "|              Please fix the error and try again.                                  |"
    echo "+-----------------------------------------------------------------------------------+"
    echo
    echo
    cd ${InitialDir}
    exit 1
}


function setup_variables()
{
    # ------------------------------------------------------------------------------------------------------------
    # Detect and process OS info
    # ------------------------------------------------------------------------------------------------------------
    case "$(uname)" in
      Darwin*)
        OSDetected="OSX"
        ;;
      Linux*)
        OSDetected="Linux"
        echo "Linux is not supported"
        cd ${StartDir}
        exit 1
        ;;
      MINGW* | msys* | cygwin* | WindowsNT)
        OSDetected="Windows"
        echo "Windows is not supported"
        cd ${StartDir}
        exit 1
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

    if [ "${OSDetected}" != "OSX" ] ; then
        echo "${OSTYPE} is not supported"
        exit 2
    fi

    cd "${InitialDir}"
    if [ -f bootstrap ]; then
        CB_ROOT_DIR=$PWD
        cd ..
        BUILD_ROOT_DIR=$PWD
    else
        if [ -f ../bootstrap ]; then
            cd ..
            CB_ROOT_DIR=$PWD
            cd ..
            BUILD_ROOT_DIR=$PWD
        else
            if [ -f ../../bootstrap ]; then
                cd ../..
                CB_ROOT_DIR=$PWD
                cd ..
                BUILD_ROOT_DIR=$PWD
            else
                BUILD_ROOT_DIR=$PWD
            fi
        fi
    fi
    
    if [ ! -f "${CB_ROOT_DIR}/bootstrap" ]; then
        echo "Could not detect CB root directory with the bootstrap file in it!"
        ErrNoCB
    fi
    CB_DEV_OUTPUT_DIR=${CB_ROOT_DIR}/src/devel${WX_DIR_VERSION}

    #cd "${BUILD_ROOT_DIR}"
    #if [ -d "${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}" ]; then
    #    WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}"
    #else
    #    ErrNowxWidget
    #fi

    if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
        echo BUILD_ROOT_DIR = ${BUILD_ROOT_DIR}
        echo WX_ROOT_DIR = ${WX_ROOT_DIR}
        echo CB_ROOT_DIR = ${CB_ROOT_DIR}
        echo CB_DEV_OUTPUT_DIR= ${CB_DEV_OUTPUT_DIR}
    fi


    # Check and set build type
    BUILD_TYPE="--build"
    CB_EXE="codeblocks"

    # SETUP CB PARAMETERS
    CB_PARAMS="--no-dde --multiple-instance --verbose --no-splash-screen --debug-log"
    # N/A for MAC: set CB_PARAMS=${CB_PARAMS} --variable-set=cb_win%BUILD_BITS%

    # debugging:
    # set CB_PARAMS=${CB_PARAMS} --no-batch-window-close
    # FUTURE: set CB_PARAMS=${CB_PARAMS}  --batch-headless-build

    if [ ! -f "${CB_ROOT_DIR}/src/CodeBlocks_Unix_MacOS.workspace" ]; then
        ErrProjectFile
    fi

    CB_TARGET="--target=All ${BUILD_TYPE} CodeBlocks_Unix_MacOS.workspace"
    CB_RUN_COMMAND_LINE="${CB_EXE} ${CB_PARAMS} ${CB_TARGET}"

    if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
        echo CB_RUN_COMMAND_LINE =${CB_RUN_COMMAND_LINE}
    fi

}

function cb_build()
{
    # ----------------------------------------------------------------
    # Ask the user if they want to run the Windows_Ouput_Create.bat after the build
    # ----------------------------------------------------------------
    DLL_COUNT=$(ls -1q ${CB_ROOT_DIR}/src/devel* 2>/dev/null | wc -l)
    if [ ${DLL_COUNT} -gt 0 ] ; then
        while true; do
            read -p "Do you want to delete the previous build directories [Y/N]? " yn
            case $yn in
                [Yy]* ) cd ${InitialDir} ; source ${InitialDir}/codeblocks_cleanup.sh; break;;
                [Nn]* ) echo "Leaving previous build directories" ; break;;
                * ) echo "Please answer yes or no.";;
            esac
        done
    fi

    # ----------------------------------------------------------------------------
    # Build command shell and wait until finished before continuing
    # ----------------------------------------------------------------------------

    START_SECONDS=$SECONDS

    cd ${CB_ROOT_DIR}/src
    echo RUNNING: ${CB_RUN_COMMAND_LINE}
    ${CB_RUN_COMMAND_LINE}

    END_SECONDS=$SECONDS
    DIFF_SECONDS=$(( $END_SECONDS - $START_SECONDS ))

    if [ ! -f "{CB_DEV_OUTPUT_DIR}/CodeBlocks" ] ; then CompileError; fi
    if [ ! -f "{CB_DEV_OUTPUT_DIR}/share/CodeBlocks/plugins/ToolsPlus.dylib" ] ; then CompileError; fi
    if [ ! -f "{CB_DEV_OUTPUT_DIR}/share/CodeBlocks/plugins/debugger_dap.dylib" ] ; then CompileError; fi
}

# -------------------------------------------------------------------------------------------------

# setup variables based on directories found. Variables used in building wxWidgets and Code::Blocks
setup_variables

echo
echo "+======================================================================================+"
echo "|                                                                                      |"
echo "|      Code::Blocks build script for ${OSDetected} and WxWidgets ${WX_VERSION}                           |"
echo "|                                                                                      |"
echo "+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +"
echo "|                                                                                      |"
echo "|  Code::Block dir: ${CB_ROOT_DIR}                   |"
echo "|                                                                                      |"
echo "|  WxWidgets dir: ${WX_ROOT_DIR}                                 |"
echo "|                                                                                      |"
echo "|  Code::Block output dir: ${CB_DEV_OUTPUT_DIR}                                 |"
echo "+======================================================================================+"
echo
echo

cb_build

# source codeblocks_update_devel.sh


cd ${InitialDir}
exit 0