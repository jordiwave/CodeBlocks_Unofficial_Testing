Windows Clangd executable install process:
==========================================

There are three main options to install the clangd.exe:
1) Install the LLVM compiler.
2) Manully extract the required files from the LLVM compiler.
3) Install the Clangd package for the Windows compiler you are using if it is available.

The  process for the three options above are detailed below.

Install the LLVM compiler
=========================
1) Download the latest (non RC/Beta)LLVM Windows executable for your OS (Win32 or Win64) from the following Github LLVM download page:
      https://github.com/llvm/llvm-project/releases

   As of Jan 2022 the Windows files are names as follows:
      LLVM-<version>-win64.exe
      LLVM-<version>-win32.exe
   where <version> is the LLVM version, like 13.0.0 or 13.0.1.

2) Run the LLVM-<version>-win<xx>.exe you downlaoded to install the LLVM compiler.



Manually Extract File from LLVM compiler
=========================================
1) Download the latest (non RC/Beta)LLVM Windows executable for your OS (Win32 or Win64) from the following Github LLVM download page:
      https://github.com/llvm/llvm-project/releases

   As of Jan 2022 the Windows files are names as follows:
      LLVM-<version>-win64.exe
      LLVM-<version>-win32.exe
   where <version> is the LLVM version, like 13.0.0 or 13.0.1.
2) Unzip the LLVM-<version>-win<xx>.exe file you downlaoded using 7ZIP or your prefered ZIP program into a sub directory
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
instructions below for the specific compiler you haev installed:

MSYS2 Compiler - MinGW64
-------------------------
a) Install MSYS 2 clang packages via the msys2.exe bash shell:
    pacman -S mingw-w64-clang-x86_64-toolchain

MSYS2 Compiler - MinGW32
-------------------------
a) Install MSYS 2 clang packages via the msys2.exe bash shell:
    pacman -S mingw-w64-clang-i686-toolchain

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