@echo off
echo off
set CB_BUILD_DIR=%1

mkdir %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject          > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\16x16  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\20x20  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\24x24  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\28x28  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\32x32  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\40x40  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\48x48  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\56x56  > nul 2>&1
mkdir   %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\64x64  > nul 2>&1

xcopy /Y /S images\fortranproject\*.* %CB_BUILD_DIR%\devel31_64\share\CodeBlocks\images\fortranproject\ > nul 2>&1
