:: @echo off

:: ----------------------------------------------------------------------------------------------------
:: This script copies the compile/link output to $(TARGET_DEVEL_DIR). Currently ...\trunk\
:: ----------------------------------------------------------------------------------------------------

if  "%1."=="." (
    echo parameter is: %1
    echo "ERROR: Post processing bat file needs $(TARGET_DEVEL_DIR) as parameter."
    goto :EOF
)
:: The TARGET_DEVEL_DIR should be $(TARGET_DEVEL_DIR))\trunk\
:: Get absolute path the $(TARGET_DEVEL_DIR) and append '\src'
set DEVEL_DIR=%~dpnx1
set DEVEL_DIR=%~dpnx1\src
echo Abspath destination is: %DEVEL_DIR%
@echo .
if not exist %DEVEL_DIR%\devel31_64\share\CodeBlocks mkdir %DEVEL_DIR%\devel31_64\share\CodeBlocks
if not exist %1 (
    echo ERROR: %1 destination dir does not exists
    goto :EOF
)
@echo on
zip -jq9 %DEVEL_DIR%\devel31_64\share\CodeBlocks\Clangd_Client.zip .\src\resources\manifest.xml .\src\resources\*.xrc
@echo .

pushd .
cd .\src\resources
zip -r9 %DEVEL_DIR%\devel31_64\share\CodeBlocks\Clangd_Client.zip images > nul
popd
@echo .

copy /Y .\bin\Clangd_Client.dll %DEVEL_DIR%\devel31_64\share\CodeBlocks\plugins\Clangd_Client.dll

