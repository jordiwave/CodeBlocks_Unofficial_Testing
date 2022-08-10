Windows Clangd executable install process:
==========================================

There are three main options to install the clangd.exe:
1) Install the LLVM compiler.
2) Manually extract the required files from the LLVM compiler.
3) Install the Clangd package for the Windows compiler you are using if it is available.

The  process for the three options above are detailed below.

Install the LLVM compiler
=========================
1) Download the latest (non RC/Beta)LLVM Windows executable for your OS (Win32 or Win64) from the following Github LLVM download page:
      https://github.com/llvm/llvm-project/releases

   As of May 2022 the Windows files are names as follows:
      LLVM-<version>-win64.exe
      LLVM-<version>-win32.exe
   where <version> is the LLVM version, like 14.0.0

2) Run the LLVM-<version>-win<xx>.exe you downloaded to install the LLVM compiler.



Manually Extract File from LLVM compiler
=========================================
1) Download the latest (non RC/Beta)LLVM Windows executable for your OS (Win32 or Win64) from the following Github LLVM download page:
      https://github.com/llvm/llvm-project/releases

   As of Jan 2022 the Windows files are names as follows:
      LLVM-<version>-win64.exe
      LLVM-<version>-win32.exe
   where <version> is the LLVM version, like 13.0.0 or 13.0.1.
2) Unzip the LLVM-<version>-win<xx>.exe file you downloaded using 7ZIP or your preferred ZIP program into a sub directory
3) Create a new directory to put the clangd.exe and dll's
4) Copy the following files into a the new directory created from the unziped LLVM directory:
            bin\clangd.exe
            bin\msvcp140.dll
            bin\vcruntime140.dll
            bin\vcruntime140_1.dll



Windows Compiler Clangd/LLVM Pacakage Installer
===============================================
Due to the number of different compilers available for Windows not all of the compilers will have either/both the Clang or LLVM required files.

If you want to install the specific package(s) for the Windows compiler you are using in order to use it's clangd.exe file please follow the 
instructions below for the specific compiler you have installed:

MSYS2 Compiler - MinGW64
-------------------------
There are two main options to install the clangd.exe as follows:
    1) The first option in order to  minimise disk space is to install the Clang extra tools using one of the following packages:
        +------------------------------------------+------------------------+
        |               Package                    | Clangd executable      |
        +------------------------------------------+------------------------+
        | mingw-w64-clang-x86_64-clang-tools-extra | clang64/bin/clangd.exe |
        | mingw-w64-x86_64-clang-tools-extra       | mingw64/bin/clangd.exe |
        +------------------------------------------+------------------------+

       To install the package do the following:
        a) Open the msys2.exe bash shell 
        b) Run the following command:
               pacman -S <Package name in the table above>
    
 OR
    2) The second option is to install the full Clang tool chain as follows:
        a) Open the msys2.exe bash shell 
        b) Run the following command:
               pacman -S mingw-w64-clang-x86_64-toolchain


MSYS2 Compiler - MinGW32
-------------------------
There are two main options to install the clangd.exe as follows:
    1) The first option in order to  minimise disk space is to install the Clang extra tools using one of the following packages:
        +------------------------------------------+------------------------+
        |               Package                    | Clangd executable      |
        +------------------------------------------+------------------------+
        | mingw-w64-clang-i686-clang-tools-extra   | clang32/bin/clangd.exe |
        | mingw-w64-i686-clang-tools-extra         | mingw32/bin/clangd.exe |
        +------------------------------------------+------------------------+

       To intall the package do the following:
        a)  Open the msys2.exe bash shell 
        b) Run the following command:
               pacman -S <Package name in the table above>
    
 OR
    2) The second option is to intall the full Clang tool chain as follows:
        a)  Open the msys2.exe bash shell 
        b) Run the following command:
               pacman -S mingw-w64-clang-i686-toolchain


Notes from the CodeBlocks forum to avoid mixing incompatible GCC/ClangD executables.
====================================================================================
DO NOT mix GCC and ClangD from different MSYS2 bin directories!!!! You have been warned.

If you are using the MSYS2 GCC compiler then you need to use the ClangD from the same directory.

If you are using the MinGW64 GCC compiler then you need to install the "mingw-w64-i686-clang-tools-extra" 
package and use the ClangD.exe from the "msys64\mingw64\bin" directory.

If you are using the MinGW32 GCC compiler then you need to install the "mingw-w64-x86_64-clang-tools-extra" 
package and use the ClangD.exe from the "msys64\mingw32\bin" directory.

If you are using the Clang64 compiler then you need to install the "mingw-w64-clang-x86_64-clang-tools-extra" 
package and use the ClangD.exe from the "msys64\clang64\bin" directory.

If you are using the Clang32 compiler then you need to install the "mingw-w64-clang-i686-clang-tools-extra" 
package and use the ClangD.exe from the "msys64\clang32\bin" directory.


If you use inconsistent files then you should get allot  LSP diagnostics messages about 
the errors.



CYGWIN Compiler
---------------
TBA - Please supply if known and possible.

MINGW original Compiler
---------------
TBA - Please supply if known and possible.

MINGW32 Compiler
---------------
TBA - Please supply if known and possible.

MINGW64 Compiler
---------------
TBA - Please supply if known and possible.

TDM32 Compiler
---------------
TBA - Please supply if known and possible.

TDM64 Compiler
---------------
TBA - Please supply if known and possible.

WINLIBS Compiler
---------------
https://winlibs.com/
TBA - Please supply if known and possible.

GTX Compiler
---------------
https://github.com/Guyutongxue/mingw-release
TBA - Please supply if known and possible.


Ubuntu 22.04 on WSL2
====================
a) sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
b) sudo apt-get install clangd
