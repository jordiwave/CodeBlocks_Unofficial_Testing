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

# -----------------------------------------------------------------------------

reset
echo
echo "+-----------------------------------------------------+"
echo "|  Code::Blocks build script running on " $(uname) "  |"
if [ "$GITHUB_ACTIONS" == "true" ] ; then
    echo "|                                                     |"
    echo "|      You are running under GITHUB ACTIONS.          |"
    echo "|                                                     |"
fi
echo "+-----------------------------------------------------+"
echo

CurrentDir=$PWD

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

# -----------------------------------------------------------------------------

echo "CurrentDir C::B root is: " $PWD
echo
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
    echo "unknown: $OSTYPE is not supported"
    cd $CurrentDir
    exit 0
    ;;
esac


case "$OSDetected" in
  OSX* | Linux*)
    unset WX_CONFIG_NAME
    unset WX_CB_BUILD_DIR
    unset BOOST_ROOT
    prefixDir=$PWD/src/devel31
    configOptions="--prefix=$prefixDir --with-contrib-plugins=all"
    ;;
  Windows*)
    echo "Detected Windows"
    export WX_CONFIG_NAME=$PWD/wx-config-cb-win64
    export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/wxWidgets-3.1.6_win64"
    export BOOST_ROOT=/mingw64
    prefixDir=$PWD/src/devel31_64
    configOptions="--prefix=$prefixDir --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=$BOOST_ROOT/lib AR_FLAGS=cr"
    ;;
  *)
    echo "OSDetected not detected for $(uname)"
    cd $CurrentDir
    exit 0
    ;;
esac

echo "Configure will set prefix directory to : \"$prefixDir\""

# -----------------------------------------------------------------------------

CHECK_MARK="\033[2K\033[0;32m\xE2\x9C\x94\033[0m"
CROSS_MARK="\033[2K\033[0;31m\xE2\x9C\x98\033[0m"
# 0 reset, 1 bold, 32 green
GREEN_START="\033[0;1;32m"
# 0 reset, 1 bold, 5 block slow, 31 red
RED_START="\033[0;1;5;31m"
COLOR_REVERT="\033[0m"

# -----------------------------------------------------------------------------

start_datetime=$(date +%s)
failureDetected="no"
echo "Start at                                : "$(date +"%d-%b-%Y %T")

# -----------------------------------------------------------------------------

if [ -f ./codeblocks_cleanup.sh ] ; then
    beforeStepStartTime=$(date +%s)

    echo -n "running ./codeblocks_cleanup.sh"
    if ./codeblocks_cleanup.sh  ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo -e "\\r${CHECK_MARK}./codeblocks_cleanup.sh finish at  : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
    else
        echo -e "\\r${CROSS_MARK}./codeblocks_cleanup.sh up failed"
        cd $CurrentDir
        exit 1
    fi
fi

# -----------------------------------------------------------------------------

# Configure, build & install using selected configuration 
echo -n "running ./update_revision.sh"
beforeStepStartTime=$(date +%s)
if [ "$GITHUB_ACTIONS" == "true" ] ; then
    ./update_revision.sh
    status=$?
else
    ./update_revision.sh > z_update_revision_result.txt 2>&1
    status=$?
fi
if [ $status == 0 ] ; then
    durationTotalTime=$(($(date +%s)-start_datetime))
    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
    echo -e "\\r${CHECK_MARK}./update_revision.sh finish at         : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

    echo -n "running ./boostrap"
    beforeStepStartTime=$(date +%s)

    if [ "$GITHUB_ACTIONS" == "true" ] ; then
        ./bootstrap
        status=$?
    else
        ./bootstrap > z_bootstrap_result.txt 2>&1
        status=$?
    fi
    if [ $status == 0 ] ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo -e "\\r${CHECK_MARK}./Bootstrap finish at                  : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

        echo -n "running ./configure " $configOptions
        beforeStepStartTime=$(date +%s)
        if [ "$GITHUB_ACTIONS" == "true" ] ; then
            ./configure $configOptions
            status=$?
        else
            ./configure $configOptions > z_configure_result.txt 2>&1
            status=$?
        fi
        if [ $status == 0 ] ; then
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo -e "\\r${CHECK_MARK}./configure .... finish at             : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

            echo -n "running make -j14..."
            beforeStepStartTime=$(date +%s)

            if [ "$GITHUB_ACTIONS" == "true" ] ; then
                make -j14
                status=$?
            else
                make -j14 > z_make_j14_result.txt 2>&1
                status=$?
            fi
            if [ $status == 0 ] ; then
                durationTotalTime=$(($(date +%s)-start_datetime))
                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                echo -e "\\r${CHECK_MARK}Make -j14 finish at                    : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

                echo -n "running "make install""
                beforeStepStartTime=$(date +%s)

                if [ "$GITHUB_ACTIONS" == "true" ] ; then
                    make install
                    status=$?
                else
                    make install > z_makeinstall_result.txt 2>&1
                    status=$?
                fi
                if [ $status == 0 ] ; then
                    durationTotalTime=$(($(date +%s)-start_datetime))
                    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                    echo -e "\\r${CHECK_MARK}Make install finish at                 : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

                    if [ "$OSDetected" = "Windows" ] ; then
                        if [ -f ./codeblocks_update_devel.sh ] ; then
                            echo "Running ./codeblocks_update_devel.sh"
                            if ./codeblocks_update_devel.sh > z_windows_update_devel_result.txt 2>&1; then
                                durationTotalTime=$(($(date +%s)-start_datetime))
                                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                                echo -e "\\r${CHECK_MARK}codeblocks_update_devel.sh finished at : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
                            else
                                echo -e "\\r${CROSS_MARK}./codeblocks_update_devel.sh failed"
                                failureDetected="yes"
                            fi
                        else
                            echo "You will need to manually update the src/develxxx directory"
                        fi
                    fi
                else
                    echo -e "\\r${CROSS_MARK}make install failed"
                    failureDetected="yes"
                fi
            else
                echo -e "\\r${CROSS_MARK}make failed"
                failureDetected="yes"
            fi
        else
            echo -e "\\r${CROSS_MARK}configure failed"
            failureDetected="yes"
        fi
    else
        echo -e "\\r${CROSS_MARK}Bootstrap failed"
        failureDetected="yes"
    fi
else
    echo -e "\\r${CROSS_MARK}./update_revision.sh failed"
    failureDetected="yes"
fi
echo "Finished at                                : "$(date +"%d-%b-%Y %T")
echo

durationTotalTime=$(($(date +%s)-start_datetime))
echo "+-----------------------------------------------------------+"
echo "|                                                           |"
echo "| Finish at  : "$(date +"%d-%b-%Y %T")" ,  duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")"  |"
echo "|                                                           |"
echo "+-----------------------------------------------------------+"
echo

cd $CurrentDir

if test "x$failureDetected" = "xyes"; then :
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
    exit 999
else
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
    exit 0
fi
