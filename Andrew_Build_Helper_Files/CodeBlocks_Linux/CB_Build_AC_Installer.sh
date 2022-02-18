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
# https://wiki.codeblocks.org/index.php/Installing_Code::Blocks_from_source_on_Linux
#   -> WX 2.8
# https://wiki.codeblocks.org/index.php/Installing_Code::Blocks_from_source_on_Arch_Linux

# -----------------------------------------------------------------------------

# BAT / CMD goto function
function goto
{
    label=$1
    cmd=$(sed -n "/^:[[:blank:]][[:blank:]]*${label}/{:a;n;p;ba};" $0 |
          grep -v ':$')
    eval "$cmd"
    exit
}

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

# -----------------------------------------------------------------------------

#path to wxWidgets configuration scripts

if [ -e /usr/bin/wx-config ]; then
    export WX_CONFIG_DIR=/usr/bin
else
    if [ -e /usr/local/lib/wx/config/wx_config ]; then
        export WX_CONFIG_DIR=/usr/local/lib/wx/config
    else
        echo Cannot detect where wx-config is. Please update this shell script and try again.
        exit 2
    fi
fi


#actual wxWidgets configuration to use, to see alternatives use: 
#  find /usr/local/lib/wx/config/*
#    or
#  find /usr/lib/x86_64-linux-gnu/wx/config/*
#    or
# wx-config --list
WX_CONFIG=gtk3-unicode-3.0
WX_CONFIG_FULLPATH="$WX_CONFIG_DIR/$WX_CONFIG"

#build and install paths
CB_DIR=~/code/codeblocks
CB_BUILD_PATH="$CB_DIR/$WX_CONFIG/build"
CB_INSTALL_PATH="$CB_DIR/$WX_CONFIG/install"

#create build area
#mkdir -p "$CB_BUILD_PATH"
#pushd    "$CB_BUILD_PATH"
#pwd

#if [ ! -d "SF_SVN_CodeBlocks" ]; then
#    svn checkout https://svn.code.sf.net/p/codeblocks/code/trunk SF_SVN_CodeBlocks
#else
#    cd SF_SVN_CodeBlocks
#fi

#CB_BUILD_DIR=CB_SVN_trunk
CB_BUILD_DIR=AC-WindowsInstaller
if [ ! -d "$CB_BUILD_DIR" ]; then
    # git clone -b AC-WindowsInstaller https://github.com/acotty/codeblocks_sf AC-WindowsInstaller
    echo The $CB_BUILD_DIR directiory does not exist. Please fix and try again
    exit 3
fi
echo Current directory: $PWD
echo CB_INSTALL_PATH: $CB_INSTALL_PATH

cd "$CB_BUILD_DIR"

# -----------------------------------------------------------------------------

#configure, build & install using selected configuration 
./bootstrap
./configure --with-contrib-plugins=all             \
            --prefix="$CB_INSTALL_PATH"
#./configure --with-contrib-plugins=all             \
#            --with-wx-config="$WX_CONFIG_FULLPATH" \
#            --prefix="$CB_INSTALL_PATH"

# -----------------------------------------------------------------------------

make
#make install

cd src
./Unix_update30.sh
cd ..

# -----------------------------------------------------------------------------

#ls -l "$CB_INSTALL_PATH/bin"
#popd

# -----------------------------------------------------------------------------

#:finished