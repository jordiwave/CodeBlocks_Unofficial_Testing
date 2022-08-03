@echo off
@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@rem load the  NIGHTLY_BUILD_SVN variable from the txt file
for /f "delims== tokens=1,2" %%G in (Build_Version_Number.txt) do set %%G=%%H

if "%1" == "Striped" set NIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%_STRIPPED
if not "%WX_DIR_VERSION%" == "" goto MakeInstaller

call :FIND_OUTPUT_DIR ..\..\src 32
call :FIND_OUTPUT_DIR ..\..\src 64
call :FIND_OUTPUT_DIR ..\src 32
call :FIND_OUTPUT_DIR ..\src 64
call :FIND_OUTPUT_DIR src 32
call :FIND_OUTPUT_DIR src 64
if "%BUILD_BITS%" == ""  goto NoOutputDirectoryFound
if "%WX_DIR_VERSION%" == "" goto NoOutputDirectoryFound
goto MakeInstaller

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

:MakeInstaller
if "%GITHUB_ACTIONS%" == "true" (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=%BUILD_BITS%" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_Simple.nsi"
) else (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=%BUILD_BITS%" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_UMUI.nsi"
)
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /Qp "/DBUILD_TYPE=%BUILD_BITS%" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DCB_ADMIN_INSTALLER=True" "Installer_ISS_full_only.iss"

:Finish
@endlocal