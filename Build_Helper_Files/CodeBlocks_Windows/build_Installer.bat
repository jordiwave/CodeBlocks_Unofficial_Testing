@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"

set BUILD_BITS=%1
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
if exist "..\..\src\devel3*_32" set BUILD_BITS=32
if exist "..\..\src\devel3*_64" set BUILD_BITS=64
if exist "..\src\devel3*_32" set BUILD_BITS=32
if exist "..\src\devel3*_64" set BUILD_BITS=64
if exist "src\devel3*_32" set BUILD_BITS=32
if exist "src\devel3*_64" set BUILD_BITS=64
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
set BUILD_BITS=64

:BuildBits_Okay
if "%WX_DIR_VERSION%" == "" (
    if "%2" == "" (
        if not "%WX_DIR_VERSION%" == "" goto WX_DIR_VERSION_Okay
        if exist "..\..\src\devel31*" set WX_DIR_VERSION=31
        if exist "..\..\src\devel32*" set WX_DIR_VERSION=32
        if exist "..\src\devel31*" set WX_DIR_VERSION=31
        if exist "..\src\devel32*" set WX_DIR_VERSION=32
        if exist "src\devel31*" set WX_DIR_VERSION=31
        if exist "src\devel32*" set WX_DIR_VERSION=32
    ) else (
        set WX_DIR_VERSION=%2
    )
)
if "%WX_DIR_VERSION%" == "" goto WXVersionError

:start
if NOT exist "..\..\windows_installer\Build_NSIS_%BUILD_BITS%bit.bat" goto NoWindowsInstallerBatchFileFound
if not exist "..\..\src\output%WX_DIR_VERSION%_%BUILD_BITS%" goto NoOutputDirectoryFound


cd ..\..\windows_installer
@echo call Build_NSIS_%BUILD_BITS%bit.bat %WX_DIR_VERSION%
call Build_NSIS_%BUILD_BITS%bit.bat %WX_DIR_VERSION%
@goto Finish

:NoWindowsInstallerBatchFileFound
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^|                                                                                     ^|
@echo ^| Error: Could not find following NSIS installer batch file:                          ^|
@echo ^|             ..\..\windows_installer\Build_NSIS_%BUILD_BITS%bit.bat         ^|
@echo ^|                                                                                     ^|
@echo ^|  Please fix the error and try again.                                                ^|
@echo ^|                                                                                     ^|
@echo ^+-------------------------------------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=1
@goto Finish


:BuildBitError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|                                                      ^|
@echo ^| Error: NO Windows '32' or '64' parameter specified.  ^|
@echo ^|                                                      ^|
@echo ^|   Please run again with the first parameter 32 or 64 ^|
@echo ^|                                                      ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=2
@goto Finish

:WXVersionError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|                                                      ^|
@echo ^| Error: NO wxWidget version found.                    ^|
@echo ^|                                                      ^|
@echo ^| Please run again with the second parameter being     ^|
@echo ^|   the wxWidget version like 31 or 32                  ^|
@echo ^|                                                      ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=3
@goto Finish


:NoOutputDirectoryFound
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^|                                                                                     ^|
@echo ^| Error: Could not find following CodeBlocks output directory:                        ^|
@echo ^|           ..\..\src\output%WX_DIR_VERSION%_%BUILD_BITS%         ^|
@echo ^|                                                                                     ^|
@echo ^|                 Please fix the error and try again.                                 ^|
@echo ^|                                                                                     ^|
@echo ^+-------------------------------------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=4
@goto Finish


:Finish
@cd /d %CurrentDir%
@endlocal