#!/bin/bash

# -----------------------------------------------------------------------------

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

#CB_BUILD_DIR=CB_SVN_trunk
CB_BUILD_DIR=AC-WindowsInstaller
if [ -d "$CB_BUILD_DIR" ]; then
    echo Cleaning directory: "$CB_BUILD_DIR"
    cd "$CB_BUILD_DIR"

    if [ -f "Makefile" ]; then
        make clean
    fi
    if [ -d "autom4te.cache" ]; then
        rm -rf autom4te.cache
    fi
    if [ -d "src/.objs31_64" ]; then
        rm -rf src/.objs31_64
    fi
    if [ -d "src/.objs31_64" ]; then
        rm -rf src/.objs31_64
    fi
    if [ -d "src/devel31_64" ]; then
        rm -rf src/devel31_64
    fi
    if [ -d "src/output31_64" ]; then
        rm -rf src/output31_64
    fi

    find . -name "*.Plo"        | xargs rm -f
    find . -name "*.bmarks"     | xargs rm -f
    find . -name "*.depend"     | xargs rm -f
    find . -name "*.layout"     | xargs rm -f
    find . -name "*.lo"         | xargs rm -f
    find . -name "*.Po"         | xargs rm -f
    find . -name "*.la"         | xargs rm -f
    find . -name "*.pc"         | xargs rm -f
    find . -name "Makefile"     | xargs rm -f
    find . -name "Makefile.in"  | xargs rm -f
    
    find . -type d -name "*.libs"       | xargs rm -rf
    find . -type d -name "*.deps"       | xargs rm -rf
    find . -type d -name "*.dirstamp"   | xargs rm -rf
    find . -type d -name "*.cache"      | xargs rm -rf
    
    if [ -d "codeblocks" ]; then
        rm -rf codeblocks
    fi
    
    rm -f *.log
    rm -f revision.m4
    rm -f missing
    rm -f Makefile.in
    rm -f ltmain.sh
    rm -f libtool
    rm -f install-sh
    rm -f depcomp
    rm -f configure
    rm -f config.sub
    rm -f config.status
    rm -f config.log
    rm -f config.guess
    rm -f compile
    rm -f codeblocks.spec.fedora
    rm -f codeblocks.spec
    rm -f aclocal.m4
    rm -f .last_revision
    rm -f m4/lt~obsolete.m4
    rm -f m4/ltversion.m4
    rm -f m4/ltsugar.m4
    rm -f m4/ltoptions.m4
    rm -f m4/libtool.m4

    cd ..
fi

rm -f *.log
rm -f *.deb
rm -f codeblocks*.dsc
rm -f codeblocks*.tar.xz
rm -f codeblocks*.buildinfo
rm -f codeblocks*.changes

if [ -d "codeblocks/gtk3-unicode-3.0" ]; then
    rm -rf codeblocks
fi
