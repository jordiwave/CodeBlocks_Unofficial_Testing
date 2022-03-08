THESE INSTRUCTIONS NEED TO BE:
    1) DOUBLE CHECKED to ensure they are 100% accurate!!!!
    2) CHECKED FOR INTERPRETATION AND READABILITY to ensure thAT THEY ARE EASY TO FOLLOW!!!!


These notes are for developers wishing to build Code::Blocks from source on MacOS without having 
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
    1) Brew installer - https://brew.sh
    2) Optional: A recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
TBA-    3) A working GNU GCC C & C++ compiler.
TBA-    4) wxWidget and wxGTK3 development environment
    5) autotools environment (autoconf, automake, libtool, make, etc)
    6) GTK2 or GTK3 (preferred) development environment
    7) Boost development environment
    8) Hunspell development environment
TBA-    9) Gamin development environment
    10) Optional: SVN and/or GIT.
    11) CoreUntils
    
    To install the items above the following are the commands to use for MacOS Big Sir (11.01):
        # Step 2:
            brew install wxwidgets
TBA-        sudo apt install -y wx3.0-headers wx-common libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev libwxbase3.0-dev
TBA-        # Step 3:
TBA-        sudo apt install -y build-essential
        # Step 5:
            brew install libtool automake autoconf
        # Step 6:
            brew install gtk+3
        # Step 7 & 8:
            brew install boost hunspell
TBA-        step 9:
TBA-        TB
        # Optional step 10:
            brew install subversion git
        # Step 11:
            brew install coreutils

 
To build Code::Blocks:
    1) Grab the source code from https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or via GIT or
        by downloading a snapshot.

    2) Make sure the source code directory does not have spaces or any non ASCII characters.

    3) In a terminal (e.g. bash) go to the top level folder you fetched the sources from SVN or GIT or the 
         directory you uncompressed the snapshot into.

    4) Instead of performing steps 6 through 8 you can run the following script:
        ./codeblocks_build.sh
       
        The codeblocks_build.sh script checks for errors and if something fails please look at the script to find which
          log file to check to see what failure occured so you can fix it.
        If the codeblocks_build.sh script passes then goto the last step

    5) Run the following to configure the project files for your environment:
        ./bootstrap

    6) Run the following to produce the makefile's for your environment so you can test it or debug before installing it 
        in the Linux OS directories:
            ./configure --with-contrib-plugins=all --prefix=$PWD/src/devel31

        or if you want to install the build in the Linux OS run the following:
        ./configure --with-contrib-plugins=all
        
        NOTE: This also only needs to be done once or if you make change to the build files used by the configure process.

        NOTES: 
            A) This also only needs to be done once or if you make change to the build files used by the configure process.
        
            B) To build C::B base files with no contributed plugins use --without-contrib-plugins instead of --with-contrib-plugins=all as per the following:
                ./configure --without-contrib-plugins

            D) To build C::B base files with all contributed plugins except help use the following:
                ./configure --with-contrib-plugins=all,-help

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

    7) Run the following to build Code::Blocks
        make

        If you want to save the make results to a file then run the following command:
            make > make_result.txt 2>&1

    8) Run the following to copy all of the relevant files into the install directory structure:
        make install
       
       Notes: 
        a) depending on how you have configured the system you may need to run "make install" as root (sudo).
        b) If you want to save the make results to a file then run the following command:
            make > make_result.txt 2>&1

    9) If you run the ./codeblocks_build.sh of configured C::B with the "--prefix=$PWD/src/devel31" option then run the following to 
        test the C::B you built:
            cd $PWD/src/devel31/bin
            codeblocks

Additional Notes/Info:
- - - - - - - - - - - -
1) To rebuild Code::Blocks run the following commands:
        make clean
        make

2) If the NassiShneiderman-plugin fails to build with a boost error then try the following:
    a) Check you have install the libboost-dev package by running the following command:
        dpkg -l | grep libboost | grep dev

    b) Explicitly set the boost-libdir by adding the following line to the configure-line above:
        "--with-boost-libdir=LIB_DIR"
       NOTE: Depending on your system, LIB_DIR might be "/usr/lib" or "/usr/lib64".



Building WxWidget
-----------------
If for some reason you need to build wxWidgets here are the quick instructions:
    cd /home/<USERNAME>
    git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets wxWidgets_github
    cd /home/<USERNAME>/wxWidgets_github
    mkdir build-release-gtk3
    cd build-release-gtk3
    ../configure --disable-debug_flag --with-gtk=3
    make -j$(nproc)

If you want/need to install the files then run the following, but be aware this may cause issues if you 
have not build wxWidget correctly or built the version you need/want (USE WITH EXTREME CAUTION):
    sudo make install


Fetch Code::Blocks Source Code
------------------------------
In a command prompt, create or go to the folder you want the Code::Blocks SVN repository installed in 
and run the following command:
    svn checkout https://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code


NOTES:
1) If you use the svn command above on Windows and want to use the files on Linux then you will need to
    edit the appropriate file in the list  below for your SVN installation as follows:
        a) Remove the # comment at the beginning of the following option:
            enable-auto-props = yes
        b) Add the following line in the [auto-props] section:
            * = svn:eol-style=AS-IS
    Potential files:
        C:\msys64\home\<username>\.subversion\config
        C:\Users\<username>\.subversion\config
        C:\Users\<username>\AppData\Roaming\Subversion\config
2) I do not recommend the following as you may encounter issues with EOL if you transfer the files to Linux or if you 
    compare against the SF soruce snapshot as the SVN keywords are not expanded:
       git svn checkout http://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code
    
3) I also do NOT recommend using TortoiseSVN on Windows to do the initial checkout as you may encounter issues with 
     EOL if you transfer the files to Linux


============================================================================================================================================
============================================================================================================================================
============================================================================================================================================

WIP OTHER PAGES:
----------------
https://github.com/bluehazzard/codeblocks_sf/wiki/build_linux_mint_18
