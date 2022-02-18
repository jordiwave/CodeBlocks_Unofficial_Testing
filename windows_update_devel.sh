#!/bin/bash

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                                                                                                 |"
echo "|                           Cleaning C::B directory build files.                                  |"
echo "|                                                                                                 |"
InitialDir=$PWD

# ----------------------------------------------------------------------------
# Check where we are running from and go to the C::B source root directory 
# ----------------------------------------------------------------------------

if [ ! -f "bootstrap" ]; then
    if [ -f "../../bootstrap" ]; then
        echo Could not find ../../bootstrap
        cd $InitialDir
        exit 3
    fi
    cd ../..
fi

# ----------------------------------------------------------------------------
# Check BUILD_BITS for validity
# ----------------------------------------------------------------------------

BUILD_BITS=$1
[ -f  "src/devel31_32/codeblocks.exe" ] && BUILD_BITS=32
[ -f  "src/devel31_64/codeblocks.exe" ] && BUILD_BITS=64
if ! { [ "$BUILD_BITS" == "32" ] || [ "$BUILD_BITS" == "64" ]; }; then
    echo "|                                                                                                 |"
    echo "|             +------------------------------------------------------+                            |"
    echo "|             | Error: NO Windows "32" or "64" parameter specified.  |                            |"
    echo "|             |        Please run again with a parameter             |                            |"
    echo "|             +------------------------------------------------------+                            |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd $InitialDir
    exit 4
fi
echo "|    Development directory is:           src/devel31_$BUILD_BITS                                           |"
echo "|    Detected that you are building for: $BUILD_BITS bits                                                  |"

# -------------------------------------------------------------------------------------------------------
# Fix make install issues
# -------------------------------------------------------------------------------------------------------
if [ -d "src/bin" ]; then
    echo "|                                                                                                 |"
    echo "|  Moving src/bin files into src/devel31_$BUILD_BITS due to make install issue not fixed yet.              |"
    echo "|                                                                                                 |"
    count=$(ls -1 src/bin 2>/dev/null | wc -l)
    if [ $count != 0 ] ; then
        mv src/bin/* src/devel31_$BUILD_BITS/
    fi
    
    count=$(ls -1 src/bin 2>/dev/null | wc -l)
    if [ $count == 0 ] ; then
        rmdir src/bin
    else
        echo "|                                                                                                 |"
        echo "|           +-------------------------------------------------------------------+                 |"
        echo "|           | Error: Moving src/bin files into src/devel31_$BUILD_BITS failed!  |                 |"
        echo "|           +-------------------------------------------------------------------+                 |"
        echo "|                                                                                                 |"
        echo "|                                                                                                 |"
        echo "+-------------------------------------------------------------------------------------------------+"
        echo
        cd $InitialDir
        exit 9
    fi
fi

# ----------------------------------------------------------------------------
# Check if build succeeded
# ----------------------------------------------------------------------------
if { !  {  
        [ -f "src/devel31_$BUILD_BITS/codeblocks.exe" ]     &&
        [ -f "src/devel31_$BUILD_BITS/libcodeblocks-0.dll" ]     &&
        [ -f "src/devel31_$BUILD_BITS/share/codeblocks/todo.zip" ]
        }
   } then
    echo "|                                                                                                 |"
    echo "|                   +----------------------------------------------------------+                  |"
    echo "|                   | Error: Code::Blocks make error was detected.             |                  |"
    echo "|                   |        Please fix the error and try again.               |                  |"
    [ ! -f "src/devel31_$BUILD_BITS/codeblocks.exe" ]            && echo "|                   |        Missing src/devel31_$BUILD_BITS/codeblocks.exe!            |                  |"
    [ ! -f "src/devel31_$BUILD_BITS/libcodeblocks-0.dll" ]       && echo "|                   |        Missing src/devel31_$BUILD_BITS/libcodeblocks-0.dll!       |                  |"
    [ ! -f "src/devel31_$BUILD_BITS/share/codeblocks/todo.zip" ] && echo "|                   |        Missing src/devel31_$BUILD_BITS/share/codeblocks/todo.zip! |                  |"
    echo "|                   +----------------------------------------------------------+                  |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd $InitialDir
    exit 5
fi

# ----------------------------------------------------------------------------
# Detect GCC directory 
# ----------------------------------------------------------------------------
GCC_ROOT=$(dirname $(which GCC))
if [ ! -d "$GCC_ROOT" ]; then
    echo "|                                                                                                 |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                    |     Error: NO GCC found     |                              |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                                                                                 |"
    cd $InitialDir
    exit 6
fi
echo "|    GCC_ROOT root detected at:          $GCC_ROOT                                             |"


# ----------------------------------------------------------------------------
# Detect wxWidget directory 
# ----------------------------------------------------------------------------
if [ ! -d "$WX_CB_BUILD_DIR" ]; then
    echo "|                                                                                                 |"
    echo "|             +---------------------------------------------------------+                         |"
    echo "|             |  Error: NO WX_CB_BUILD_DIR environment variable set     |                         |"
    echo "|             |           Please export WX_CB_BUILD_DIR environment and |                         |"
    echo "|             |           try again.                                    |                         |"
    echo "|             |           WX_CB_BUILD_DIR is wxWidget CB build src      |                         |"
    echo "|             |           root directory.                               |                         |"
    echo "|             +---------------------------------------------------------+                         |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd $InitialDir
    exit 7
fi
echo "|    WX_CB_BUILD_DIR set to:             $WX_CB_BUILD_DIR    |"

# -------------------------------------------------------------------------------------------
# Copy the compiler DLL and wxWidget DLL's into the src/devel31_$BUILD_BITS (32 or 64) directory
# -------------------------------------------------------------------------------------------
echo "|                                                                                                 |"
echo "|    Copying compiler and wxWidget DLL's into the src/devel31_$BUILD_BITS directory.                       |"

if [ "$BUILD_BITS" == "32" ]; then
    if [ -f  "$GCC_ROOT/libgcc_s_dw2-1.dll" ]; then
        cp -f "$GCC_ROOT/libgcc_s_dw2-1.dll"        src/devel31_$BUILD_BITS > /dev/null
    fi
fi    
if [ "$BUILD_BITS" == "64" ]; then
    if [ -f "$GCC_ROOT/libgcc_s_seh-1.dll" ]; then
        cp -f "$GCC_ROOT/libgcc_s_seh-1.dll"        src/devel31_$BUILD_BITS > /dev/null
    fi
    # The next DLL is required for some GCC compilers, but not for others. Either way copy it is if [ -f s.
    if [ -f  "$GCC_ROOT/libgcc_s_seh_64-1.dll" ]; then
        cp -f "$GCC_ROOT/libgcc_s_seh_64-1.dll"     src/devel31_$BUILD_BITS > /dev/null
    fi
fi    
[ -f  "$GCC_ROOT/libwinpthread-1.dll" ]         && cp -f "$GCC_ROOT/libwinpthread-1.dll"        src/devel31_$BUILD_BITS > /dev/null
[ -f  "$GCC_ROOT/libstdc++-6.dll" ]             && cp -f "$GCC_ROOT/libstdc++-6.dll"            src/devel31_$BUILD_BITS > /dev/null
[ -f  "$GCC_ROOT/libbz2-1.dll" ]                && cp -f "$GCC_ROOT/libbz2-1.dll"               src/devel31_$BUILD_BITS > /dev/null
[ -f  "$GCC_ROOT/zlib1.dll" ]                   && cp -f "$GCC_ROOT/zlib1.dll"                  src/devel31_$BUILD_BITS > /dev/null
[ ! -f "src/devel31_$BUILD_BITS/exchndl.dll" ]  && cp -f -r "src/exchndl/win$BUILD_BITS/bin/."  src/devel31_$BUILD_BITS > /dev/null

count=$(ls $GCC_ROOT/libhunspell-*.dll 2>/dev/null | wc -l)
if [ $count != 0 ] ; then
    for libHunspellDLLFile in $GCC_ROOT/libhunspell-*.dll
    do 
        cp -f $libHunspellDLLFile src/devel31_$BUILD_BITS > /dev/null
    done
fi

count=$(ls $WX_CB_BUILD_DIR/lib/gcc_dll/*.dll 2>/dev/null | wc -l)
if [ $count != 0 ] ; then
    for wxFileDLL in $WX_CB_BUILD_DIR/lib/gcc_dll/*.dll
    do 
        cp $wxFileDLL  src/devel31_$BUILD_BITS > /dev/null
    done
fi

# ------------------------------------------------------------------------------------------
# Rename DLL files if built with MSYS 2 using bootstrap/configure/make/make install process
# ------------------------------------------------------------------------------------------
count=$(ls -1 src/devel31_$BUILD_BITS/share/codeblocks/plugins/*.dll 2>/dev/null | wc -l)
if [ $count == 0 ] ; then
    echo "|                                                                                                 |"
    echo "|             +-----------------------------------------------------------+                       |"
    echo "|             | Error: Code::Blocks Plugin DLL files not found in:        |                       |"
    echo "|             |          src/devel31_$BUILD_BITS/share/codeblocks/plugins          |                       |"
    echo "|             +-----------------------------------------------------------+                       |"
    echo "|                                                                                                 |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    cd $InitialDir
    exit 8
fi

count=$(ls -1 src/devel31_$BUILD_BITS/share/codeblocks/plugins/lib*.dll 2>/dev/null | wc -l)
if [ $count != 0 ] ; then
    PrevDirectory=$PWD
    cd "src/devel31_$BUILD_BITS/share/codeblocks/plugins"
    for pluginLibFile in lib*.dll
    do 
        newPluginLibFile=$(echo $pluginLibFile |sed 's/^.\{3\}//g')
        [ -f $newFilename ] && rm -f $newPluginLibFile
        mv $pluginLibFile $newPluginLibFile
    done
    cd $PrevDirectory
fi

# -------------------------------------------------------------------------------------------------------
# Delete DLL l*.la redundant files built with MSYS 2 using bootstrap/configure/make/make install process
# -------------------------------------------------------------------------------------------------------
count=$(ls -1 src/devel31_$BUILD_BITS/*.la 2>/dev/null | wc -l)
if [ $count != 0 ] ; then
    PrevDirectory=$PWD
    cd "src/devel31_$BUILD_BITS"
    find . -type f -name "*.la" | xargs rm -f
    cd $PrevDirectory
fi

# ------------------------------------------------------------------------------------------

echo "|                                                                                                 |"
echo "|                      Completed Cleaning C::B directory build files.                             |"
echo "|                                                                                                 |"
echo "+-------------------------------------------------------------------------------------------------+"
echo

cd $InitialDir