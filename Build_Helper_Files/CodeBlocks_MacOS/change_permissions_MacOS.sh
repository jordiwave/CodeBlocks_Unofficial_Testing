#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#
#                                                                                       #
#  This bash script changes the permissions of files copied from Windows to Linux       #
#                                                                                       #
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal non root user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

CurrentDir=$PWD

# -----------------------------------------------------------------------------

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

case "$(uname)" in
  Darwin*)
    OSDetected="Darwin"
    ;;
  Linux*)
    OSDetected="Linux"
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    echo "Windows is not supported"
    cd $CurrentDir
    exit 0
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

echo
echo "CurrentDir C::B root is: " $PWD
echo

find . -type f -name "*.sh"    | xargs chmod +rwx
chmod +rwx bootstrap
chmod +rwx wx-config-cb-win64