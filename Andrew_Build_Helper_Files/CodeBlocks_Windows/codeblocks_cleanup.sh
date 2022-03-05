#!/bin/bash

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

echo 'Cleaning C::B directory build files.'

CurrentDir=$PWD
# [ -f "Makefile" ] && make clean

if [ -f "bootstrap" ]; then
    cd src
else
    if [ -f "../../bootstrap" ]; then
        echo Could not find ../../bootstrap
        cd $CurrentDir
        exit 2
    fi
    cd ../../src
fi

# Remove specific directories
[ -d "codeblocks" ]     && rm -rf codeblocks

# Remove directories in tree
find . -type d -name "*.libs"           | xargs rm -rf
find . -type d -name "*.deps"           | xargs rm -rf
find . -type d -name "*.dirstamp"       | xargs rm -rf
find . -type d -name "*.cache"          | xargs rm -rf

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


[ -d ".objs31_64"  ] && rm -rf .objs31_64
[ -d "devel31_64"  ] && rm -rf devel31_64
[ -d "output31_64" ] && rm -rf output31_64

[ -d ".objs31_32"  ] && rm -rf .objs31_32
[ -d "devel31_32"  ] && rm -rf devel31_32
[ -d "output31_32" ] && rm -rf output31_32

[ -d ".objs31"  ] && rm -rf .objs31
[ -d "devel31"  ] && rm -rf devel31
[ -d "output31" ] && rm -rf output31


cd ..
[ -f "codeblocks.pc" ]      && rm -f "codeblocks.pc"
[ -f "configure" ]          && rm -f "configure"
[ -f "configure~" ]         && rm -f "configure~"
[ -f "configure.in" ]       && rm -f "configure.in"
[ -f "Makefile" ]           && rm -f "Makefile"
[ -f "Makefile.in" ]        && rm -f "Makefile.in"
[ -f "m4\lt~obsolete.m4" ]  && rm -f "m4\lt~obsolete.m4"
[ -f "m4\ltversion.m4" ]    && rm -f "m4\ltversion.m4"
[ -f "m4\ltsugar.m4" ]      && rm -f "m4\ltsugar.m4"
[ -f "m4\ltoptions.m4" ]    && rm -f "m4\ltoptions.m4"
[ -f "m4\libtool.m4" ]      && rm -f "m4\libtool.m4"
[ -f "config.sub" ]         && rm -f "config.sub"
[ -f "config.status" ]      && rm -f "config.status"
[ -f "config.log" ]         && rm -f "config.log"
[ -f "config.guess" ]       && rm -f "config.guess"
[ -f "compile" ]            && rm -f "compile"
[ -f "codeblocks.spec.fedora" ]    && rm -f "codeblocks.spec.fedora"
[ -f "codeblocks.spec" ]    && rm -f "codeblocks.spec"
[ -f "aclocal.m4" ]         && rm -f "aclocal.m4"
[ -f "revision.m4" ]        && rm -f "revision.m4"
[ -f "missing" ]            && rm -f "missing"
[ -f "ltmain.sh" ]          && rm -f "ltmain.sh"
[ -f "libtool" ]            && rm -f "libtool"
[ -f "install-sh" ]         && rm -f "install-sh"
[ -f "depcomp" ]            && rm -f "depcomp"
[ -f "wxwin.m4" ]           && rm -f "wxwin.m4"
[ -d "autom4te.cache" ]     && rm -rf autom4te.cache
find . -maxdepth 1 -type f -name "z_*_result.txt"           | xargs rm -f

# For Linux, but does not fail on windows in order to keep common script!!!
if [ -f "../codeblocks*.dsc" ]; then 
    echo 'Deleting Debain build files'
    [ -f "../*.log" ]                  && rm -f "../*.log"
    [ -f "../*.deb" ]                  && rm -f "../*.deb"
    [ -f "../codeblocks*.dsc" ]        && rm -f "../codeblocks*.dsc"
    [ -f "../codeblocks*.tar.xz" ]     && rm -f "../codeblocks*.tar.xz"
    [ -f "../codeblocks*.buildinfo" ]  && rm -f "../codeblocks*.buildinfo"
    [ -f "../codeblocks*.changes" ]    && rm -f "../codeblocks*.changes"
    [ -d "../codeblocks/gtk3-unicode-3.0" ]&& rm -rf ../codeblocks
fi

echo 'Completed clean'
cd $CurrentDir