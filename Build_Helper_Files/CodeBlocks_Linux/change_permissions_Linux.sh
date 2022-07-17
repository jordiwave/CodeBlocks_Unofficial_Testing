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

CurrentDir=${PWD}

# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------

case "$(uname)" in
  Darwin*)
    echo "Mac OSX is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  Linux*)
    OSDetected="Linux"
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    echo "Windows is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  AIX*)
    echo "AIX is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
  *)
    echo "unknown: $OSTYPE is not supported"
    cd ${CurrentDir}
    exit 1
    ;;
esac

echo
echo "C::B CurrentDir is: " ${PWD}
echo

find . -maxdepth 1 -type f -name "*"    | xargs chmod 664 
find . -maxdepth 1 -type f -name "*.sh" | xargs chmod 764
grep -l "\/bin\/bash" * 2>/dev/null     | xargs chmod 764

if [ -d "debian" ] ; then 
    # For Linux Debian DEB build cleanup
    cd debian
    echo "Now changing permissions in: " ${PWD}
    find . -maxdepth 1 -type f -name "*"    | xargs chmod 664
    find . -maxdepth 1 -type f -name "*.sh" | xargs chmod 764
    grep -l "\/bin\/bash" * 2>/dev/null     | xargs chmod 764
    cd ..
fi

cd ${CurrentDir}