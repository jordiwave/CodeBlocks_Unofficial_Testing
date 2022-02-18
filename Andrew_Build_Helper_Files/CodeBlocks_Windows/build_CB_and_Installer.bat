@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDirectory="%CD%"

set BUILD_BITS=%1
IF "%BUILD_BITS%" == "32" goto BuildBits_Okay
IF "%BUILD_BITS%" == "64" goto BuildBits_Okay
IF "%BUILD_BITS%" == "" goto BuildBitError

:BuildBits_Okay
@echo on
call build_CodeBlocks.bat %BUILD_BITS%
IF %ERRORLEVEL% NEQ 0 (
    @echo build_CodeBlocks.bat returned an ERRORLEVEL of %ERRORLEVEL% 
    goto CompileError
)
@cd /d %CurrentDirectory%

@if not exist "..\..\src\devel31_%BUILD_BITS%\codeblocks.exe"   goto CompileError
@if not exist "..\..\src\devel31_%BUILD_BITS%\codeblocks.dll"   goto CompileError
@if not exist "..\..\src\devel31_%BUILD_BITS%\Addr2LineUI.exe"  goto CompileError
@if not exist "..\..\src\devel31_%BUILD_BITS%\share\codeblocks\plugins\ToolsPlus.dll"  goto CompileError
@if not exist "..\..\src\devel31_%BUILD_BITS%\libstdc++-6.dll"   goto CompileError

call build_Installer.bat %BUILD_BITS%
IF %ERRORLEVEL% NEQ 0 (
    @echo build_Installer.bat returned an ERRORLEVEL of %ERRORLEVEL% 
    goto InstallerError
)
@goto Finish

:NoBuildDirFound
@echo.
@echo.
@echo ^+----------------------------------------------------------------------^+
@echo ^| Error: No Build Directory Found. Please fix the error and try again. ^|
@echo ^+----------------------------------------------------------------------^+
@echo.
@echo.
@goto Finish


:CompileError
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^| Error: Code::Blocks compile error was detected. Please fix the error and try again. ^|
@echo ^+-------------------------------------------------------------------------------------^+
@echo.
@echo.
@goto Finish

:InstallerError
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^| Error: Installer error ocurred. Please fix the error and try again. ^|
@echo ^+-------------------------------------------------------------------------------------^+
@echo.
@echo.
@goto Finish

:BuildBitError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^| Error: NO Windows '32' or '64' parameter specified.  ^|
@echo ^|        Please run again with a parameter             ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@goto Finish


:Finish
@cd /d %CurrentDirectory%
@endlocal