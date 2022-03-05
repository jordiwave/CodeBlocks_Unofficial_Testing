@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"

set BUILD_BITS=%1
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
if exist "..\..\src\devel31_32" set BUILD_BITS=32
if exist "..\..\src\devel31_64" set BUILD_BITS=64
if exist "src\devel31_32" set BUILD_BITS=32
if exist "src\devel31_64" set BUILD_BITS=64
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
goto BuildBitError

:BuildBits_Okay

:start
if NOT exist "..\..\windows_installer\Build_NSIS_%BUILD_BITS%bit.bat" goto NoWindowsInstallerBatchFileFound

cd ..\..\windows_installer
call Build_NSIS_%BUILD_BITS%bit.bat
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
@goto Finish


:BuildBitError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|                                                      ^|
@echo ^| Error: NO Windows '32' or '64' parameter specified.  ^|
@echo ^|                                                      ^|
@echo ^|        Please run again with a parameter             ^|
@echo ^|                                                      ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@goto Finish

:Finish
@cd /d %CurrentDir%
@endlocal