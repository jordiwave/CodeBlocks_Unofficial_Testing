These notes are for developers wishing to build Code::Blocks from source on Windows using MSYS2 Mingw64 without having 
to install Code::Blocks.

If you want to build Code::Blocks on Windows using the workspace/project files please use the Readme_Build_Windows_by_Workspace.txt file.
If you want to build Code::Blocks on Windows using the bootstrap/configure/make process please use the Readme_Build_Windows_MSYS2_by_Makefile.txt file.
If you want to build Code::Blocks on Linux using the workspace/project files please use the Readme_Build_Linux_by_Workspace.txt file.
If you want to build Code::Blocks on Linux using the bootstrap/configure/make process please use the Readme_Build_Linux_by_Makefile.txt file.
If you want to build Code::Blocks on Linux Debian deb files please use the Readme_Build_Debian_deb_packages.txt file.
If you want to debug Code::Blocks executable on Windows please use the Readme_Build_Windows_by_Workspace.txt file.
If you want to debug Code::Blocks executable on Linux please use the Readme_Build_Linux_by_Workspace.txt file.

Code::Blocks build instructions:
--------------------------------
This section outlines the requirements and process to build Code::Blocks from source code.

Requirements:
    1) A working MSYS2 MinGW64 environment. This can be installed by:
        a) Download and run the install from the following page using the default settings:
            https://www.msys2.org/
        b) Install additional MSYS packages as per the following:
            i) Run the following progam to open a bash shell
                    C:\mssy64\msys2.exe
            ii) In the msys2 bash shell run the following command to install the additional packages
                    pacman -S msys2-runtime-devel base-devel mingw-w64-x86_64-toolchain mingw-w64-clang-x86_64-toolchain zip unzip svn libtool m4 autoconf automake mingw-w64-x86_64-boost mingw-w64-x86_64-hunspell mingw-w64-x86_64-hunspell-en mingw-w64-x86_64-fontconfig

                This command will install the following MSYS2 packages along with other dependant packages needed:
                    * msys2-runtime-devel
                    * base-devel
                    * mingw-w64-x86_64-toolchain
                    * mingw-w64-clang-x86_64-toolchain
                    * zip
                    * unzip
                    * svn
                    * libtool
                    * m4
                    * autoconf
                    * automake
                    * mingw-w64-x86_64-boost
                    * mingw-w64-x86_64-hunspell
                    * mingw-w64-x86_64-hunspell-en
                    * mingw-w64-x86_64-wxmsw3.1
                    * mingw-w64-x86_64-fontconfig
            ii) In the msys2 bash shell run the following command to update the msys2 to the latest release:
                    pacman -Syu

                NOTE: If you are asked to "To complete this update all MSYS2 processes including this terminal will be closed. Confirm to proceed [Y/n]"
                 then do NOT press Y or N, but close the bash window by clicking on the [X] on the top right of the window and then open the C:\mssy64\msys2.exe
                 again and run the "pacman -Syu" command again and again until no updates are available.
    2) A local build of wxWidgets 3.1.5 for Code::Blocks. If you have not built wxWidgetas for Code::Blocks before have a look at the following
       files or check out the https://forums.wxwidgets.org/viewtopic.php?t=42817 thread.
            - Andrew_Build_Helper_Files\Libraries_Windows\build_WXWidget_3.1.5_win32.bat
            - Andrew_Build_Helper_Files\Libraries_Windows\build_WXWidget_3.1.5_win64.bat

To build Code::Blocks:
    1) Once the changes are in the main Code::Blocks SF repo download the Code::Blocks source code from 
        https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or by downloading a snapshot.

    2) Make sure the source code directory does not have spaces or any non ASCII characters in it.

    3) Start a MinGW64 bash shell by running the following command (this is a different shell than was used for pacman installation):
        C:\msys64\mingw64.exe

    4) In the MinGW64 bash shell change to the top level folder you fetched the sources from SVN or the directory you uncompressed the snapshot into.

    5) Instead of performing steps 6 through 13 you can run the following script after checking the exported environment 
        variables for your setup and updating if applicable:
        ./codeblocks_build.sh
       
        The codeblocks_build.sh script checks for errors and if something fails please look at the script to find which
          log file to check to see what failure occured so you can fix it.
        If the codeblocks_build.sh script passes then goto the last step

    6) In the bash shell set the following environment variables:
            export WX_CONFIG_NAME=$PWD/wx-config-cb-win64
            export WX_CB_BUILD_DIR="/d/Andrew_Development/Libraries/wxWidgets-3.1.5_win64"
            export BOOST_ROOT=/mingw64

        NOTES: 
            a) Both directories above are Unix format!!! e.g. D: === /d/
            b) The WX_CB_BUILD_DIR is the root directory for the wxWidget monolithic CB build.
            c) To save these variables you need to put them in your .bashrc file (C:\msys64\home\<username>\.bashrc)

    7) Run the following to configure the project files for your environment:
            ./bootstrap

    8) Run the following to produce the makefile's for your environment:
        ./configure --prefix=$PWD/src/devel31_64 --enable-windows-installer-build --with-contrib-plugins=all --with-boost-libdir=$BOOST_ROOT/lib

        The parameters are:
            --prefix=$PWD/src/devel31_64
            --enable-windows-installer-build
            --with-boost-libdir=$BOOST_ROOT/lib
            
            with --enable-windows-installer-build the configure sets the following internally:
                =>  bindir='${prefix}'
                    sbindir='${prefix}'
                    libexecdir='${prefix}'
                    libdir='${prefix}'

        NOTES: 
            A) This also only needs to be done once or if you make change to the build files used by the configure process.
        
            B) To build C::B base files with no contributed plugins use --without-contrib-plugins instead of --with-contrib-plugins=all as per the following:
                ./configure --prefix=$PWD/src/devel31_64 --enable-windows-installer-build --without-contrib-plugins --with-boost-libdir=$BOOST_ROOT/lib

            D) To build C::B base files with all contributed plugins except help use the following:
                ./configure --prefix=$PWD/src/devel31_64 --enable-windows-installer-build --with-contrib-plugins=all,-help --with-boost-libdir=$BOOST_ROOT/lib

                Where:
                    "all" compiles all contrib plugins
                    "all,-help" compiles all contrib plugins except the help plugin
                    By default, no contrib plugins are compiled
                    Plugin names are (this list may be out of date, so you may need to lookup the plugin names manually) :
                            AutoVersioning, BrowseTracker, byogames, Cccc, CppCheck, cbkoders, codesnippets,
                            codestat, copystrings, Cscope, DoxyBlocks, dragscroll, EditorConfig, EditorTweaks, envvars, exporter,
                            FileManager, headerfixup, help, hexeditor, incsearch, keybinder, libfinder, MouseSap,
                            NassiShneiderman, ProjectOptionsManipulator, profiler, regex, ReopenEditor, rndgen, smartindent, spellchecker,
                            symtab, ThreadSearch, ToolsPlus, Valgrind, wxcontrib, wxsmith, wxsmithcontrib, wxsmithaui

    9) Run the following to build Code::Blocks
            make
        
        If you want to save the make results to a file then run the following command:
            make > make_result.txt 2>&1
        NOTE: Future - investigate why "make -jxx" fails

    10) Run the following to copy all of the relevant files into the install directory structure in $PWD/src/devel31_64:
            make install

        NOTE: Future - see why extra directories are created and see if the creation can be suppressed.

    11) You will currently need to manually copy the following files into the $PWD/src/devel31_64 directory:
            C:\msys64\mingw64\bin\libgcc_s_dw2-1.dll
            C:\msys64\mingw64\bin\libgcc_s_seh-1.dll
            C:\msys64\mingw64\bin\libwinpthread-1.dll
            C:\msys64\mingw64\bin\libstdc++-6.dll
            C:\msys64\mingw64\bin\libhunspell-1.7-0.dll

            %WX_CB_BUILD_DIR%\lib\gcc_dll\wxmsw*_gcc_cb.dll
            %WX_CB_BUILD_DIR%\lib\gcc_dll\wxmsw*_gl_gcc_cb.dll

            $PWD\exchndl\win64\bin\*.*
            
        or 
        You can run the following bash script:
            ./codeblocks_update_devel.sh        

        NOTE: Future - investigate if this can be added to the "make install" process

    12) You will need to manually rename the plugin DLL's to remove the "lib" pre-appended to the DLL names.
            You can create a batch file to do this using the following as a possible starting point:

                SET PrevDirRenameDLLFiles="%CD%"
                cd "devel31_%BUILD_BITS%\share\codeblocks\plugins"
                for /f "usebackq delims=^=^" %%a in (`"dir "lib*.dll" /b" 2^>nul`) do (set fnameDLL=%%a) & call :renameDLL
                cd /d %PrevDirRenameDLLFiles%

        NOTE: Future - investigate how to work around this.

    13) Due to a bug in the make install you will need to copy the files 
            From:   src/bin/*.*
            To:     src/devel31_64/

    14) CodeBlock's can now be run from the $PWD/src/devel31_64 directory.

Additional Info:
- - - - - - - -

    To rebuild Code::Blocks run the following commands:
        make clean
        make

Run Code::Blocks you built:
    If everything has succeeded then you can run Code::Blocks from either the src\devel31_64 or src\output31_64 directories. 
    If Code::Blocks crashes when you run it then run it from the devel directory then a codeblocks.rpt crash 
      file will be created that you should open as is it is a trace file that shows the functions called when
      the crash occurred.
    To run Code::Blocks open one of the following appropriate files:
    a) x64 (64 bit)
        src\devel31_64\codeblocks.exe
        src\output31_64\codeblocks.exe
    
    NOTE: Future - need to figure out how to produce/create the src\output31_64 directory. 

Debugging Code:Blocks source
----------------------------
See the "Readme_Build_Windows_by_Workspace.txt" file for details.


Fetch Code::Blocks Source Code
------------------------------
See the "Readme_Build_Windows_by_Workspace.txt" file for details.

============================================================================================================================================
============================================================================================================================================
============================================================================================================================================

WIP OTHER PAGES:
----------------
https://github.com/bluehazzard/codeblocks_sf/wiki/build_linux_mint_18
