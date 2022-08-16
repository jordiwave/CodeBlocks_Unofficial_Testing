@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

CALL :NORMALIZEPATH "%cd%\.."
set CB_ROOT_DIR=%RETVAL%
echo "CB_ROOT_DIR : %CB_ROOT_DIR%

@rem load the  NIGHTLY_BUILD_SVN variable from the txt file
for /f "delims== tokens=1,2" %%G in (Build_Version_Number.txt) do set %%G=%%H

if "%1" == "Debug" set NIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%_Debug
if "%1" == "Release" set NIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%_Release
if not "%WX_DIR_VERSION%" == "" goto MakeInstaller

call :Configure_WX_Variables
if "%BUILD_BITS%" == ""  goto WXDetectError
if "%WX_DIR_VERSION%" == "" goto WXDetectError
if not exist "%CB_ROOT_DIR%\src\devel%WX_DIR_VERSION%_%BUILD_BITS%" goto NoOutputDirectoryFound

:MakeInstaller
if exist "%CB_ROOT_DIR%\src\devel%WX_DIR_VERSION%_%BUILD_BITS%\wxmsw*_core_*.dll" set NIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%_MSYS2
if "%GITHUB_ACTIONS%" == "true" (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=%BUILD_BITS%" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_Simple.nsi"
) else (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=%BUILD_BITS%" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_UMUI.nsi"
)
@goto Finish

:Configure_WX_Variables
    set BUILD_BITS=
    if exist "%CB_ROOT_DIR%\src\devel31_32" call :CONFIGURE_WX31_32_VARS
    if exist "%CB_ROOT_DIR%\src\devel31_64" call :CONFIGURE_WX31_64_VARS
    if exist "%CB_ROOT_DIR%\src\devel32_32" call :CONFIGURE_WX32_32_VARS
    if exist "%CB_ROOT_DIR%\src\devel32_64" call :CONFIGURE_WX32_64_VARS
    EXIT /B

:CONFIGURE_WX31_32_VARS
    set BUILD_BITS=32
    set WXWIDGET_VERSION=3.1.7
    set WX_DIR_VERSION=31
    EXIT /B
:CONFIGURE_WX31_64_VARS
    set BUILD_BITS=64
    set WXWIDGET_VERSION=3.1.7
    set WX_DIR_VERSION=31
    EXIT /B
:CONFIGURE_WX32_32_VARS
    set BUILD_BITS=32
    set WXWIDGET_VERSION=3.2.0
    set WX_DIR_VERSION=32
    EXIT /B
:CONFIGURE_WX32_64_VARS
    set BUILD_BITS=64
    set WXWIDGET_VERSION=3.2.0
    set WX_DIR_VERSION=32
    EXIT /B

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

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
    @set RETURN_ERROR_LEVEL=1
    @goto Finish

:WXDetectError
    @echo.
    @echo ^+---------------------------------------------------------------------^+
    @echo ^| Error: Cannot detected src\devel3x_32 or src\devel3x_64 directory.  ^|
    @echo ^|                 Please fix and try again                            ^|
    @echo ^+---------------------------------------------------------------------^+
    @echo.
    @echo.
    @set RETURN_ERROR_LEVEL=2
    @goto Finish


:Finish
if not "%RETURN_ERROR_LEVEL%" == "" (CALL)
@endlocal