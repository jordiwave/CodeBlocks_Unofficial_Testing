#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

echo '+------------------------------+'
echo '| Cleaning C::B d build files. |'
echo '+------------------------------+'

CurrentDir=${PWD}
# [ -f "Makefile" ] && make clean

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

DEB_COUNT=$(ls -1q ../*.deb 2>/dev/null | wc -l 2>/dev/null)
if [[ ${LIB_SO_COUNT} -gt 0 ]] ; then
    PreviousDir=${PWD}
    cd ..
    # For Linux Debian DEB build cleanup
    echo 'Cleaning Debian deb files from: ' ${PWD} 
    find . -maxdepth 1 -type f -name "*.log"        | xargs rm -f
    find . -maxdepth 1 -type f -name "*.dsc"        | xargs rm -f
    find . -maxdepth 1 -type f -name "*.buildinfo"  | xargs rm -f
    find . -maxdepth 1 -type f -name "*.changes"    | xargs rm -f
    find . -maxdepth 1 -type f -name "*.deb"        | xargs rm -f
    find . -maxdepth 1 -type f -name "*.ddeb"       | xargs rm -f
    find . -maxdepth 1 -type f -name "*.xz"         | xargs rm -f
    find . -maxdepth 1 -type f -name "stamp*"       | xargs rm -f
    [ -d "codeblocks/gtk3-unicode-3.0" ] && rm -rf "codeblocks/gtk3-unicode-3.0"
    cd ${PreviousDir}
fi

if [ -d "..bin/codeblocks" ]; then 
    # For MacOS build cleanup
    echo "Removing MacOS directory:       ${PWD}/../bin/codeblocks"
    rm -rf "../bin/codeblocks"
fi

echo 'Cleaning files from directory: ' ${PWD}
[ -f "bundle.sh" ]                  && rm -f "bundle.sh"
[ -f "aclocal.m4" ]                 && rm -f "aclocal.m4"
[ -f "CodeBlocks-Installer.dmg" ]   && rm -f "CodeBlocks-Installer.dmg"
[ -f "codeblocks.pc" ]              && rm -f "codeblocks.pc"
[ -f "codeblocks.plist" ]           && rm -f "codeblocks.plist"
[ -f "codeblocks.spec" ]            && rm -f "codeblocks.spec"
[ -f "codeblocks.spec.fedora" ]     && rm -f "codeblocks.spec.fedora"
[ -f "compile" ]                    && rm -f "compile"
[ -f "config.guess" ]               && rm -f "config.guess"
[ -f "config.log" ]                 && rm -f "config.log"
[ -f "config.status" ]              && rm -f "config.status"
[ -f "config.sub" ]                 && rm -f "config.sub"
[ -f "configure" ]                  && rm -f "configure"
[ -f "configure.in" ]               && rm -f "configure.in"
[ -f "configure~" ]                 && rm -f "configure~"
[ -f "debian/control" ]             && rm -f "debian/control"
[ -f "depcomp" ]                    && rm -f "depcomp"
[ -f "install-sh" ]                 && rm -f "install-sh"
[ -f "libtool" ]                    && rm -f "libtool"
[ -f "ltmain.sh" ]                  && rm -f "ltmain.sh"
[ -f "Makefile" ]                   && rm -f "Makefile"
[ -f "Makefile.in" ]                && rm -f "Makefile.in"
[ -f "m4/libtool.m4" ]              && rm -f "m4/libtool.m4"
[ -f "m4/ltoptions.m4" ]            && rm -f "m4/ltoptions.m4"
[ -f "m4/ltsugar.m4" ]              && rm -f "m4/ltsugar.m4"
[ -f "m4/ltversion.m4" ]            && rm -f "m4/ltversion.m4"
[ -f "m4/lt~obsolete.m4" ]          && rm -f "m4/lt~obsolete.m4"
[ -f "missing" ]                    && rm -f "missing"
[ -f "revision.m4" ]                && rm -f "revision.m4"
[ -f "wxwin.m4" ]                   && rm -f "wxwin.m4"
[ -f "zmake.m4" ]                   && rm -f "zmake.m4"

[ -d "autom4te.cache" ]             && rm -rf autom4te.cache
[ -d "CodeBlocks.app" ]             && rm -rf CodeBlocks.app
find . -maxdepth 1 -type f -name "z_*_result.txt"         | xargs rm -f
find . -maxdepth 1 -type f -name "config.*.cdbs-orig"     | xargs rm -f

cd src
echo 'Cleaning files from directory: ' ${PWD}

# Remove specific directories
[ -d "codeblocks" ]                 && rm -rf codeblocks

# Remove directories in tree
find . -type d -name "*.libs"       | xargs rm -rf
find . -type d -name "*.deps"       | xargs rm -rf
find . -type d -name "*.dirstamp"   | xargs rm -rf
find . -type d -name "*.cache"      | xargs rm -rf

# Remove specific files in tree
find . -type f -name "*.Plo"                    | xargs rm -f
find . -type f -name "*.bmarks"                 | xargs rm -f
find . -type f -name "*.depend"                 | xargs rm -f
find . -type f -name "*.layout"                 | xargs rm -f
find . -type f -name "*.lo"                     | xargs rm -f
find . -type f -name "*.la"                     | xargs rm -f
find . -type f -name "*.pc"                     | xargs rm -f
find . -type f -name "*cbplugin"                | xargs rm -f
find . -type f -name "Makefile"                 | xargs rm -f
find . -type f -name "Makefile.in"              | xargs rm -f
find . -type f -name ".dirstamp"                | xargs rm -f
find . -type f -name "*.gch"                    | xargs rm -f
find . -type f -name "compile_commands.json"    | xargs rm -f
find . -type f -name "*.log"                    | xargs rm -f
find . -type f -name "configure"                | xargs rm -f
find . -type f -name "configure.in"             | xargs rm -f
find . -type f -name ".last_revision"           | xargs rm -f
find . -type f -name "auto_revision.exe"        | xargs rm -f
find . -type f -name "cb_console_runner.exe"    | xargs rm -f
find . -type f -name "cb_share_config.exe"      | xargs rm -f
find . -type f -name "codeblocks.exe"           | xargs rm -f
find . -type f -name "*.o"                      | xargs rm -f
find . -type f -name "*.zip"                    | xargs rm -f
find . -type f -name "*.Po" -not -path "./src/plugins/contrib/SpellChecker/hunspell/po" -type f -delete

find . -type d -name ".objs*"                   | xargs rm -rf
find . -type d -name "devel*"                   | xargs rm -rf
find . -type d -name "output*"                  | xargs rm -rf

[ -f "src/include/config.h"    ]                && rm -rf "src/include/config.h"
[ -f "src/include/config.h.in" ]                && rm -rf "src/include/config.h.in"
[ -f "src/include/config.h.in~" ]               && rm -rf "src/include/config.h.in~"
[ -f "src/include/stamp-h1"    ]                && rm -rf "src/include/stamp-h1"

cd ..

if [ -d "debian" ] ; then 
    # For Linux Debian DEB build cleanup
    cd debian
    echo 'Cleaning Debian files from:    ' ${PWD} 
    find . -maxdepth 1 -type f -name "stamp-*"        | xargs rm -f
    find . -maxdepth 1 -type f -name "files"          | xargs rm -f
    find . -maxdepth 1 -type f -name "*.substvars"    | xargs rm -f
    find . -maxdepth 1 -type f -name "*.debhelper"    | xargs rm -f
    [ -d "./codeblocks" ]                   && rm -rf "./codeblocks"
    [ -d "./codeblocks-common" ]            && rm -rf "./codeblocks-common"
    [ -d "./codeblocks-contrib" ]           && rm -rf "./codeblocks-contrib"
    [ -d "./codeblocks-contrib-common" ]    && rm -rf "./codeblocks-contrib-common"
    [ -d "./codeblocks-contrib-dbg" ]       && rm -rf "./codeblocks-contrib-dbg"
    [ -d "./codeblocks-dbg" ]               && rm -rf "./codeblocks-dbg"
    [ -d "./codeblocks-dev" ]               && rm -rf "./codeblocks-dev"
    [ -d "./codeblocks-headers" ]           && rm -rf "./codeblocks-headers"
    [ -d "./codeblocks-libwxcontrib0" ]     && rm -rf "./codeblocks-libwxcontrib0"
    [ -d "./codeblocks-libwxcontrib0" ]     && rm -rf "./codeblocks-libwxcontrib0"
    [ -d "./codeblocks-wxcontrib-dev" ]     && rm -rf "./codeblocks-wxcontrib-dev"
    [ -d "./codeblocks-wxcontrib-headers" ] && rm -rf "./codeblocks-wxcontrib-headers"
    [ -d "./codeblocks" ]                   && rm -rf "./codeblocks"
    [ -d "./libcodeblocks0" ]               && rm -rf "./libcodeblocks0"
    [ -d "./libwxsmithlib0" ]               && rm -rf "./libwxsmithlib0"
    [ -d "./libwxsmithlib0-dev" ]           && rm -rf "./libwxsmithlib0-dev"
    [ -d "./wxsmith-dev" ]                  && rm -rf "./wxsmith-dev"
    [ -d "./wxsmith-headers" ]              && rm -rf "./wxsmith-headers"
    [ -d "./tmp" ]                          && rm -rf "./tmp"
    if [ -d ".debhelper" ] ; then 
        rm -rf ".debhelper"
    fi
    cd ..
fi

if [ -d "Build_Helper_Files" ] ; then 
    cd ./Build_Helper_Files
    find . -type f -name "*.log"            | xargs rm -f
    cd ..
fi

echo '+------------------------------+'
echo '|         Completed clean      |'
echo '+------------------------------+'
cd ${CurrentDir}