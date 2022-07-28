THESE INSTRUCTIONS NEED TO BE:
    1) DOUBLE CHECKED to ensure they are 100% accurate!!!!
    2) CHECKED FOR INTERPRETATION AND READABILITY to ensure thAT THEY ARE EASY TO FOLLOW!!!!


These notes are for developers wishing to build Code::Blocks from source on Unix using the 
Code::Blocks workspace using an existing installed version of Code::Blocks.

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
    1) A recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
    2) A working GNU GCC C & C++ compiler.
    3) wxWidget and wxGTK3 development environment
    4) Optional: autotools environment (autoconf, automake, libtool, make, etc)
    5) GTK2 or GTK3 (preferred) development environment
    6) Boost development environment
    7) Hunspell development environment
    8) Gamin development environment
    9) Optional: SVN and/or GIT.
    10) Optional packages needed for building Debian deb files
    11) Optional package to resolve warnings when run from shell

    
    DO NOT USE ANY SNAP PACKAGES as the installed packages are sandboxed by default.

    To install the items above the following are the commands to use for Ubuntu/Xubuntu/Linux Mint:
        # Step 2:
        sudo apt install -y wx3.0-headers wx-common libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev libwxbase3.0-dev
        # Step 3:
        sudo apt install -y build-essential
        # Optional step 4:
        sudo apt install -y libtool automake autoconf
        # Step 5:
        sudo apt install -y libgtk-3-dev
        # Step 6,7 & 8:
        sudo apt install -y libboost-dev libboost-system-dev libhunspell-dev libgamin-dev 
        # Optional step 9:
        sudo apt install -y subversion git
        # Optional step 10:
        sudo apt install -y libbz2-dev debhelper cdbs  xsltproc
        # Optional step 11:
        sudo apt install yaru-theme-icon

To build Code::Blocks:
    1) Grab the source code from https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or via GIT or by downloading a snapshot.
    2) Make sure the source code directory does not have spaces or any non ASCII characters.
    3) Run the nightly Code::Blocks 
    4) Open the following Code::Blocks workspace:
        src/CodeBlocks_wx30-unix.workspace
    5) Build the workspace via one of the following methods:
        a) Click on the build icon in the compiler toolbar if it is shown
        b) Select the Build->Build menu option
        c) Press CTRL-F9
        d) In the management display right click on the "CodeBLokcs Workspace wx3.1.x ...." and select build or rebuild workspace
    6) In the "Build log" keep an eye out for when the build finishes or any errors you will need to fix. 
        If you cannot find it press F2 to open or close the "Logs & others" view. Once it is open select the 
        "Build Log" tab, but if it is not available then right clock on an existing tab and then goto toggle \
        in the pop up context menu and click on "Build log"
   
    NOTES:
    1) If the NassiShneiderman-plugin fails to build with a boost error then try the following:
        a) Check you have install the libboost-dev package by running the following command:
            dpkg -l | grep libboost | grep dev

        b) Explicitly set the boost-libdir by adding the following line to the configure-line above:
            "--with-boost-libdir=LIB_DIR"
           NOTE: Depending on your system, LIB_DIR might be "/usr/lib" or "/usr/lib64".

Update Code::Blocks Build directories:
    Once you have successfully built Code::Blocks you will need to run a script file to update the devel and output 
      directories to copy the needed files. To do this run the file below from the src directory:
            ./Unix_update30.sh

Run Code::Blocks you built:
    If everything has succeeded then you can run Code::Blocks from either the devel or output directories. 
    If Code::Blocks crashes when you run it then run it from the devel directory and a codeblocks.rpt crash 
      file will be created that you should open as is a trace file that shows the functions called when
      the crash occurred.
    To run Code::Blocks open one of the following appropriate files:
        devel31_64\codeblocks
        output30\codeblocks
        
    
Debugging Code:Blocks source
----------------------------
You can debug Code::Blocks from within Code::Blocks. This allows you to set breakpoints in the code
to debug problems in order to fix them or report them as a bug.

The following is the process to debug Code::Blocks built form the source files:
    1) From the nightly build open the workspace (see above for the appropriate workspace)
    2) Build Code:Blocks as per the process above
    3) Run the update batch file using the process above
    4) Now instead of running the Code::Blocks you do one of the following to run Code::Blocks 
        via the debugger in the Code::Blocks you have open:
            a) Click on the build icon in the debugger toolbar if it is shown
            b) Select the Debug->Start/Continue menu option
            c) Press F8
    5) You are now debugging Code::Blocks within Code::Blocks. 

You can also run the .\output\codeblocks. but you will need use the following process:
    1) From the nightly build open the workspace (see above for the appropriate workspace)
    2) Build Code:Blocks as per the process above
    3) Exit Code:Blocks
    4) Run the update batch file using the process above
    5) Run the .\output\codeblocks
    6) Open the workspace (see above for the appropriate workspace)
    7) Now instead of running the Code::Blocks you do one of the following to run Code::Blocks 
        via the debugger in the Code::Blocks you have open:
            a) Click on the build icon in the debugger toolbar if it is shown
            b) Select the Debug->Start/Continue menu option
            c) Press F8
    8) You are now debugging Code::Blocks within the previously built Code::Blocks. 

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

