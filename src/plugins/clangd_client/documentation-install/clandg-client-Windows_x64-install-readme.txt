Windows Clangd-Client install process:
=====================================

MSYS2 Compiler
---------------
1) Install MSYS 2 clang packages via the msys2.exe bash shell:
    pacman -S mingw-w64-clang-x86_64-toolchain
2) Install the plugin:
    a) Open the plugin manager via the "Plugins->Manage Plugins.." menu option.
    b) Press the "Install new" button on the right.
    c) Specifiy the plugin "clangd_client.cbplugin" that you downloaded from this directory
    d) Once loaded restart C::B for the plugin to start correctly.
2) Configure the plugin for use:
    a) In the Settings->editor select Clangd_Client settings.
    b) In the C/C++ parser tab change the "LLVM directory" to specify the clangd.exe you have installed:
    C:\msys64\clang64\bin\clangd.exe

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