Unix build instructions:
------------------------
You need a working autotools environment (autoconf, automake, libtool, make, etc).
In a terminal, go to the top level folder.
If you fetched the sources from SVN, you need to bootstrap the program first. So type:

./bootstrap

This will adapt the project's configuration file to your environment. This only needs to be done
once: the first time you checkout the SVN version.

After this, type the following:

./configure --with-contrib-plugins=all
make
make install

For the last step you must be root.
That's it.

or:

./configure --prefix=/usr --with-contrib-plugins=all,-help
make
make install

"all" compiles all contrib plugins
"all,-help" compiles all contrib plugins except the help plugin
By default, no contrib plugins are compiled
Plugin names are:
    AutoVersioning, BrowseTracker, byogames, Cccc, CppCheck, cbkoders, codesnippets, codestat,
    copystrings, Cscope, DoxyBlocks, dragscroll, EditorConfig, EditorTweaks, envvars, FileManager,
    headerfixup, help, hexeditor, incsearch, keybinder, libfinder, MouseSap, NassiShneiderman,
    ProjectOptionsManipulator, profiler, regex, ReopenEditor, rndgen, exporter, symtab,
    ThreadSearch, ToolsPlus, Valgrind, wxsmith, wxsmithcontrib,wxsmithaui

If the NassiShneiderman-plugin should be build, you might see this error, when you run configure:

checking whether the Boost::System library is available... yes
configure: error: Could not find a version of the library!

If this happens, you have to explicitly set the boost-libdir.
You should try to add "--with-boost-libdir=LIB_DIR" to the configure-line.
Depending on your system, LIB_DIR might be "/usr/lib" or "/usr/lib64".

Working on Code::Blocks sources from within Code::Blocks!
---------------------------------------------------------
The following apply for all platforms where you have Code::Blocks installed and working.

These two folders will contain the same files and directory structure and you can use the IDE from
either of these two directories. This structure has been created so that you can work in
Code::Blocks while editing Code::Blocks' sources ;). Basically, you 'll be using the
"output/CodeBlocks.exe" executable. Code::Blocks' project settings are such that all output goes
under "devel". So you can edit Code::Blocks' sources inside Code::Blocks and, when pressing "Run",
it will run the "devel/CodeBlocks.exe" executable ;). This way, you can't ruin the main executable
you 're using (under "output"). When your changes satisfy you and all works well, quit Code::Blocks,
run "make update" from command line and re-launch "output/CodeBlocks.exe". You 'll be working on
your brand new IDE!


***********************************************************************************************************************************************
***********************************************************************************************************************************************
***********************************************************************************************************************************************
***********************************************************************************************************************************************

Code::Blocks Linux build instructions:
======================================

These notes are for developers wishing to build Code::Blocks from source

WxWidget 3.1.5 DEBIAN Packages
------------------------------
The following commands obtain the WxWidget 3.1.5 files required to build Code::Blocks

wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxbase3.1-0-unofficial-dbg_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxbase3.1-0-unofficial_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxbase3.1unofficial-dev_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-media3.1-0-unofficial-dbg_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-media3.1-0-unofficial_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-media3.1unofficial-dev_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-webview3.1-0-unofficial-dbg_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-webview3.1-0-unofficial_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk-webview3.1unofficial-dev_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk3.1-0-unofficial-dbg_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk3.1-0-unofficial_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/libwxgtk3.1unofficial-dev_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/wx-common_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/wx3.1-examples_3.1.5-1.focal_all.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/wx3.1-headers_3.1.5-1.focal_amd64.deb
wget https://repos.codelite.org/wx3.1.5/ubuntu/pool/universe/w/wxwidgets3.1/wx3.1-i18n_3.1.5-1.focal_all.deb

Once you obtain them install them by the following dpkg command:
sudo dpkg -i *.deb 

Linux Development Environment
-----------------------------
You need the following working:
    autotools environment (autoconf, automake, libtool, make, etc).
    GCC compiler
    GDB Debugger for debugging

You need to install the following:   
    sudo apt-get install libwxbase3.0-0-unofficial libwxbase3.0-dev libwxgtk3.0-0-unofficial libwxgtk3.0-dev wx3.0-headers wx-common libwxbase3.0-dbg libwxgtk3.0-dbg wx3.0-i18n wx3.0-examples wx3.0-doc
    sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev libboost-iostreams-dev
    sudo apt-get install libgtk-3-dev pkg-config hunspell libhunspell-dev libgamin-dev libboost-dev    
    sudo apt install subversion


Build WxWidget 
--------------

To build wxWidgets (with GTK3 support):

cd /home/<USERNAME>
git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets wxWidgets_github
cd /home/<USERNAME>/wxWidgets_github
mkdir build-release-gtk3
cd build-release-gtk3
../configure --disable-debug_flag --with-gtk=3
make -j$(nproc)
sudo make install


Fetch Code::Blocks Source Code
------------------------------
In a terminal, ctreate or go to the folder you want the Code::Blocks SVN repostory installed in and run one of the following command:
    svn checkout https://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code

    git svn checkout http://svn.code.sf.net/p/codeblocks/code/trunk  codeblocks-code


Building Code::Blocks Source Code
---------------------------------
If you fetched the sources from SVN, you need to bootstrap the program first. So type:

./bootstrap

This will adapt the project's configuration file to your environment. This only needs to be done once: the first time you checkout the SVN version.

After this, type the following:

./configure --with-contrib-plugins=all --with-boost-libdir=/usr/lib/x86_64-linux-gnu --with-boost-system=boost_system
make
sudo make install

or:

./configure --prefix=/usr --with-contrib-plugins=all,-help
make
sudo make install

"all" compiles all contrib plugins
"all,-help" compiles all contrib plugins except the help plugin
By default, no contrib plugins are compiled
Plugin names are:
    AutoVersioning, BrowseTracker, byogames, Cccc, CppCheck, cbkoders, codesnippets, codestat,
    copystrings, Cscope, DoxyBlocks, dragscroll, EditorConfig, EditorTweaks, envvars, FileManager,
    headerfixup, help, hexeditor, incsearch, keybinder, libfinder, MouseSap, NassiShneiderman,
    ProjectOptionsManipulator, profiler, regex, ReopenEditor, rndgen, exporter, symtab,
    ThreadSearch, ToolsPlus, Valgrind, wxsmith, wxsmithcontrib,wxsmithaui

If the NassiShneiderman-plugin should be build, you might see this error, when you run configure:

checking whether the Boost::System library is available... yes
configure: error: Could not find a version of the library!

If this happens, you have to explicitly set the boost-libdir.
You should try to add "--with-boost-libdir=LIB_DIR" to the configure-line.
Depending on your system, LIB_DIR might be "/usr/lib" or "/usr/lib64".

Working on Code::Blocks sources from within Code::Blocks!
---------------------------------------------------------
The following apply for all platforms where you have Code::Blocks installed and working.

These two folders will contain the same files and directory structure and you can use the IDE from
either of these two directories. This structure has been created so that you can work in
Code::Blocks while editing Code::Blocks' sources ;). Basically, you 'll be using the
"output/CodeBlocks.exe" executable. Code::Blocks' project settings are such that all output goes
under "devel". So you can edit Code::Blocks' sources inside Code::Blocks and, when pressing "Run",
it will run the "devel/CodeBlocks.exe" executable ;). This way, you can't ruin the main executable
you 're using (under "output"). When your changes satisfy you and all works well, quit Code::Blocks,
run "make update" from command line and re-launch "output/CodeBlocks.exe". You 'll be working on
your brand new IDE!


