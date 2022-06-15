#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#
#                                                                                       #
#     This bash script builds the CodeBlocks installer on Windows MSYS2 Mingw64                                               #
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
FailureDetected="no"

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and to the 
    # file specified:
    exec > >(tee -i codeBlocks_Installer.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -------------------------------------------------------------------------------------------------

case "$(uname)" in
  Darwin*)
    echo "Currently: $(uname) is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  Linux*)
    echo "Currently: $(uname) is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    OSDetected="Windows"
    ;;
  AIX*)
    echo "AIX is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
  *)
    echo "unknown: ${OSTYPE} is not supported"
    cd ${CurrentDir}
    exit 2
    ;;
esac

if [ -d "..\..\src\output31_32" ] ; then export BUILD_BITS=32 ; fi
if [ -d "..\..\src\output31_64" ] ; then  export BUILD_BITS=64 ; fi
if [ -d "src\output31_32" ] ; then  export BUILD_BITS=32 ; fi
if [ -d "src\output31_64" ] ; then  export BUILD_BITS=64 ; fi
if [ "${BUILD_BITS}" == "" ] ; then
    echo "Error: Cannot find src\output31_32 or src\output31_64"
    cd ${CurrentDir}
    exit 3
fi

export WX_CB_BUILD_DIR=$(realpath ${WX_CB_BUILD_DIR})
echo
echo "+-------------------------------------------+"
echo "|  Code::Blocks Installer script            |"
echo "|                                           |"
echo "| OS:         $(uname)         |"
echo "| BUILD_BITS: ${BUILD_BITS}                 |"
echo "| PWD:        ${PWD} |"
echo "+-------------------------------------------+"
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

cd ../../windows_installer

#rem load the  NIGHTLY_BUILD_SVN variable from the txt file
#for /f "delims== tokens=1,2" %%G in (Build_Version_Number.txt) do set %%G=%%H

while IFS='=' read -r varCol valueCol
do
    if [ "${varCol}" == "NIGHTLY_BUILD_SVN" ] ; then
        NIGHTLY_BUILD_SVN=${valueCol}
    fi
done < "Build_Version_Number.txt"

echo "NIGHTLY_BUILD_SVN=${NIGHTLY_BUILD_SVN}"
echo 
if [ "${NIGHTLY_BUILD_SVN}" != "" ] ; then
    echo "execute /mingw64/bin/makensis.exe Installer_NSIS_Simple.nsi //DBUILD_TYPE=64 //DNIGHTLY_BUILD_SVN=${NIGHTLY_BUILD_SVN} "
#    /mingw64/bin/makensis.exe Installer_NSIS_UMUI.nsi /DBUILD_TYPE=64 /DNIGHTLY_BUILD_SVN=${NIGHTLY_BUILD_SVN} 
    /mingw64/bin/makensis.exe Installer_NSIS_Simple.nsi //DBUILD_TYPE=64 //DNIGHTLY_BUILD_SVN=${NIGHTLY_BUILD_SVN} 
    status=$?
    if [ $status != 0 ] ; then
        echo "${CR}${CROSS_MARK}makensis failed"
        FailureDetected="yes"
    fi
else
    echo "${CR}${CROSS_MARK}Could not get NIGHTLY_BUILD_SVN from the Build_Version_Number.txt file!"
    FailureDetected="yes"
fi

# -------------------------------------------------------------------------------------------------

if test "x${FailureDetected}" = "xyes"; then :
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
    exit 0
fi

# -------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------