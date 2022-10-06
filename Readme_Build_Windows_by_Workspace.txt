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


Code::Blocks build instructions:
--------------------------------
This section outlines the requirements and process to build Code::Blocks from source code.

Requirements:
    1) A recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
    2) A working GNU GCC C & C++ compiler. See Readme_Build_Windows_MSYS2_by_Makefile.txt for MSYS2 install info/guide.
    3) A local build of wxWidgets for Code::Blocks

To build Code::Blocks:
    1) Grab the source code from https://sourceforge.net/p/codeblocks/code/HEAD/tree/ via SVN or via GIT or by downloading a snapshot.
    2) Make sure the source code directory does not have spaces or any non ASCII characters.
    3) Run the nightly Code::Blocks 
    4) Ensure that the "GNU GCC Compiler" is configured by opening the "Global compiler settings" from 
        the "Settings->Compiler" menu and then in the Selected Compiler drop down list select "GNU GCC Compiler". 
        Then clock on the "toolchain executables" and ensure the settings are correct for the GNU GCC compiler 
        you are going to use to build Code::Blocks.
    5) Open one of the following Code::Blocks workspace that is applicable for your Windows and GNU GCC installed:
        a) x86 (32 bit)
            src/CodeBlocks_wx31.workspace
        b) x64 (64 bit)
            src/CodeBlocks_wx31_64.workspace
    5) Build the workspace via one of the following methods:
        a) Click on the build icon in the compiler toolbar if it is shown
        b) Select the Build->Build menu option
        c) Press CTRL-F9
        d) In the management display right click on the "CodeBLokcs Workspace wx3.1.x ...." and select build or rebuild workspace
    6) In the "Build log" keep an eye out for when the build finishes or any errors you will need to fix. 
        If you cannot find it press F2 to open or close the "Logs & others" view. Once it is open select the 
        "Build Log" tab, but if it is not available then right clock on an existing tab and then goto toggle \
        in the pop up context menu and click on "Build log"
    
Update Code::Blocks Build to add DLL's etc:
    Once you have successfully built Code::Blocks you will need to run a batch file to update the devel and output 
      directories to copy the needed files. To do they update run the appropriate file below from the src directory
      in a command window:
        a) x86 (32 bit)
            update.bat 31_32
        b) x64 (64 bit)
            update.bat 31_64

Run Code::Blocks you built:
    If everything has succeeded then you can run Code::Blocks from either the devel or output directories. 
    If Code::Blocks crashes when you run it then run it from the devel directory and a codeblocks.rpt crash 
      file will be created that you should open as is a trace file that shows the functions called when
      the crash occurred.
    To run Code::Blocks open one of the following appropriate files:
    a) x86 (32 bit)
        devel31\codeblocks.exe
        output31\codeblocks.exe
    b) x64 (64 bit)
        devel31_64\codeblocks.exe
        output31_64\codeblocks.exe
            
        
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

4) This may not be 100% right, but if you are using MSYS2 then you need to install the packages for MSYS2\MinGW64 as per the commands below:
    pacman -S mingw-w64-x86_64-toolchain msys2-runtime-devel zip unzip mingw-w64-clang-x86_64-toolchain
    pacman -Syu


WIP OTHER PAGES:
----------------
https://github.com/bluehazzard/codeblocks_sf/wiki/build_linux_mint_18
https://forums.codeblocks.org/index.php/topic,23689.msg161532.html#msg161532
