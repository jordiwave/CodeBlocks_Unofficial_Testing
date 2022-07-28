#!/bin/bash

# --------------------------------------------------------------------------------------#
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
# --------------------------------------------------------------------------------------#
# https://wiki.wxwidgets.org/Compiling_and_getting_started
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Set build variables
# ----------------------------------------------------------------------------
#DEBUG_SCRIPT="No"
DEBUG_SCRIPT="Yes"
WX_VERSION=3.1.7
WX_VER_DIR=317
WX_PREFIX_INSTALL=/opt/wx
WX_DEB_OUTPUT_NAME=wxwidgetsTrunk

# save for later use
StartDir=${PWD}

# ----------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i wxWidget_${WX_VERSION}_$(uname)_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.

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


function build_failed()
{
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
    cd ${StartDir}
    exit 7
}

function build_pass()
{
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
}
# -----------------------------------------------------------------


#build and install paths
if [ -d "../../../../Libraries" ]; then
    WX_DIR=../../../../Libraries/wxWidgets-${WX_VERSION}_Linux
else
    if [ -d "WX_DIR=~/code" ]; then
        WX_DIR=~/code/wxWidgets-${WX_VERSION}_Linux
    else
        WX_DIR=~/Code/wxWidgets-${WX_VERSION}_Linux
    fi
    if [[ ! -d ${WX_DIR} ]] && [[ "${WSL_DISTRO_NAME}" == "" ]] ; then
        mkdir -p ${WX_DIR}
    fi
fi

if [ ! -d ${WX_DIR} ] ; then
    echo "Cannot find ${WX_DIR} directory"
    build_failed
fi

#create build area
cd ${WX_DIR}

if [ ! -f "wxWidgets-${WX_VERSION}.tar.bz2" ]; then
    wget https://github.com/wxWidgets/wxWidgets/releases/download/v${WX_VERSION}/wxWidgets-${WX_VERSION}.tar.bz2
fi

if [ ! -f "CMakeLists.txt" ]; then
    tar --strip-components=1 -xvf wxWidgets-${WX_VERSION}.tar.bz2
fi

# -----------------------------------------------------------------
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

# -----------------------------------------------------------------

if [ ! -d "gtk-build" ]; then
    mkdir gtk-build                 # the name is not really relevant
fi

cd gtk-build

# -----------------------------------------------------------------

if [ -f "Makefile" ]; then
    echo "Found Makefile, so perform a 'make clean'"
    sudo make clean 1>/dev/null
fi

# -----------------------------------------------------------------

if [ "${WX_PREFIX_INSTALL}" == "" ] ; then
    echo "run: ../configure  1>/dev/null"
    ../configure  1>/dev/null                   # builds unicode, shared lib
else
    echo "run:  ../configure  --prefix=${WX_PREFIX_INSTALL}  1>/dev/null"
    if [ ! -d "${WX_PREFIX_INSTALL}" ] ; then
        sudo mkdir -p ${WX_PREFIX_INSTALL}
    fi
    ../configure --prefix=${WX_PREFIX_INSTALL} 1>/dev/null # builds unicode, shared lib. Install in /opt instead of /usr in order not to collide with Ubuntu package
fi
status=$?
if [ $status != 0 ] ; then
    build_failed
fi

# -----------------------------------------------------------------

echo "run: make ${BUILD_MAKE_CONCURENCY}  1>/dev/null"
make ${BUILD_MAKE_CONCURENCY}  1>/dev/null
status=$?
if [ $status != 0 ] ; then
    build_failed
fi

# -----------------------------------------------------------------

LIB_SO_COUNT=$(ls -1q ./lib/libwx*.so | wc -l)
if [ ${LIB_SO_COUNT} -eq 14 ] ; then
    echo "**********************************************"
    echo "*       BUILD COMPELTED NOW TO INSTALL       *"
    echo "**********************************************"
else
    echo ${ECHO_E} "CB wxWidget detected .SO count ${LIB_SO_COUNT} should have been 14!!!"
    echo ${ECHO_E} "  See ./lib/libwx*.so"
    ls -la ./lib/libwx*.so
    build_failed
fi

# -----------------------------------------------------------------

# need to use checkinstall so that wxWidgets is installed as a deb package so it can be removed or used in the debian creation steos
echo "run: sudo checkinstall --pkgname=${WX_DEB_OUTPUT_NAME} --pkgversion=${WX_VERSION} --default make install"
sudo checkinstall --pkgname=${WX_DEB_OUTPUT_NAME} --pkgversion=${WX_VERSION} --default make install
status=$?
if [ $status != 0 ] ; then
    build_failed
fi
# check for deb package is installed: dpkg -l | grep wxwidgets  | awk '{print $2}'
# check which package a file belongs to : dpkg -S <filename>


# -----------------------------------------------------------------

echo "run: sudo ldconfig"
sudo ldconfig                   # not required in each system

# -----------------------------------------------------------------

if [ -f "/usr/local/lib/wx/config/gtk3-unicode-3.1" ] ; then
    echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.1 310"
    sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.1 310
else
    if [ -f "${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.1" ] ; then
        echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config ${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.1 310"
        sudo update-alternatives --install /usr/bin/wx-config wx-config ${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.1 310
    fi
fi

if [ -f "/usr/local/lib/wx/config/gtk3-unicode-3.2" ] ; then
    echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.2 320"
    sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.2 320
else
    if [ -f "${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.2" ] ; then
        echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config ${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.2 320"
        sudo update-alternatives --install /usr/bin/wx-config wx-config ${WX_PREFIX_INSTALL}/lib/wx/config/gtk3-unicode-3.2 320
    fi
fi
echo "Use 'sudo update-alternatives --config wx-config' to change from wx3.2 to wx3.0 or wx3.1"
update-alternatives --list  wx-config

# -----------------------------------------------------------------

build_pass
cd ${StartDir}
