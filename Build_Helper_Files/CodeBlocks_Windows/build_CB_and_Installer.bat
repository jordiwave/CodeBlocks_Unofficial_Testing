@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDirectory="%CD%"

@rem setup global variables
call build_set_WXWidget_variables.bat

:BuildBits_Okay
set BUILD_OUTPUT_DIR=devel%WX_DIR_VERSION%_%BUILD_BITS%
@echo call build_CodeBlocks_via_GUI.bat %BUILD_BITS%
call build_CodeBlocks_via_GUI.bat %BUILD_BITS%
@IF %ERRORLEVEL% NEQ 0 (
    @echo build_CodeBlocks_via_GUI.bat returned an ERRORLEVEL of %ERRORLEVEL% 
    @goto CompileError
)

@if not exist "..\..\src\%BUILD_OUTPUT_DIR%\CodeBlocks.exe" (
    @echo not exist "..\..\src\%BUILD_OUTPUT_DIR%\CodeBlocks.exe"
    @goto CompileError
)
@if not exist "..\..\src\%BUILD_OUTPUT_DIR%\codeblocks.dll" (
    @echo not exist "..\..\src\%BUILD_OUTPUT_DIR%\codeblocks.dll"
    @goto CompileError
)
@if not exist "..\..\src\%BUILD_OUTPUT_DIR%\Addr2LineUI.exe" (
    @echo not exist "..\..\src\%BUILD_OUTPUT_DIR%\Addr2LineUI.exe"
    @goto CompileError
    )
@if not exist "..\..\src\%BUILD_OUTPUT_DIR%\share\codeblocks\plugins\ToolsPlus.dll" (
    @echo not exist "..\..\src\%BUILD_OUTPUT_DIR%\share\codeblocks\plugins\ToolsPlus.dll"
    @goto CompileError
)
@if not exist "..\..\src\%BUILD_OUTPUT_DIR%\libstdc++-6.dll" (
    @echo not exist "..\..\src\%BUILD_OUTPUT_DIR%\libstdc++-6.dll"
    @goto CompileError
)

@echo call build_Installer.bat %BUILD_BITS% %WX_DIR_VERSION%
call build_Installer.bat %BUILD_BITS% %WX_DIR_VERSION%
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

:WindowsOuputCreateError
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^| Error: Code::Blocks call to WindowsOuputCreateError.bat failed. Please fix the error and try again. ^|
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
