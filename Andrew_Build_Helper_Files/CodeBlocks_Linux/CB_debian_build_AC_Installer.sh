#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------
# To enable sending the output of this script to the terminal and to the file specified uncomment the following line:
exec > >(tee -i Build_CB_Output.log) 2>&1
# NOTE: if you want to append to the file change the -i to -ia in the line above.

# -----------------------------------------------------------------------------

# source: https://github.com/arnholm/cpde_3rdparty/blob/master/gcc/codeblocks/build_cb.sh
# discussion: https://forums.codeblocks.org/index.php/topic,24628.msg168076.html#msg168076
#
# Wiki Pages:
# https://forums.codeblocks.org/index.php/topic,23689.msg161532.html#msg161532
#
#Files:
# travis.yml

# -----------------------------------------------------------------------------

#if [ ! -d "SF_SVN_CodeBlocks" ]; then
#    svn checkout https://svn.code.sf.net/p/codeblocks/code/trunk SF_SVN_CodeBlocks
#fi
# cd SF_SVN_CodeBlocks

#CB_BUILD_DIR=CB_SVN_trunk
CB_BUILD_DIR=AC-WindowsInstaller
if [ ! -d "$CB_BUILD_DIR" ]; then
    # git clone -b AC-WindowsInstaller https://github.com/acotty/codeblocks_sf AC-WindowsInstaller
    echo The $CB_BUILD_DIR directiory does not exist. Please fix and try again
    exit 3
fi
cd "$CB_BUILD_DIR"

# -----------------------------------------------------------------------------

if [ -e "debain/control" ]; then
    dpkg-buildpackage --target=clean
fi
if [ -e "Makefile" ]; then
    #if [ -d "CB_INSTALL_PATH" ]; then
    #    make uninstall &>/dev/null
    #fi
    make clean &>/dev/null
fi
if [ -d "autom4te.cache" ]; then
    rm -rf autom4te.cache
fi

# Manually update to the latest ChangeLog:
# ./updateChangeLog.sh

# Configure, build & install using selected configuration 
./bootstrap

# To configure for building the debian deb packages run the following script:
./debian/setup_control.sh

# build C::B and debian files using selected configuration 
dpkg-buildpackage -us -uc

# -----------------------------------------------------------------------------
