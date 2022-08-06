@echo on

@rem ----------------------------------------------------------------------
@rem Parameter 1 is either 32 or 64 for build type for 32 or 64 bits
@rem Parameter 2 is the WxWidget version 31 or 32
@rem ----------------------------------------------------------------------

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"

if not "%1" == "" if "%2" == "" (
    set BUILD_BITS=%1
    set WX_DIR_VERSION=%2
    set CB_OUTPUT_DIR=%1\output31_%2
    goto BuildBits_Okay
)    

call :FIND_OUTPUT_DIR ..\..\src 32
call :FIND_OUTPUT_DIR ..\..\src 64
call :FIND_OUTPUT_DIR ..\src 32
call :FIND_OUTPUT_DIR ..\src 64
call :FIND_OUTPUT_DIR src 32
call :FIND_OUTPUT_DIR src 64
if "%BUILD_BITS%" == ""  goto NoOutputDirectoryFound
if "%CB_OUTPUT_DIR%" == "" goto NoOutputDirectoryFound
if "%WX_DIR_VERSION%" == "" goto NoOutputDirectoryFound
goto BuildBits_Okay

:FIND_OUTPUT_DIR 
    SET OUTPUT_OR=
    if "%BUILD_BITS%" == "" SET OUTPUT_OR=Yes
    if "%CB_OUTPUT_DIR%" == "" SET OUTPUT_OR=Yes
    if "%OUTPUT_OR%" == "Yes" (
        if exist "%1\output31_%2" (
            set BUILD_BITS=%2
            set WX_DIR_VERSION=31
            set CB_OUTPUT_DIR=%1\output31_%2
        ) else if exist "%1\output32_%2" (
            set BUILD_BITS=%2
            set WX_DIR_VERSION=32
            set CB_OUTPUT_DIR=%1\output32_%2
        )
    )    
    SET OUTPUT_OR=
    EXIT /B

:BuildBits_Okay
CALL :NORMALIZEPATH %CB_OUTPUT_DIR%
SET CB_OUTPUT_DIR=%RETVAL%

CALL :NORMALIZEPATH "..\..\windows_installer\Build_NSIS.bat"
SET NSIS_BAT_FILE=%RETVAL%

if not exist "%NSIS_BAT_FILE%" goto NoWindowsInstallerBatchFileFound
if not exist "%CB_OUTPUT_DIR%" goto NoOutputDirectoryFound

cd ..\..\windows_installer
@echo Create debug installer by 'call %NSIS_BAT_FILE% Debug'
@call %NSIS_BAT_FILE% Debug

@cd /d %CurrentDir%
set GCC_STRIP=C:\msys64\mingw%BUILD_BITS%\bin\strip.exe
if exist %GCC_STRIP% (
    @echo "Stripping exe and DLL fles in %CB_OUTPUT_DIR%"
    @for /f "usebackq delims=^=^" %%a in (`"dir "%CB_OUTPUT_DIR%\*.exe" /b/s" 2^>nul`) do @%GCC_STRIP% %%a  > nul
    @for /f "usebackq delims=^=^" %%a in (`"dir "%CB_OUTPUT_DIR%\*.dll" /b/s" 2^>nul`) do @%GCC_STRIP% %%a  > nul
) else goto NoStripExeFound

cd ..\..\windows_installer
@echo Create release installer by 'call %NSIS_BAT_FILE% Release'
@call %NSIS_BAT_FILE% Release
@goto Finish

:NoWindowsInstallerBatchFileFound
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^|                                                                                     ^|
@echo ^| Error: Could not find following NSIS installer batch file:                          ^|
@echo ^|             ..\..\windows_installer\Build_NSIS.bat         ^|
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


:NoStripExeFound
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|                                                      ^|
@echo ^| Error: NO %GCC_STRIP% found.                    ^|
@echo ^|                                                      ^|
@echo ^| Please fix and run again ^|
@echo ^|                                                      ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=4
@goto Finish


:NoOutputDirectoryFound
@echo.
@echo.
if "%WX_DIR_VERSION%" == "" set WX_DIR_VERSION=??
if "%BUILD_BITS%" == "" set BUILD_BITS=??
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
@set RETURN_ERROR_LEVEL=5
@goto Finish

:NoOutputDirDebugFound 
@echo.
@echo.
@echo ^+-------------------------------------------------------------------------------------^+
@echo ^|                                                                                     ^|
@echo ^| Error: Could not find following CodeBlocks output directory:                        ^|
@echo ^|           %CB_OUTPUT_DIR_DEBUG%                                            ^|
@echo ^|                                                                                     ^|
@echo ^|                 Please fix the error and try again.                                 ^|
@echo ^|                                                                                     ^|
@echo ^+-------------------------------------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=6
@goto Finish

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

:Finish
@cd /d %CurrentDir%
@endlocal