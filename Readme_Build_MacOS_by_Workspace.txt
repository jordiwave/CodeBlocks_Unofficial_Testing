THESE INSTRUCTIONS NEED TO BE CHECKED FOR INTERPRETATION AND READABILITY to ensure thAT THEY ARE EASY TO FOLLOW!!!!

These notes are for developers wishing to build Code::Blocks from source on Windows.

The only external library needed to build Code::Blocks is wxWidgets.
You must compile wxWidgets as a monolithic DLL. Refer to the build documentation in the wxWidgets
sources on how to do it.

If you want to build Code::Blocks on Windows using the workspace/project files please use the Readme_Build_Windows_by_Workspace.txt file.
If you want to build Code::Blocks on Windows using the bootstrap/configure/make process please use the Readme_Build_Windows_MSYS2_by_Makefile.txt file.
If you want to build Code::Blocks on Linux using the workspace/project files please use the Readme_Build_Linux_by_Workspace.txt file.
If you want to build Code::Blocks on Linux using the bootstrap/configure/make process please use the Readme_Build_Linux_by_Makefile.txt file.
If you want to build Code::Blocks on Linux Debian deb files please use the Readme_Build_Debian_deb_packages.txt file.
If you want to debug Code::Blocks executable on Windows please use the Readme_Build_Windows_by_Workspace.txt file.
If you want to debug Code::Blocks executable on Linux please use the Readme_Build_Linux_by_Workspace.txt file.
If you want to build Code::Blocks on MacOS using the workspace/project files please use the Readme_Build_MacOS_by_Workspace.txt file.
If you want to build Code::Blocks on MacOS using the bootstrap/configure/make process please use the Readme_Build_MacOS_by_Makefile.txt file.


Code::Blocks build instructions:
--------------------------------
This section outlines the requirements and process to build Code::Blocks from source code.

Requirements:
    1) The latest official Code::Blocks MacOS release
    2) A working CLANG compiler.
    3) Brew installer - https://brew.sh
    4) Optional: A recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
    5) wxWidget and wxGTK3 development environment
    6) autotools environment (autoconf, automake, libtool, make, etc)
    7) GTK2 or GTK3 (preferred) development environment
    8) Boost development environment
    9) Hunspell development environment
    10) Optional: SVN and/or GIT.
    11) CoreUntils
    
    To install the items above the following are the commands to use for MacOS Big Sir (11.01):
        # Step 5:
            brew install wxwidgets
        # Step 6:
            brew install libtool automake autoconf
        # Step 7:
            brew install gtk+3
        # Step 8 & 9:
            brew install boost hunspell
        # Optional step 10:
            brew install subversion git
        # Step 11:
            brew install coreutils

To build Code::Blocks:
    1) Grab the source code from https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or
        by downloading a snapshot.
    2) Make sure the source code directory does not have spaces or any non ASCII characters.
    3) Run Code::Blocks 
    4) Ensure that C::B is set to use the bash shell instead of zsh as folows:
        * Select the  "Settings->"Environment" menu option
        * Select the "General Settings" icon in the top 
        * In the options area at the bottom scroll to the bottom or resize to show the bottom options
        * Change the "Shell to run commands in:" to "bin/bash -c"
    5) Open one of the following Code::Blocks workspace:
            src/CodeBlocks_MacOS.workspace
    6) Build the workspace via one of the following methods:
        a) Click on the build icon in the compiler toolbar if it is shown
        b) Select the Build->Build menu option
        c) Press CTRL-F9
        d) In the management display right click on the "CodeBLokcs Workspace wx3.1.x ...." and select build or rebuild workspace
    6) In the "Build log" keep an eye out for when the build finishes or any errors you will need to fix. 
        If you cannot find it press F2 to open or close the "Logs & others" view. Once it is open select the 
        "Build Log" tab, but if it is not available then right clock on an existing tab and then goto toggle \
        in the pop up context menu and click on "Build log"
    

Run Code::Blocks you built:
    If everything has succeeded then you can run Code::Blocks from either the devel31_64 directory. 
    If Code::Blocks crashes when you run it then run it from the devel31_64 directory and a codeblocks.rpt crash 
      file will be created that you should open as is a trace file that shows the functions called when
      the crash occurred.
    To run Code::Blocks the following needs to be done:
        a) Open a terminal window
        b) Change to the src\devel31_64 directory:
        c) Use the following command to start CodeBlocks:
            /codeblocks -v --prefix $PWD
            

**************************************************************************************************************
**************************************************************************************************************
**************************************************************************************************************
        
Debugging Code:Blocks source
----------------------------
You can debug Code::Blocks from within Code::Blocks. This allows you to set breakpoints in the code
to debug problems in order to fix them or report them as a bug.

The following is the process to debug Code::Blocks built form the source files:
    1) From the nightly build open the workspace (see above for the appropriate workspace)
    2) Setup the following global environment variables via the "Setting->Global Variables..." menu option
        Current set: default
        Current Variable:   CB_RELEASE_TYPE
                            Settings:
                                    Built-in fields:
                                        Name:       Value:
                                        BASE        -g -O0 -ggdb
        Current Variable:   WX31_64
                            Settings:
                                    Built-in fields:
                                        Name:       Value:
                                        base        <root directory of your wxwidget3.1.5 local build for CB)
                                        lib         $(#WX31_64)\lib
                                        include     $(#WX31_64)\include

    3) Build Code:Blocks as per the process above
    4) Run the update batch file using the process above
    5) Now instead of running the Code::Blocks exe you do one of the following to run Code::Blocks 
        via the debugger in the Code::Blocks you have open:
            a) Click on the build icon in the debugger toolbar if it is shown
            b) Select the Debug->Start/Continue menu option
            c) Press F8
    6) You are now debugging Code::Blocks within Code::Blocks. 

You can also run the output\codeblocks.exe, but you will need use the following process:
    1) From the nightly build open the workspace (see above for the appropriate workspace)
    2) Build Code:Blocks as per the process above
    3) Exit Code:Blocks
    4) Run the update batch file using the process above
    5) Run the output\codeblocks.exe
    6) Open the workspace (see above for the appropriate workspace)
    7) Now instead of running the Code::Blocks exe you do one of the following to run Code::Blocks 
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


I do not recommend the following as you may encounter issues with EOL if you transfer the files to Linux or if you 
    compare against the SF soruce snapshot as the SVN keywords are not expanded:
       git svn checkout http://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code
       
       
       
       
DUMPING GROUND:

zip -r MACOS . -i /*_MacOS.cbp 