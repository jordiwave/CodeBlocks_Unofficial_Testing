#!/bin/sh


#
# This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
# http://www.gnu.org/licenses/gpl-3.0.html
#
# --------------------------------------------------------------------------------------#
#                                                                                       #
#     This bash script builds CodeBlocks on windows using MSYS Mingw64 or on Linux      #
#                                                                                       #
# --------------------------------------------------------------------------------------#

reset
echo
echo
echo ' Code::Blocks build script running on ' $(uname)
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
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    exit 0
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    exit 0
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    exit 0
    ;;
  SunOS*)
    echo "SunOS is not supported"
    exit 0
    ;;
  *)
    echo "unknown: $OSTYPE is not supported"
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
    echo 'Detected Windows'
    export WX_CONFIG_NAME=$PWD/wx-config-cb-win64
    export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/wxWidgets-3.1.5_win64"
    export BOOST_ROOT=/mingw64
    prefixDir=$PWD/src/devel31_64
    configOptions="--prefix=$prefixDir --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=$BOOST_ROOT/lib"
    ;;
  *)
    echo "OSDetected not detected for $(uname)"
    exit 0
    ;;
esac

echo "Configure will set prefix directory to : \"$prefixDir\""

start_datetime=$(date +%s)
failureDetected='no'
echo 'Start at               : '$(date +"%d-%b-%Y %T")

if [ -f ./codeblocks_cleanup.sh ] ; then
    beforeStepStartTime=$(date +%s)

    if ./codeblocks_cleanup.sh  ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo './codeblocks_cleanup.sh finish at      : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
    else
        echo './codeblocks_cleanup.sh up failed'
        exit 1
    fi
fi

echo 'running ./boostrap'
beforeStepStartTime=$(date +%s)

if ./bootstrap > z_bootstrap_result.txt 2>&1 ; then
    durationTotalTime=$(($(date +%s)-start_datetime))
    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
    echo './Bootstrap finish at                  : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

    echo 'running ./configure ' $configOptions
    beforeStepStartTime=$(date +%s)
    if ./configure $configOptions > z_configure_result.txt 2>&1; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo './configure .... finish at             : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

        echo 'running make -j14...'
        beforeStepStartTime=$(date +%s)
        if make -j14 > z_make_j14_result.txt 2>&1; then
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo 'Make -j14 finish at                    : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

            echo 'running "make install"'
            beforeStepStartTime=$(date +%s)
            if make install > z_makeinstall_result.txt 2>&1; then
                durationTotalTime=$(($(date +%s)-start_datetime))
                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                echo '"Make install" finish at           : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
                if [ "$OSDetected" = "Windows" ] ; then
                    if [ -f ./codeblocks_update_devel.sh ] ; then
                        echo 'Running ./codeblocks_update_devel.sh'
                        if ./codeblocks_update_devel.sh > z_windows_update_devel_result.txt 2>&1; then
                            durationTotalTime=$(($(date +%s)-start_datetime))
                            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                            echo 'codeblocks_update_devel.sh finished at : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
                        else
                            echo './codeblocks_update_devel.sh failed'
                            failureDetected='yes'
                        fi
                    else
                        echo 'You will need to manually update the src/develxxx directory'
                    fi
                fi
            else
                echo 'make install failed'
                failureDetected='yes'
            fi
        else
            echo 'make failed'
            failureDetected='yes'
        fi
    else
        echo 'configure failed'
        failureDetected='yes'
    fi
else
    echo 'Bootstrap failed'
    failureDetected='yes'
fi

durationTotalTime=$(($(date +%s)-start_datetime))
echo
echo 'Finish at  : '$(date +"%d-%b-%Y %T")' ,  duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
echo
if test "x$failureDetected" = "xyes"; then :
    echo
    echo "*****************************************************************************************"
    echo "*                                                                                       *"
    echo "*  ######  #     # ### #       ######      #######    #    ### #       ####### ######   *"
    echo "*  #     # #     #  #  #       #     #     #         # #    #  #       #       #     #  *"
    echo "*  #     # #     #  #  #       #     #     #        #   #   #  #       #       #     #  *"
    echo "*  ######  #     #  #  #       #     #     #####   #     #  #  #       #####   #     #  *"
    echo "*  #     # #     #  #  #       #     #     #       #######  #  #       #       #     #  *"
    echo "*  #     # #     #  #  #       #     #     #       #     #  #  #       #       #     #  *"
    echo "*  ######   #####  ### ####### ######      #       #     # ### ####### ####### ######   *"
    echo "*                                                                                       *"
    echo "*****************************************************************************************"
    echo
else
    echo
    echo "+--------------------------------------------------------------------------------------+"
    echo "|                                                                                      |"
    echo "|                       *******      **      ********  ********                        |"
    echo "|                       **    **    ****    **        **                               |"
    echo "|                       **    **   **  **   **        **                               |"
    echo "|                       *******   **    **  ********* *********                        |"
    echo "|                       **       **********        **        **                        |"
    echo "|                       **       **      **        **        **                        |"
    echo "|                       **       **      ** ********  ********                         |"
    echo "|                                                                                      |"
    echo "+--------------------------------------------------------------------------------------+"
    echo
fi
echo
exit 0