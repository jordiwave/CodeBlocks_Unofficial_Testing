#!/bin/bash

# https://wiki.wxwidgets.org/Compiling_and_getting_started

#build and install paths
WX_DIR=~/code/wxwidget315

#create build area
mkdir -p "$WX_DIR"
pushd    "$WX_DIR"
pwd

wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.5/wxWidgets-3.1.5.tar.bz2
tar -xvfj wxWidgets-3.1.5.tar.bz2

#cd ~/wx/wxWidgets-3.1.3     
#mkdir gtk-build             # the name is not really relevant
#cd gtk-build
#../configure                # builds unicode, shared lib
#make -j3                    # use 3 cores. Set to the number of cores your have. 'make' uses 1 core
#sudo make install           # some platforms require to use 'su' instead of 'sudo'
#sudo ldconfig               # not required in each system
# -----------------------------------------------------------------

#configure, build & install using selected configuration 

#make clean
#make
#make install
ls -la
popd
