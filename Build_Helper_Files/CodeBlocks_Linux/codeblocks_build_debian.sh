#!/bin/bash

# ------------------------------------------------------------------------------------------#
#                                                                                           #
# This file is part of the Code::Blocks IDE and licensed under the                          #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html          #
#                                                                                           #
# ------------------------------------------------------------------------------------------#
#                                                                                           #
#     This bash script builds CodeBlocks on Linux to create the Debian install files        #
#                                                                                           #
# MS WSL2 NOTES:                                                                            #
# 1. By default you cannot build the Debian files on MS WSL2 on a NTFS file system          #
#      as the permissions on files cannot be changed, so the build will fail.               #
#                                                                                           # 
# 2. To fix the permission issue do the following:                                          #
#                                                                                           #
#    a) Create an /etc/wsl.conf file (as sudo) with the following contents:                 #
#                                                                                           #
#    [automount]                                                                            #
#    options="metadata"                                                                     #
#                                                                                           #
#    [automount]                                                                            #
#    enabled=true                                                                           #
#    options=metadata,uid=1000,gid=1000,umask=022                                           #
#                                                                                           #
#    b) Exit WSL                                                                            #
#                                                                                           #
#    c) Terminate the instance (wsl --terminate <distroname>)                               #
#        or                                                                                 #
#       shut it down (wsl --shutdown)                                                       #
#                                                                                           #
#    d) Restart WSL                                                                         #
#                                                                                           #
#    e) A hack to get building on MS WSL2 is to modify the                                  #
#        U:\usr\share\perl5\Dpkg\Source\Package\V3\Native.pm file to the following near     #
#         the end of the file:                                                              #
#                pop_exit_handler();                                                        #
#                if (defined $ENV{WSL_DISTRO_NAME}) {                                       #
#                    info(g_('ENV{WSL_DISTRO_NAME}: %s'), $ENV{WSL_DISTRO_NAME});           #
#                    chmod(0666 &~ umask(), $tarname);                                      #
#                }                                                                          #
#                else {                                                                     #
#                    chmod(0666 &~ umask(), $tarname)                                       #
#                        or syserr(g_("unable to change permission of '%s'"), $tarname);    #
#                }                                                                          #
# ------------------------------------------------------------------------------------------#
# Resources used to create this script:                                                     #
# source: https://github.com/arnholm/cpde_3rdparty/blob/master/gcc/codeblocks/build_cb.sh   #
# discussion: https://forums.codeblocks.org/index.php/topic,24628.msg168076.html#msg168076  #
# Wiki Pages: https://forums.codeblocks.org/index.php/topic,23689.msg161532.html#msg161532  #
# ------------------------------------------------------------------------------------------#

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

reset
echo
echo "+-----------------------------------------------------+"
echo "|     Code::Blocks build script running on " $(uname) "    |"
echo "+-----------------------------------------------------+"
echo

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
    exit 3
    ;;
  Linux*)
    OSDetected="Linux"
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    echo "Windows is not supported"
    cd ${CurrentDir}
    exit 3
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
    echo "unknown: $OSTYPE is not supported"
    cd ${CurrentDir}
    exit 3
    ;;
esac

echo "CurrentDir C::B root is: " ${PWD}
echo

# --------------------------------------------------------------------------------------
# The following is to enable sending the output of this script to the terminal and to the 
# file specified:
# exec > >(tee -i Build_CB_Output.log) 2>&1
# NOTE: if you want to append to the file change the -i to -ia in the line above.
# --------------------------------------------------------------------------------------

CHECK_MARK="\033[2K\033[0;32m\xE2\x9C\x94\033[0m"
CROSS_MARK="\033[2K\033[0;31m\xE2\x9C\x98\033[0m"
# 0 reset, 1 bold, 32 green
GREEN_START="\033[0;1;32m"
# 0 reset, 1 bold, 5 block slow, 31 red
RED_START="\033[0;1;5;31m"
COLOR_REVERT="\033[0m"

# -----------------------------------------------------------------------------

start_datetime=$(date +%s)
FailureDetected="no"
echo "Start at                                    : "$(date +"%d-%b-%Y %T")

# -----------------------------------------------------------------------------

if [ -e "debain/control" ]; then
    echo "running ./dpkg-buildpackage --target=clean"
    ./dpkg-buildpackage --target=clean
fi
if [ -e "Makefile" ]; then
    #if [ -d "CB_INSTALL_PATH" ]; then
    #    make uninstall &>/dev/null
    #fi
    echo "running make clean"
    make clean &>/dev/null
fi
if [ -d "autom4te.cache" ]; then
    rm -rf autom4te.cache
fi

# -----------------------------------------------------------------------------

# Configure, build & install using selected configuration 
echo -n "running ./update_revision.sh"
beforeStepStartTime=$(date +%s)
if ./update_revision.sh > z_update_revision_result.txt 2>&1 ; then
    durationTotalTime=$(($(date +%s)-start_datetime))
    stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
    echo -e "\\r${CHECK_MARK}./update_revision.sh finish at            : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

    echo -n "running ./boostrap"
    beforeStepStartTime=$(date +%s)
    if ./bootstrap > z_bootstrap_result.txt 2>&1 ; then

        durationTotalTime=$(($(date +%s)-start_datetime))
        stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
        echo -e "\\r${CHECK_MARK}./Bootstrap finish at                     : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

        # To configure for building the debian deb packages run the following script:
        echo -n "running ./debian/setup_control.sh"
        beforeStepStartTime=$(date +%s)
        if ./debian/setup_control.sh > z_setup_control_result.txt 2>&1 ; then

            durationTotalTime=$(($(date +%s)-start_datetime))
            stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
            echo -e "\\r${CHECK_MARK}./debian/setup_control.sh.... finish at   : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")

            # build C::B and debian files using selected configuration 
            echo -n "running dpkg-buildpackage -us -uc"
            beforeStepStartTime=$(date +%s)
            if dpkg-buildpackage -us -uc > z_dpkg-buildpackage_result.txt 2>&1 ; then

                durationTotalTime=$(($(date +%s)-start_datetime))
                stepDeltaTime=$(($(date +%s)-beforeStepStartTime))
                echo -e "\\r${CHECK_MARK}dpkg-buildpackage -us -uc.... finish at : "$(date +"%d-%b-%Y %T")" , delta : "$(date -d "1970-01-01 + $stepDeltaTime seconds" "+%H:%M:%S")" ,    duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")
            else
                echo -e "\\r${CROSS_MARK}dpkg-buildpackage -us -uc failed"
                FailureDetected="yes"
            fi
        else
            echo -e "\\r${CROSS_MARK}./debian/setup_control.sh failed"
            FailureDetected="yes"
        fi
    else
        echo -e "\\r${CROSS_MARK}Bootstrap failed"
        FailureDetected="yes"
    fi
else
    echo -e "\\r${CROSS_MARK}./update_revision.sh failed"
    FailureDetected="yes"
fi

echo "Finished at                                : "$(date +"%d-%b-%Y %T")
echo

# -----------------------------------------------------------------------------
durationTotalTime=$(($(date +%s)-start_datetime))
echo "+-----------------------------------------------------------+"
echo "|                                                           |"
echo "| Finish at  : "$(date +"%d-%b-%Y %T")" ,  duration : "$(date -d "1970-01-01 + $durationTotalTime seconds" "+%H:%M:%S")"  |"
echo "|                                                           |"
echo "+-----------------------------------------------------------+"
echo

if test "x${FailureDetected}" = "xyes" ; then
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
    echo -e "\\r${COLOR_REVERT} "
    cd ${CurrentDir}
    exit 4
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
    echo -e "\\r${COLOR_REVERT} "
    cd ${CurrentDir}
    exit 0
fi

