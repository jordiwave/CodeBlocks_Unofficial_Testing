#!/bin/sh

#
# This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
# http://www.gnu.org/licenses/gpl-3.0.html
#
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
    configOptions="--with-contrib-plugins=all"
    ;;
  Windows*)
    echo 'Detected Windows'
    export WX_CONFIG_NAME=$PWD/wx-config-cb-win64
    export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/wxWidgets-3.1.5_win64"
    export BOOST_ROOT=/mingw64
    configOptions="--prefix=$PWD/src/devel31_64 --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=$BOOST_ROOT/lib"
    ;;
  *)
    echo "OSDetected not detected for $(uname)"
    exit 0
    ;;
esac

start_datetime=$(date +%s)
failureDetected='no'
echo 'Start at               : '$(date +"%d-%b-%Y %T")

if [ -f ./windows_cleanup.sh ] ; then
    beforeStepStartTime=$(date +%s)

    if ./windows_cleanup.sh  ; then
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo 'Cleaning finish at     : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
    else
        echo 'Cleaning up failed'
        exit 1
    fi
fi

echo 'running ./boostrap'
beforeStepStartTime=$(date +%s)

if ./bootstrap > z_bootstrap_result.txt 2>&1 ; then
    durationTotalTime=$(($(date +%s)-start_datetime))
    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
    echo 'Bootstrap finish at    : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

    echo 'running ./configure ' $configOptions
    beforeStepStartTime=$(date +%s)
    # ./configure --with-contrib-plugins=all > z_configure_result.txt
    # ./configure --prefix=$PWD/src/devel31_64 --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=$BOOST_ROOT/lib > z_configure_result.txt
    if ./configure $configOptions > z_configure_result.txt 2>&1; then
#    if ./configure --prefix=$PWD/src/devel31_64 --enable-windows-installer-build --with-contrib-plugins=all,-wxcontrib,-wxsmith,-wxsmithcontrib,-wxsmithaui --with-boost-libdir=$BOOST_ROOT/lib > z_configure_result.txt 2>&1; then
# Later -wxsmith, then wxcontrib , wxsmithcontrib , wxsmithaui
        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo 'Configure finish at    : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

        echo 'running make -j14...'
        beforeStepStartTime=$(date +%s)
        if make -j14 -k > z_make_j14_result.txt 2>&1; then
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo 'Make -j14 finish at    : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
        else
            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo 'Make -j14 finish at    : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

            echo 'running make'
            beforeStepStartTime=$(date +%s)
            if make > z_make_result.txt 2>&1; then
                durationTotalTime=$(($(date +%s)-start_datetime))
                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                echo 'Make finish at         : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
            else
                echo 'make failed'
                failureDetected='yes'
            fi
        fi

        if [ "x$failureDetected" = "xno" ]; then
            if [ "$OSDetected" = "Windows" ] ; then
                echo 'running make install'
                beforeStepStartTime=$(date +%s)
                if make install > z_makeinstall_result.txt 2>&1; then
                    durationTotalTime=$(($(date +%s)-start_datetime))
                    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                    echo 'Make install finish at : '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
                    if [ -f ./windows_update_devel.sh ] ; then
                        echo 'Running ./windows_update_devel.sh'
                        if ./windows_update_devel.sh > z_windows_update_devel_result.txt 2>&1; then
                            durationTotalTime=$(($(date +%s)-start_datetime))
                            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                            echo 'windows_update_devel.sh: '$(date +"%d-%b-%Y %T")' , delta : '$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
                        else
                            echo './windows_update_devel.sh failed'
                        fi
                    else
                        echo 'You will need to manually update the src/develxxx directory'
                    fi
                else
                    echo 'make install failed'
                fi
            else
                echo "On $OSDetected you need manually run \"make install\""
            fi
        fi
    else
        echo 'configure failed'
    fi
else
    echo 'Bootstrap failed'
fi

durationTotalTime=$(($(date +%s)-start_datetime))
echo
echo 'Finish at    : '$(date +"%d-%b-%Y %T")' ,    duration : '$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
echo
if test "x$failureDetected" = "xyes"; then :
echo
echo "*********** BUILD FAILED --  BUILD FAILED --  BUILD FAILED  --  BUILD FAILED ***********"
echo "*********** BUILD FAILED --  BUILD FAILED --  BUILD FAILED  --  BUILD FAILED ***********"
echo
fi
echo
exit 0