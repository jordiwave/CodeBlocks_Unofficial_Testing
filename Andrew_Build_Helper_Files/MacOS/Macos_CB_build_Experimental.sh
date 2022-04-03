# Absolute path to this script, e.g. /home/user/bin/foo.sh
# https://stackoverflow.com/questions/1055671/how-can-i-get-the-behavior-of-gnus-readlink-f-on-a-mac

WX_TAG="v3.1.5"
SCRIPT=$(greadlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
InitialDir=$PWD

export CXX=clang++
export CC=clang

function setup_variables()
{
    cd "${SCRIPTPATH}"
    if [ -f bootstrap ]; then
        CB_ROOT_DIR=$PWD
        cd ..
        BUILD_ROOT_DIR=$PWD
    else
        if [ -f ../../bootstrap ]; then
            cd ../..
            CB_ROOT_DIR=$PWD
            cd ..
            BUILD_ROOT_DIR=$PWD
        else
            BUILD_ROOT_DIR=$PWD
        fi
    fi
    
    cd "${BUILD_ROOT_DIR}"
    if [ -d ./wxwidgets-code ]; then
        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code"
    fi

    OUT_WX="${BUILD_ROOT_DIR}/bin/wxwidgets"
    OUT_CB="${BUILD_ROOT_DIR}/bin/codeblocks"

    cd ${BUILD_ROOT_DIR}
}

# Specify here the version of wxWidgets. It must be tagged.
# \see https://github.com/wxWidgets/wxWidgets.git
function install_dependencies()
{
    if [ ! -d /Applications/Xcode.app ]; then
        xcode-select --install
        xcodebuild -license
    fi

    if [ ! -f /usr/local/bin/brew ]; then
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    if [ ! -f /usr/local/bin/gdb ]; then
        brew install gdb
    fi

    if [ ! -f /usr/bin/svn ]; then
        brew install svn
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

    if [ ! -f /usr/bin/git ]; then
        brew install git
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
    
    if [ ! -d /usr/local/Cellar/zlib ]; then
        brew install zlib
    fi
    
    if [ ! -d /usr/local/Cellar/bzip2 ]; then
        brew install bzip2
    fi
    
    if [ ! -d /usr/local/Cellar/gh ]; then
        brew install gh
    fi


    ##echo "macports update."
    ## it can take time
    # sudo port selfupdate
    ##echo "libgcc installation."
    #sudo port install libgcc
    ##echo "cctools installation."
    #sudo port install cctools
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
        git clone https://github.com/wxWidgets/wxWidgets.git --branch "$WX_TAG" --single-branch --recurse-submodules wxwidgets-code
        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code"
        buildWxWidgets=Yes
    else
        read -r -p 'Would you like to re-compile wxWidgets? ( type "y" for yes or "n" for no ): '
        if [ "${REPLY}" == 'y' ] || [ "${REPLY}" == 'Y' ]; then
            buildWxWidgets=Yes
        fi
    fi

    if [ "$buildWxWidgets" == "Yes" ]; then

        WX_ROOT_DIR="${BUILD_ROOT_DIR}/wxwidgets-code"

        # https://wiki.wxwidgets.org/Possible_Configure_Flags_under_OS_X
        if [ -d "$OUT_WX" ]; then
            rm -r -v -f "$OUT_WX"
        fi

        mkdir -p "$OUT_WX"
        cd "$OUT_WX"

        "${WX_ROOT_DIR}/configure"  --with-osx-cocoa --with-macosx-version-min=11.0 --disable-debug --disable-debug-flag --enable-unicode --enable-cxx11 --with-opengl --with-expat=builtin --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin
        #"${WX_ROOT_DIR}/configure"  --with-osx-cocoa --with-macosx-version-min=10.15 --disable-debug --disable-debug-flag --enable-unicode --enable-cxx11 --with-opengl --with-expat=builtin --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin
        # https://wiki.wxwidgets.org/Possible_Configure_Flags_under_OS_X
            # --with-osx-cocoa
            # --with-macosx-version-min=10.15
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
            # --with-zlib=builtin
        
        nice make -j $(($(nproc) -1))
    fi
    if [ -f "$OUT_WX/lib/libwx_osx_cocoau_richtext-3.1.5.0.0.dylib " ]; then
        cd "$OUT_WX"
        make install
    fi

    cd "${BUILD_ROOT_DIR}"
}

function codeblocks_build()
{
    cd "${BUILD_ROOT_DIR}"
    if [ ! -d wxwidgets-code ]; then
        svn checkout http://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code
        cd codeblocks-code
        CB_ROOT_DIR=$PWD
        cd "${BUILD_ROOT_DIR}"
    fi

    if [ -d "$OUT_CB" ]; then
        rm -r -v -f "$OUT_CB"
    fi
    mkdir -p "$OUT_CB"
        
    cd "${CB_ROOT_DIR}"
    read -r -p 'Would you like to build/rebuild Codeblocks? ( type "y" for yes or "n" for no ): '
    if [ "${REPLY}" == 'y' ] || [ "${REPLY}" == 'Y' ]; then
        # To make sure you start again from scratch. You can do that : ?cd codeblock-code | svn cleanup --remove-unversioned
        # before to launch the script.
        if [ -f ./configure ]; then
            make clean
            make distclean
            make clean-bin
            make clean-zipfiles
            rm ./configure
            rm ./makefile
        fi
        chmod +x ./bootstrap *.sh
        ./bootstrap
        if [ -f ./configure ]; then
            chmod +x configure *.sh
            ./configure
        else
            cd "${BUILD_ROOT_DIR}"
            echo "+--------------------------------+"
            echo "| ERROR: bootstrap failed!!!!    |"
            echo "+--------------------------------+"
            return -1
        fi
    fi
    
    if [ -f ./makefile ]; then
        make -j $(($(nproc) -1))
        ./bundle.sh

        read -r -p 'Would you like to install CodeBlocks? ( type "y" for yes or "n" for no ): '
        if [ "${REPLY}" == 'y' ] || [ "${REPLY}" == 'Y' ]; then
            make install
            cp -R ./CodeBlocks.app /Applications
        fi
    else
        cd "${BUILD_ROOT_DIR}"
        echo "+--------------------------------+"
        echo "| ERROR: Configure failed!!!!    |"
        echo "+--------------------------------+"
        return -2
    fi
    cd "${BUILD_ROOT_DIR}"
}

cd "$SCRIPTPATH"

# Un-comment the following to install build dependencies:
#install_dependencies

# Un-comment the following to install extra apps to help development:
#install_extra_apps

# setup variables based on directories found. Variables used in building wxWidgets and Code::Blocks
setup_variables

if [ "${WX_ROOT_DIR}" = "" ] || [ ! -f "$OUT_WX/lib/libwx_osx_cocoau_richtext-3.1.5.0.0.dylib" ]; then
    wxwidgets_build
else
    echo "Allready built wxWidgets"
#    wxwidgets_build
fi

if [ "${CB_ROOT_DIR}" = "" ] || [ ! -f "$OUT_CB/bin/codeblocks" ]; then
    codeblocks_build
else
    echo "Allready built Code::Blocks"
#    codeblocks_build
fi

cd "${InitialDir}"
