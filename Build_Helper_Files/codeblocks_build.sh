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
# MUST configure soft link for wx-config
WXWIDGET_VERSION=3.2.0
WXWIDGET_DIR=32
#WXWIDGET_VERSION=3.1.7
#WXWIDGET_DIR=31
#WXWIDGET_VERSION=3.0.5
#WXWIDGET_DIR=30
# MUST configure soft link for wx-config
BUILD_BITS=64
failureDetected="no"

# --------------------------------------------------------------------------------------
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
    cd ${CurrentDir}
    exit 3
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
  *)
    echo "unknown: ${OSTYPE} is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
esac


# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i CodeBlocks_${WXWIDGET_VERSION}_$(uname)_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -------------------------------------------------------------------------------------------------
# find / -name wx-config 2>/dev/null
#
# check which wx-config is default run "wx-config --list"
#
if [ "${OSDetected}" == "Linux" ] ; then
    # ========================
    #  Linux Mint and XUbuntu:
    echo "running:  sudo rm /usr/local/bin/wx-config"
    sudo rm /usr/local/bin/wx-config 
    sudo rm /usr/local/bin/wx-config 
    if [ "${WXWIDGET_DIR}" == 30 ] ; then
        echo "running:     sudo ln -s /usr/lib/x86_64-linux-gnu/wx/config/gtk3-unicode-3.0   /usr/local/bin/wx-config"
        sudo ln -s /usr/lib/x86_64-linux-gnu/wx/config/gtk3-unicode-3.0   /usr/local/bin/wx-config
    fi
    if [ "${WXWIDGET_DIR}" == 31 ] ; then
        echo "running: sudo ln -s /usr/local/lib/wx/config/gtk3-unicode-3.1              /usr/local/bin/wx-config"
        sudo ln -s /usr/local/lib/wx/config/gtk3-unicode-3.1              /usr/local/bin/wx-config
    fi
    if [ "${WXWIDGET_DIR}" == 32 ] ; then
        echo "running:  sudo ln -s /usr/local/lib/wx/config/gtk3-unicode-3.2              /usr/local/bin/wx-config"
        sudo ln -s /usr/local/lib/wx/config/gtk3-unicode-3.2              /usr/local/bin/wx-config
    fi
else
    if [ "${OSDetected}" == "Windows" ] ; then
        # ========================
        # Windows testing
        echo "Testing for OS ${OSTYPE}"
    else
        # ========================
        # Mac OSX 
        echo "OS ${OSTYPE} is not supported yet!!!"
        cd ${CurrentDir}
        exit 3
    fi
fi

# -------------------------------------------------------------------------------------------------
# uncomment the following line for debugging:
# export
# -------------------------------------------------------------------------------------------------

echo
echo "+-------------------------------------------------------------------------+"
echo "|  Code::Blocks build script running on " $(uname) "                           |"
if [ "${GITHUB_ACTIONS}" == "true" ] ; then
    echo "|      You are running under GITHUB ACTIONS.                              |"
    if [ "${OSDetected}" == "Windows" ] ; then
        if [ "${NUMBER_OF_PROCESSORS}" ==  2 ] ; then
            echo "|       Setting BUILD_MAKE_CONCURENCY=-j2                                 |"
            BUILD_MAKE_CONCURENCY=-j2
        else
            # using self-hosted local build
            echo "|       Local self hosted build, so setting BUILD_MAKE_CONCURENCY=-j      |"
            BUILD_MAKE_CONCURENCY=-j
        fi
    else
        echo "|       Setting BUILD_MAKE_CONCURENCY=-j2                                 |"
        BUILD_MAKE_CONCURENCY=-j2
    fi
else
    if [ "${OSDetected}" == "Windows" ] ; then
        BUILD_MAKE_CONCURENCY=-j
        echo "|       Windows build, so setting BUILD_MAKE_CONCURENCY=-j                |"
    else
        echo "|       Setting BUILD_MAKE_CONCURENCY=-j4                                 |"
        BUILD_MAKE_CONCURENCY=-j4
    fi
fi
if [ "${OSDetected}" == "Windows" ] ; then
    echo "|       Windows $(expr substr $(uname -s) 12 4)                                                      |"
fi
echo "+-------------------------------------------------------------------------+"

# -------------------------------------------------------------------------------------------------
if [ ! -f "bootstrap" ]; then
    if [ -f "../bootstrap" ]; then
        cd ..
    else
        if [ -f "../../bootstrap" ]; then
            cd ../..
        else
            echo Could not find bootstrap or ../bootstrap or ../../bootstrap
            cd ${CurrentDir}
            exit 2
        fi
    fi
fi

# -------------------------------------------------------------------------------------------------

echo "| CurrentDir C::B root is: ${PWD}                              |"
echo "+-------------------------------------------------------------------------+"

case "${OSDetected}" in

  OSX* | Linux*)
    unset WX_CONFIG_NAME
    unset WX_CB_BUILD_DIR
    unset BOOST_ROOT
    prefixDir=${PWD}/src/devel${WXWIDGET_DIR}
    configOptions="--prefix=${prefixDir} --with-contrib-plugins=all"
    ;;

  Windows*)
    echo "| Detected Windows OS                                                     |"

    if [ ! -d "${WX_CB_BUILD_DIR}" ]; then
        unset WX_CB_BUILD_DIR
    fi

    if [ "${GITHUB_ACTIONS}" == "true" ] ; then
        if [ -d "Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}" ] ; then 
            export WX_CB_BUILD_DIR=Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
        else
            if [ -d "/d/a/CodeBlocks_Unofficial_Testing/CodeBlocks_Unofficial_Testing/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}" ] ; then 
                export WX_CB_BUILD_DIR=/d/a/CodeBlocks_Unofficial_Testing/CodeBlocks_Unofficial_Testing/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
            else
                if [ -d "/d/a/CodeBlocks_Unofficial_Testing/CodeBlocks_Unofficial_Testing/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}" ] ; then 
                    export WX_CB_BUILD_DIR=/d/a/CodeBlocks_Unofficial_Testing/CodeBlocks_Unofficial_Testing/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
                fi
            fi
        fi
    fi

    if [ "${WX_CB_BUILD_DIR}" == "" ]; then
        WX_DIR_FIND=Libraries/github_wxWidget_${WXWIDGET_VERSION}
        if [ -d "../${WX_DIR_FIND}" ]; then
            export WX_CB_BUILD_DIR=${PWD}/../${WX_DIR_FIND}
        else
            if [ -d "../../${WX_DIR_FIND}" ]; then
                export WX_CB_BUILD_DIR=${PWD}/../../${WX_DIR_FIND}
            else
                if [ -d "../../../${WX_DIR_FIND}" ]; then
                    export WX_CB_BUILD_DIR=${PWD}/../../../${WX_DIR_FIND}
                else
                    if [ -d "../../../../${WX_DIR_FIND}" ]; then
                        export WX_CB_BUILD_DIR=${PWD}/../../../../${WX_DIR_FIND}
                    else
                        if [ -d "../../../../../${WX_DIR_FIND}" ]; then
                            export WX_CB_BUILD_DIR=${PWD}/../../../../../${WX_DIR_FIND}
                        fi
                    fi
                fi
            fi
        fi
    fi
    if [ "${WX_CB_BUILD_DIR}" == "" ]; then
        WX_DIR_FIND=Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}
        if [ -d "../${WX_DIR_FIND}" ]; then
            export WX_CB_BUILD_DIR=${PWD}/../${WX_DIR_FIND}
        else
            if [ -d "../../${WX_DIR_FIND}" ]; then
                export WX_CB_BUILD_DIR=${PWD}/../../${WX_DIR_FIND}
            else
                if [ -d "../../../${WX_DIR_FIND}" ]; then
                    export WX_CB_BUILD_DIR=${PWD}/../../../${WX_DIR_FIND}
                else
                    if [ -d "../../../../${WX_DIR_FIND}" ]; then
                        export WX_CB_BUILD_DIR=${PWD}/../../../../${WX_DIR_FIND}
                    else
                        if [ -d "../../../../../${WX_DIR_FIND}" ]; then
                            export WX_CB_BUILD_DIR=${PWD}/../../../../../${WX_DIR_FIND}
                        fi
                    fi
                fi
            fi
        fi
    fi

    if [ "${WX_CB_BUILD_DIR}" == "" ]; then
        if [ -d "/d/Andrew_Development/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}" ] ; then 
            export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/wxWidgets-${WXWIDGET_VERSION}_win${BUILD_BITS}"
        fi
        if [ "${WX_CB_BUILD_DIR}" == "" ]; then
            if [ -d "/d/Andrew_Development/Libraries/github_wxWidget_${WXWIDGET_VERSION}" ] ; then 
                export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/github_wxWidget_${WXWIDGET_VERSION}"
            fi
        fi
    fi

    # -------------------------------------------------------------------------------------------------
    # Check wxWidget directory exists
    # -------------------------------------------------------------------------------------------------
    if [ ! -d "${WX_CB_BUILD_DIR}" ]; then
        echo "|                                                                         |"
        echo "|   +-----------------------------------------------------------------+   |"
        echo "|   | Error: NO WX_CB_BUILD_DIR environment variable set!             |   |"
        echo "|   |        Please export WX_CB_BUILD_DIR environment and try again. |   |"
        echo "|   |        WX_CB_BUILD_DIR is wxWidget src root dir                 |   |"
        echo "|   +-----------------------------------------------------------------+   |"
        echo "|                                                                         |"
        echo "+-------------------------------------------------------------------------+"
        echo
        echo "a Debug Info - Up one directory: ${PWD}/.. "
        ls -la ${PWD}/..
        echo
        cd ${CurrentDir}
        exit 4
    fi

    export WX_CONFIG_NAME=${PWD}/wx-config-cb-win${BUILD_BITS}_${WXWIDGET_DIR} --version=${WXWIDGET_DIR}
    export BOOST_ROOT=/mingw${BUILD_BITS}
    prefixDir=${PWD}/src/devel${WXWIDGET_DIR}_${BUILD_BITS}
    configOptions="--prefix=${prefixDir} --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=${BOOST_ROOT}/lib AR_FLAGS=cr"

    echo "Setup the following external environment variables:"
    echo "    export WX_CONFIG_NAME=${PWD}/wx-config-cb-win${BUILD_BITS}_${WXWIDGET_DIR} --version=${WXWIDGET_DIR}"
    echo "    export BOOST_ROOT=/mingw${BUILD_BITS}"
    echo "Setup the following internal variables:"
    echo "    prefixDir=${PWD}/src/devel${WXWIDGET_DIR}_${BUILD_BITS}"
    echo "    configOptions=--prefix=${prefixDir} --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=${BOOST_ROOT}/lib AR_FLAGS=cr"
    ;;

  *)
    echo "OSDetected not detected for $(uname)"
    cd ${CurrentDir}
    exit 5
    ;;
esac

if [ ! "${WX_CB_BUILD_DIR}" == "" ]; then
    export WX_CB_BUILD_DIR=$(realpath ${WX_CB_BUILD_DIR})
    echo "| WX_CB_BUILD_DIR: ${WX_CB_BUILD_DIR}     |"
fi  
echo "| BUILD_BITS:      ${BUILD_BITS}                                                     |"
if [ ! "${WX_CONFIG_NAME}" == "" ]; then
    echo "| WX_CONFIG_NAME:  ${WX_CONFIG_NAME}                      |"
fi  
if [ ! "${BOOST_ROOT}" == "" ]; then
    echo "| BOOST_ROOT:      ${BOOST_ROOT}                                 |"
fi  
echo "| prefixDir:       ${prefixDir}                           |"
echo "| PWD:             ${PWD} |"
echo "+-------------------------------------------------------------------------+"
echo

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    # ------------------------------------------------------------------------------------------------------------
    # Setup ANSI color control variables
    # ------------------------------------------------------------------------------------------------------------
    ECHO_E="-e"
    ECHO_N="-e"
    CR="\\r"
    CHECK_MARK="\033[2K\033[0;32m\xE2\x9C\x94\033[0m"
    CROSS_MARK="\033[2K\033[0;31m\xE2\x9C\x98\033[0m"
    # 0 reset, 1 bold, 32 green
    GREEN_START="\033[0;1;32m"
    # 0 reset, 1 bold, 5 block slow, 31 red
    RED_START="\033[0;1;5;31m"
    COLOR_REVERT="\033[0m"
fi


# -------------------------------------------------------------------------------------------------

start_datetime=$(date +%s)
echo "Start at                                : "$(date +"%d-%b-%Y %T")

if [ -f codeblocks_cleanup.sh ] ; then
    beforeStepStartTime=$(date +%s)

    echo ${ECHO_N} "running ./codeblocks_cleanup.sh"
    if ./codeblocks_cleanup.sh  ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo ${ECHO_E} "${CR}${CHECK_MARK}./codeblocks_cleanup.sh finish at  : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
    else
        echo ${ECHO_E} "${CR}${CROSS_MARK}./codeblocks_cleanup.sh up failed"
        cd ${CurrentDir}
        exit 6
    fi
fi

# -------------------------------------------------------------------------------------------------
# Configure, build & install using selected configuration 
# -------------------------------------------------------------------------------------------------
echo "===================================================================================================="
echo ${ECHO_N} "running ./update_revision.sh"
beforeStepStartTime=$(date +%s)
./update_revision.sh
status=$?
if [ $status == 0 ] ; then
    durationTotalTime=$(($(date +%s)-start_datetime))
    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
    echo ${ECHO_E} "${CR}${CHECK_MARK}./update_revision.sh finish at         : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
	echo "===================================================================================================="

    echo ${ECHO_N} "running ./boostrap"
    beforeStepStartTime=$(date +%s)

    ./bootstrap
    status=$?
    if [ $status == 0 ] ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo ${ECHO_E} "${CR}${CHECK_MARK}./Bootstrap finish at                  : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
		echo "===================================================================================================="

        echo ${ECHO_N} "running ./configure " ${configOptions}
        beforeStepStartTime=$(date +%s)
        ./configure ${configOptions}
        # example ./configure --prefix=${PWD}/src/devel32 --with-contrib-plugins=all
        status=$?
        if [ $status == 0 ] ; then
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo ${ECHO_E} "${CR}${CHECK_MARK}./configure .... finish at             : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
			echo "===================================================================================================="

            echo ${ECHO_N} "running make ${BUILD_MAKE_CONCURENCY}..."
            beforeStepStartTime=$(date +%s)

            make ${BUILD_MAKE_CONCURENCY}
            status=$?
            if [ $status == 0 ] ; then
                durationTotalTime=$(($(date +%s)-start_datetime))
                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                echo ${ECHO_E} "${CR}${CHECK_MARK}Make -j14 finish at                    : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
				echo "===================================================================================================="

                echo ${ECHO_N} "running "make install""
                beforeStepStartTime=$(date +%s)

                make install
                status=$?
                if [ $status == 0 ] ; then
                    durationTotalTime=$(($(date +%s)-start_datetime))
                    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                    echo ${ECHO_E} "${CR}${CHECK_MARK}Make install finish at                 : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
					echo "===================================================================================================="

                else
                    echo ${ECHO_E} "${CR}${CROSS_MARK}make install failed"
                    failureDetected="yes"
                fi
            else
                echo ${ECHO_E} "${CR}${CROSS_MARK}make failed"
                failureDetected="yes"
            fi
        else
            echo ${ECHO_E} "${CR}${CROSS_MARK}configure failed"
            failureDetected="yes"
        fi
    else
        echo ${ECHO_E} "${CR}${CROSS_MARK}Bootstrap failed"
        failureDetected="yes"
    fi
else
    echo ${ECHO_E} "${CR}${CROSS_MARK}./update_revision.sh failed"
    failureDetected="yes"
fi

# ------------------------------------------------------------------------------------------------------------
# Build finished - pass or fail
# ------------------------------------------------------------------------------------------------------------
echo "Finished at                                : "$(date +"%d-%b-%Y %T")
echo
durationTotalTime=$(($(date +%s)-start_datetime))
echo "+-----------------------------------------------------------+"
echo "|                                                           |"
echo "| Finish at  : "$(date +"%d-%b-%Y %T")" ,  duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")"  |"
echo "|                                                           |"
echo "+-----------------------------------------------------------+"
echo

cd ${CurrentDir}

# -------------------------------------------------------------------------------------------------

if test "x${failureDetected}" = "xyes"; then :
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
    echo ${ECHO_E} "${CR}${COLOR_REVERT}"
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
    echo ${ECHO_E} "${CR}${COLOR_REVERT}"
fi

if [ "${OSDetected}" == "Windows" ] ; then
    if [ -f ./Build_Helper_Files/codeblocks_update_devel.sh ] ; then
        echo "Running ./Build_Helper_Files/codeblocks_update_devel.sh"
        ./Build_Helper_Files/codeblocks_update_devel.sh
        status=$?
        if [ $status == 0 ] ; then
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo ${ECHO_E} "${CR}${CHECK_MARK}codeblocks_update_devel.sh finished at : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
        else
            echo ${ECHO_E} "${CR}${CROSS_MARK}./codeblocks_update_devel.sh failed"
            failureDetected="yes"
        fi
        echo "===================================================================================================="
    else
        echo ${ECHO_E} "${CR}${CROSS_MARK}You will need to manually update the src/devel${WXWIDGET_DIR}_64 directory as the codeblocks_update_devel.sh file could not be found!!!!"
        failureDetected="yes"
    fi
fi

exit 0

# -------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------