@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"

set BUILD_BITS=%1
IF "%BUILD_BITS%" == "32" goto start
IF "%BUILD_BITS%" == "64" goto start
goto BuildBitError


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