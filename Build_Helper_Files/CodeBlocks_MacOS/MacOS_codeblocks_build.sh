#!/bin/bash

reset
echo MacOS - build CODE::BLOCKS and WxWidgets if needed

DEBUG_SCRIPT="Yes"

#WX_VERSION=3.1.7
#WX_GITHUB_TAG=3.1.7
#WX_DIR_VERSION=31
#WX_OSX_LIB_DIR_VERSION=3.1

WX_VERSION=3.2.0
WX_GITHUB_TAG=3.2.0
WX_DIR_VERSION=32
WX_OSX_LIB_DIR_VERSION=3.2

MAC_OSX_MIN_VERSION=11.6
#MAC_OSX_MIN_VERSION=10.15

InitialDir=${PWD}
failureDetected="no"

export CXX=clang++
export CC=clang

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    reset
    # The following is to enable sending the output of this script to the terminal and 
    # to the file specified:
    exec > >(tee -i CodeBlocks_MAC_${WX_VERSION}_build.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -----------------------------------------------------------------------------

CHECK_MARK="\033[2K\033[0;32m\xE2\x9C\x94\033[0m"
CROSS_MARK="\033[2K\033[0;31m\xE2\x9C\x98\033[0m"
# 0 reset, 1 bold, 32 green
GREEN_START="\033[0;1;32m"
# 0 reset, 1 bold, 5 block slow, 31 red
RED_START="\033[0;1;5;31m"
COLOR_REVERT="\033[0m"

# -----------------------------------------------------------------------------

function setup_variables()
{
    cd "${InitialDir}"
    if [ -f bootstrap ]; then
        CB_ROOT_DIR=${PWD}
        cd ..
        BUILD_ROOT_DIR=${PWD}
    else
        if [ -f ../bootstrap ]; then
            cd ..
            CB_ROOT_DIR=${PWD}
            cd ..
            BUILD_ROOT_DIR=${PWD}
        else
            if [ -f ../../bootstrap ]; then
                cd ../..
                CB_ROOT_DIR=${PWD}
                cd ..
                BUILD_ROOT_DIR=${PWD}
            else
                BUILD_ROOT_DIR=${PWD}
            fi
        fi
    fi
    
    cd "${BUILD_ROOT_DIR}"
    if [ -d ./wxwidgets-code ]; then
        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}"
    fi

    OUT_WX="${BUILD_ROOT_DIR}/bin/wxwidgets"
    OUT_CB="${BUILD_ROOT_DIR}/bin/codeblocks"

    if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
        echo WX_ROOT_DIR = ${WX_ROOT_DIR}
        echo OUT_WX = ${OUT_WX}
        echo OUT_CB = ${OUT_CB}
    fi

    cd ${BUILD_ROOT_DIR}
}

# Specify here the version of wxWidgets. It must be tagged.
# see https://github.com/wxWidgets/wxWidgets.git
function install_dependencies()
{
    if [ ! -d /Applications/Xcode.app ]; then
        xcode-select --install
        xcodebuild -license
    fi

    if [ ! -f /usr/local/bin/brew ]; then
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    if [ ! -d /usr/local/Cellarn/autoconf ]; then
        brew install autoconf
    fi
    
    if [ ! -d /usr/local/Cellarn/automake ]; then
        brew install automake
    fi

    if [ ! -d /usr/local/Cellar/libtool ]; then
        brew install libtool
    fi

    if [ ! -d /usr/local/Cellar/boost ]; then
        brew install boost
    fi

    if [ ! -d /usr/local/Cellar/squirrel ]; then
        brew install squirrel
    fi
    
    if [ ! -d /usr/local/Cellar/hunspell ]; then
        brew install hunspell
    fi
        
    if [ ! -d /usr/local/Cellar/gtk+3 ]; then
        brew install gtk+3
    fi
    
    if [ ! -d /usr/local/Cellar/coreutils ]; then
        brew install coreutils
    fi

    if [ ! -d /usr/local/Cellar/llvm ]; then
        brew install llvm
    fi
    
    if [ ! -d /usr/local/Cellar/create-dmg ]; then
        brew install create-dmg
    fi

    if [ "${GITHUB_ACTIONS}" == "" ] ; then
        if [ ! -f /usr/local/bin/gdb ]; then
            brew install gdb
        fi
        if [ ! -f /usr/bin/svn ]; then
            brew install svn
        fi
        if [ ! -f /usr/bin/git ]; then
            brew install git
        fi
        
        if [ ! -d /usr/local/Cellar/zlib ]; then
            brew install zlib
        fi
        
        if [ ! -d /usr/local/Cellar/bzip2 ]; then
            brew install bzip2
        fi
    
        if [ ! -d /usr/local/Cellar/gh ]; then
            brew install gh
        fi
    fi
}

function install_extra_apps()
{
    if [ ! -d "/Applications/Double Commander.app" ]; then
        brew install double-commander
    fi

    if [ ! -d "/Applications/Visual Studio Code.app" ]; then
        brew install visual-studio-code
    fi

    if [ ! -d "/Applications/CCleaner.app" ]; then
        brew install ccleaner
    fi
    
    if [ ! -d "/Applications/Meld.app" ]; then
        brew install --cask meld
    fi
}

function wxwidgets_build()
{
    buildWxWidgets=No

    cd "${BUILD_ROOT_DIR}"
    if [ ! -d ./wxwidgets-code ]; then
        echo "+------------------------------------------------------------+"
        echo "| Clone https://github.com/wxWidgets/wxWidgets.git "v${WX_GITHUB_TAG}" |"
        echo "+------------------------------------------------------------+"
        git clone https://github.com/wxWidgets/wxWidgets.git --branch "v${WX_GITHUB_TAG}" --single-branch --recurse-submodules wxwidgets-code_${WX_VERSION}
        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}"
        buildWxWidgets=Yes
    else
        if [ "${GITHUB_ACTIONS}" == "true" ] ; then
            buildWxWidgets=Yes
        else
            read -r -p 'Would you like to re-compile wxWidgets? ( type "y" for yes or "n" for no ): '
            if [ "${REPLY}" == 'y' ] || [ "${REPLY}" == 'Y' ]; then
                buildWxWidgets=Yes
            fi
        fi
    fi

    if [ "${buildWxWidgets}" == "Yes" ]; then

        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code_${WX_VERSION}"

        # https://wiki.wxwidgets.org/Possible_Configure_Flags_under_OS_X
        if [ -d "${OUT_WX}" ]; then
            rm -r -v -f "${OUT_WX}"
        fi

        mkdir -p "${OUT_WX}"
        cd "${OUT_WX}"

        echo "+--------------------------------------------------+"
        echo "|  configure wxWidgets in  ${OUT_WX} |"
        echo "+--------------------------------------------------+"

        # https://wiki.wxwidgets.org/Possible_Configure_Flags_under_OS_X
        "${WX_ROOT_DIR}/configure"  --with-osx-cocoa --with-macosx-version-min=${MAC_OSX_MIN_VERSION} \
                                    --disable-debug --disable-debug-flag --enable-unicode --enable-cxx11 \
                                    --with-opengl --with-expat=builtin --with-libjpeg=builtin --with-libpng=builtin \
                                    --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin \
            # "${WX_ROOT_DIR}/configure" 
            # --with-osx-cocoa
                # --with-macosx-version-min=${MAC_OSX_MIN_VERSION}  aka MAC_OSX_MIN_VERSION=11.6 or MAC_OSX_MIN_VERSION=10.15
            # --disable-debug
            # --disable-debug-flag
            # --enable-unicode
            # --enable-cxx11
            # --with-opengl
            # --with-expat=builtin
            # --with-libjpeg=builtin
            # --with-libpng=builtin
            # --with-regex=builtin
            # --with-libtiff=builtin
                #--with-zlib=builtin

        status=$?
        if [ $status == 0 ] ; then
            echo "+--------------------------------------------------+"
            echo "|              make wxWidgets                      |"
            echo "+--------------------------------------------------+"
            nice make -j $(($(nproc) -1))
        else
            echo "+--------------------------------------------------+"
            echo "|        configure wxWidgets  FAILED               |"
            echo "+--------------------------------------------------+"
            failureDetected="yes"
            return 1
        fi
    fi

    if [ -f "${OUT_WX}/lib/libwx_osx_cocoau_richtext-${WX_VERSION}.0.0.dylib" ]; then
        echo "+--------------------------------------------------+"
        echo "|               install wxWidgets                  |"
        echo "+--------------------------------------------------+"
        cd "${OUT_WX}"
        make install
        status=$?
        if [ $status == 0 ] ; then
            return 0
        else
            echo "+--------------------------------------------------+"
            echo "|        install wxWidgets  FAILED               |"
            echo "+--------------------------------------------------+"
            failureDetected="yes"
            return 1
        fi
    else
        ls -la "${OUT_WX}"
        ls -la "${OUT_WX}/lib"
        ls -la "${OUT_WX}/lib/libwx_osx_cocoau_richtext*.dylib"
        echo "+--------------------------------------------------+"
        echo "|              make wxWidgets  FAILED              |"
        echo "+--------------------------------------------------+"
        failureDetected="yes"
        return 1
    fi
}

function codeblocks_build()
{
    cd "${BUILD_ROOT_DIR}"

    if [ -d "${OUT_CB}" ]; then
        rm -r -v -f "${OUT_CB}"
    fi
    mkdir -p "${OUT_CB}"
        
    cd "${CB_ROOT_DIR}"

    # To make sure you start again from scratch. You can do that : ?cd codeblock-code | svn cleanup --remove-unversioned
    # before to launch the script.
    if [ -f ./configure ]; then
        if [ "${GITHUB_ACTIONS}" == "" ] ; then
            make clean
            make distclean
            make clean-bin
            make clean-zipfiles
            rm ./configure
            rm ./makefile
        fi
    fi

    echo "+--------------------------------------------------+"
    echo "|   CodeBLocks:  bootstrap                         |"
    echo "+--------------------------------------------------+"
    chmod +x ./bootstrap *.sh
    ./bootstrap
    status=$?
    if [ $status == 0 ] ; then
        echo "+--------------------------------------------------+"
        echo "|   CodeBLocks:  configure                         |"
        echo "+--------------------------------------------------+"
        chmod +x configure *.sh
        
        if [ -f ./configure ]; then
            ./configure CXXFLAGS=-mmacosx-version-min=${MAC_OSX_MIN_VERSION} CC=clang CXX=clang++ \
                        --disable-pch --prefix=${CB_ROOT_DIR}/src/devel${WX_DIR_VERSION} \
                        --with-contrib-plugins=all,-FileManager
                #./configure 
                #               CXXFLAGS=-mmacosx-version-min=${MAC_OSX_MIN_VERSION}  aka MAC_OSX_MIN_VERSION=11.6 or MAC_OSX_MIN_VERSION=10.15
                #               CC=clang
                #               CXX=clang++
				#		        --disable-pch
                #               --prefix=${CB_ROOT_DIR}/src/devel${WX_DIR_VERSION}
                #               --with-contrib-plugins=all,-FileManager
                #
                # Xaviou changes/additions
				#		        --with-wx-config=/Users/username/dev/wx316/build-macOS-11.6/wx-config
				#		        --prefix=/Users/username/dev/CB/build-macOS-11.6/output

            status=$?
            if [ $status != 0 ] ; then
                echo "+----------------------------------------+"
                echo "| ERROR: CodeBLocks configure failed!!!  |"
                echo "+----------------------------------------+"
                return -1
            fi
        else
            echo "+-------------------------------------------------------------+"
            echo "| ERROR: CodeBLocks bootstrap failed (no configure output)!!! |"
            echo "+-------------------------------------------------------------+"
            return -1
        fi
    else
        echo "+--------------------------------------------------+"
        echo "| ERROR: CodeBLocks bootstrap failed!!!            |"
        echo "+--------------------------------------------------+"
        return 1
    fi
    
    if [ -f ./makefile ]; then
        echo "+--------------------------------------------------+"
        echo "|   CodeBLocks:  make                              |"
        echo "+--------------------------------------------------+"
        make -j $(($(nproc) -1))
        status=$?
        if [ $status != 0 ] ; then
            echo "+-----------------------------------+"
            echo "| ERROR: CodeBLocks make failed!!!  |"
            echo "+-----------------------------------+"
            return -1
        fi

        echo "+----------------------------------------+"
        echo "|   CodeBLocks:  make install            |"
        echo "+----------------------------------------+"
        make install
        status=$?
        if [ $status != 0 ] ; then
            echo "+------------------------------------------+"
            echo "| ERROR: CodeBLocks make install failed!!! |"
            echo "+------------------------------------------+"
            return -1
        fi
        cp -R ./CodeBlocks.app /Applications

        echo "+--------------------------------------------------+"
        echo "|   CodeBLocks:  bundle                            |"
        echo "+--------------------------------------------------+"
        ./bundle.sh
        status=$?
        if [ $status != 0 ] ; then
            echo "+----------------------------------------+"
            echo "| ERROR: CodeBLocks bundle.sh failed!!!  |"
            echo "+----------------------------------------+"
            return -1
        fi
        
        echo "+--------------------------------------------------+"
        echo "|   CodeBLocks:  create DMG installer              |"
        echo "+--------------------------------------------------+"
        if [ -f ./CB_create_dmg.sh ] ; then
            ./CB_create_dmg.sh
        else
            create-dmg CodeBlocks-Installer.dmg CodeBlocks.app
        fi
        status=$?
        if [ $status != 0 ] ; then
            echo "+----------------------------------------+"
            echo "| ERROR: CodeBLocks create-dmg failed!!! |"
            echo "+----------------------------------------+"
            return -1
        fi

    else
        echo "+-------------------------------------------+"
        echo "| ERROR: Configure failed (no makefile)!!!! |"
        echo "+-------------------------------------------+"
        return -2
    fi

    return 0
}

function check_wxwidgets_build()
{
    if [ ! -d "/usr/local/lib/wx/include/osx_cocoa-unicode-${WX_OSX_LIB_DIR_VERSION}" ]; then
        if  [ "${WX_ROOT_DIR}" = "" ] || [ ! -f "${OUT_WX}/lib/libwx_osx_cocoau_richtext-${WX_VERSION}.0.0.dylib" ]; then
            if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
                if [ "${WX_ROOT_DIR}" = "" ]; then
                    echo "WX_ROOT_DIR is empty: ${WX_ROOT_DIR}"
                fi
                if [ ! -f "${OUT_WX}/lib/libwx_osx_cocoau_richtext-${WX_VERSION}.0.0.dylib" ]; then
                    echo "Cannot find: ${OUT_WX}/lib/libwx_osx_cocoau_richtext-${WX_VERSION}.0.0.dylib"
                fi
            fi
            wxwidgets_build
            status=$?
            if [ $status != 0 ] ; then
                failureDetected="yes"
                if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
                    echo "wxwidgets_build failure detected."
                fi
            fi
        else
            echo "You already have already built wxWidgets ${WX_VERSION}"
        fi
    else
        echo "You already have wxWidgets ${WX_OSX_LIB_DIR_VERSION} installed"
    fi
}

cd "${InitialDir}"

# Un-comment the following to install build dependencies:
if [ "${GITHUB_ACTIONS}" == "true" ] ; then
    install_dependencies
fi

# Un-comment the following to install extra apps to help development:
#install_extra_apps

# setup variables based on directories found. Variables used in building wxWidgets and Code::Blocks
setup_variables

#build wxwidgets if it is not allready installed
check_wxwidgets_build

if test "x${failureDetected}" = "xno"; then :
    cd "${BUILD_ROOT_DIR}"
    if [ "${CB_ROOT_DIR}" = "" ] || [ ! -f "${OUT_CB}/bin/codeblocks" ]; then
        if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
            if [ "${CB_ROOT_DIR}" = "" ]; then
                echo "CB_ROOT_DIR is empty: ${CB_ROOT_DIR}"
            fi
            if [ ! -f "${OUT_CB}/bin/codeblocks" ]; then
                echo "Cannot find: ${OUT_CB}/bin/codeblocks"
            fi
        fi
        codeblocks_build
        status=$?
        if [ $status != 0 ] ; then
            failureDetected="yes"
            if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
                echo "codeblocks_build failure detected."
            fi
        fi
    else
        echo "Code::Blocks already built."

        cd "${CB_ROOT_DIR}"
        if [ "${GITHUB_ACTIONS}" == "true" ] ; then
            REPLY=Y
        else
            read -r -p 'Would you like to build/rebuild Codeblocks? ( type "y" for yes or "n" for no ): '
        fi
        if [ "${REPLY}" == "y" ] || [ "${REPLY}" == "Y" ]; then
            codeblocks_build
            status=$?
            if [ $status != 0 ] ; then
                failureDetected="yes"
                if [ "${DEBUG_SCRIPT}" == "Yes" ]; then
                    echo "codeblocks_build failure detected."
                fi
            fi
        fi
    fi
fi

if test "x${failureDetected}" = "xyes"; then :
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
    echo -e "\\r${COLOR_REVERT}"
    exit 999
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
    echo -e "\\r${COLOR_REVERT}"
    exit 0
fi
