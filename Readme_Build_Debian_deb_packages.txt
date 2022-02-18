THESE INSTRUCTIONS NEED TO BE:
    1) DOUBLE CHECKED to ensure they are 100% accurate!!!!
    2) CHECKED FOR INTERPRETATION AND READABILITY to ensure thAT THEY ARE EASY TO FOLLOW!!!!


These notes are for developers wishing to build the Code::Blocks Debian deb files from source on Debian or a Debian based Linux OS 
from source files without having to install Code::Blocks. 

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
    1) Optional: A recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
    2) A working GNU GCC C & C++ compiler.
    3) wxWidget and wxGTK3 development environment
    4) autotools environment (autoconf, automake, libtool, make, etc)
    5) GTK2 or GTK3 (preferred) development environment
    6) Boost development environment
    7) Hunspell development environment
    8) Gamin development environment
    9) Optional: SVN and/or GIT.
    10) Packages needed for building Debian deb files
    
    DO NOT USE ANY SNAP PACKAGES as the installed packages are sandboxed by default.

    To install the items above the following are the commands to use for Ubuntu/Xubuntu/Linux Mint:
        # Step 2:
        sudo apt install -y wx3.0-headers wx-common libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev libwxbase3.0-dev
        # Step 3:
        sudo apt install -y build-essential
        # Step 4:
        sudo apt install -y libtool automake autoconf
        # Step 5:
        sudo apt install -y libgtk-3-dev
        # Step 6,7 & 8:
        sudo apt install -y libboost-dev libboost-system-dev libhunspell-dev libgamin-dev 
        # Optional step 9:
        sudo apt install -y subversion git
        # step 10:
        sudo apt install -y libbz2-dev debhelper cdbs xsltproc

 
To build Code::Blocks:
    1) Grab the source code from https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or via GIT or
        by downloading a snapshot.
    2) Make sure the source code directory does not have spaces or any non ASCII characters.
    3) In a terminal (e.g. bash) go to the top level folder you fetched the sources from SVN or GIT or the 
         directory you uncompressed the snapshot into.
TBA: ./update_revision.sh          
    4) Run the following to configure the project files for your environment:
        ./bootstrap

        NOTE: This only needs to be done once.
    5) To configure for building the debian deb packages run the following script:
        ./debian/setup_control.sh
    6) Run the following to build Code::Blocks and create teh Debain deb files:
        dpkg-buildpackage -us -uc


    To rebuild Code::Blocks run the following commands:
        TBA - unknown , please supply if known 

NOTES:
1) If the NassiShneiderman-plugin fails to build with a boost error then try the following:
    a) Check you have install the libboost-dev package by running the following command:
        dpkg -l | grep libboost | grep dev

    b) Explicitly set the boost-libdirby adding the following line to the configure-line above:
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



WIP OTHER PAGES:
----------------
https://github.com/bluehazzard/codeblocks_sf/wiki/build_linux_mint_18
https://forums.codeblocks.org/index.php/topic,23689.msg161532.html#msg161532
