#!/bin/bash

# https://wiki.wxwidgets.org/Compiling_and_getting_started
WX_VERSION=3.1.7
WX_VER_DIR=317

#build and install paths
if [ -d "../../../../Libraries" ]; then
WX_DIR=../../../../Libraries/wxWidgets-${WX_VERSION}_Linux
else
WX_DIR=~/code/wxWidgets-${WX_VERSION}_Linux
fi


#create build area
echo mkdir -p "${WX_DIR}"
mkdir -p "${WX_DIR}"
pushd    "${WX_DIR}"
pwd

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
if [ -f "makefile" ]; then
make clean
fi
../configure                    # builds unicode, shared lib
make ${BUILD_MAKE_CONCURENCY}   # use 3 cores. Set to the number of cores your have. 'make' uses 1 core
echo "**********************************************"
echo "*       BUILD COMPELTED NOW TO INSTALL       *"
echo "**********************************************"

echo Running: sudo make install and then sudo ldconfig
sudo make install               # some platforms require to use 'su' instead of 'sudo'
sudo ldconfig                   # not required in each system
# -----------------------------------------------------------------

ls -la
popd
