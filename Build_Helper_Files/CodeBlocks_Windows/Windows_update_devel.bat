@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal
SET CurrentDir="%CD%"

for /f "tokens=4-7 delims=[.] " %%i in ('ver') do (if %%i==Version (set WIN_Version=%%j.%%k) else (set WIN_Version=%%i.%%j))

if exist "..\..\src\devel32_32" call :CONFIGURE_WX32_32_VARS
if exist "..\..\src\devel32_64" call :CONFIGURE_WX32_64_VARS

if exist "..\src\devel32_32" call :CONFIGURE_WX32_32_VARS
if exist "..\src\devel32_64" call :CONFIGURE_WX32_64_VARS

if exist "src\devel32_32" call :CONFIGURE_WX32_32_VARS
if exist "src\devel32_64" call :CONFIGURE_WX32_64_VARS

if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
goto WXDetectError

:BuildBits_Okay
@rem change to the CB source root directory
if exist bootstrap cd src
if exist ..\..\bootstrap cd ..\..\src
if exist ..\..\..\bootstrap cd ..\..\..\src
set BUILD_DEV_OUTPUT_DIR=%CD%\devel%WX_DIR_VERSION%_%BUILD_BITS%

@rem GCC_ROOT is the root directory of the compiler you are going to use to build C::B with
set GCC_ROOT=C:\msys64\mingw%BUILD_BITS%
@rem set GCC_ROOT=C:\mingw64-winlib
@rem set GCC_ROOT=C:\TDM-GCC-64

@rem WXWIN is the wxWidgets root directory that you have all ready built from source 
if exist "..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"       CALL :NORMALIZEPATH "..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"
if exist "..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"    CALL :NORMALIZEPATH "..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"
if exist "..\..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%" CALL :NORMALIZEPATH "..\..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"
SET WXWIN=%RETVAL%
if not exist "%WXWIN%" goto ErrNowxWidget

@rem ----------------------------------------------------------------------------
@rem Copy the compiler DLL and wxWidget DLL's into the %BUILD_DEV_OUTPUT_DIR% directory
@rem ----------------------------------------------------------------------------
@echo Copying compiler and wxWidget DLL's into the %BUILD_DEV_OUTPUT_DIR% directory.

if "%BUILD_BITS%" == "32" call :CopyMSYS2File libgcc_s_dw2-1.dll
if "%BUILD_BITS%" == "64" call :CopyMSYS2File libgcc_s_seh-1.dll
@rem The next DLL is required for some GCC compilers, but not for others. Either way copy it is if exists.
if "%BUILD_BITS%" == "64" call :CopyMSYS2File libgcc_s_seh_64-1.dll

call :CopyMSYS2File libwinpthread-1.dll
call :CopyMSYS2File libstdc++-6.dll 
call :CopyMSYS2File libhunspell-*.dll
call :CopyMSYS2File libbz2*.dll   
call :CopyMSYS2File zlib*.dll

if exist "%WXWIN%\lib\gcc_dll\wxmsw*_gcc_cb.dll"    copy /Y "%WXWIN%\lib\gcc_dll\wxmsw*_gcc_cb.dll"    %BUILD_DEV_OUTPUT_DIR% > nul
if exist "%WXWIN%\lib\gcc_dll\wxmsw*_gl_gcc_cb.dll" copy /Y "%WXWIN%\lib\gcc_dll\wxmsw*_gl_gcc_cb.dll" %BUILD_DEV_OUTPUT_DIR% > nul


if not exist "%BUILD_DEV_OUTPUT_DIR%\exchndl.dll" (
    if "%WIN_Version%" == "10.0" copy /Y "exchndl\Win_10\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "6.3" copy /Y "exchndl\Win_7\win%BUILD_BITS%\bin\*.*"  %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "6.2" copy /Y "exchndl\Win_7\win%BUILD_BITS%\bin\*.*"  %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "5.2" copy /Y "exchndl\Win_xp\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "5.1" copy /Y "exchndl\Win_xp\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if not exist "%BUILD_DEV_OUTPUT_DIR%\exchndl.dll" copy /Y "exchndl\Win_7\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    )

@rem ----------------------------------------------------------------------------
@rem Check if build worked
@rem ----------------------------------------------------------------------------
if not exist "%BUILD_DEV_OUTPUT_DIR%\codeblocks.exe"                goto CompileError
if not exist "%BUILD_DEV_OUTPUT_DIR%\libcodeblocks.la"  (
    if not exist "%BUILD_DEV_OUTPUT_DIR%\*codeblocks.dll"           goto CompileError
    if not exist "%BUILD_DEV_OUTPUT_DIR%\share\codeblocks\todo.zip" goto CompileErrorZIP
    )

@rem ------------------------------------------------------------------------------------------
@rem Check plugin DLL files were built
@rem ------------------------------------------------------------------------------------------
if not exist "%BUILD_DEV_OUTPUT_DIR%\share\codeblocks\plugins\*.dll" goto NoPluginDLLFilesFound

@rem ---------------------------------------------------------------------------------------------------------
@rem Check if MSYS 2 using bootstrap/configure/make/make install process was used and if it was cleanup files
@rem ---------------------------------------------------------------------------------------------------------
if not exist "%BUILD_DEV_OUTPUT_DIR%\*.la" goto Finish

SET PrevDirectory="%CD%"
cd "%BUILD_DEV_OUTPUT_DIR%"
for /f "usebackq delims=^=^" %%a in (`"dir "*.la" /b/s" 2^>nul`) do del /Q %%a
cd /d %PrevDirectory%
@goto Finish

@rem ------------------------------------------------------------------------------------------

:WXDetectError
    @echo.
    @echo ^+---------------------------------------------------------------------^+
    @echo ^| Error: Cannot detected src\devel3x_32 or src\devel3x_64 directory.  ^|
    @echo ^|                 Please fix and try again                            ^|
    @echo ^+---------------------------------------------------------------------^+
    @echo.
    @echo.
    @set RETURN_ERROR_LEVEL=1
    @goto Finish

:CompileError
    @echo.
    @echo.
    @echo ^+----------------------------------------------------^+
    @echo ^| Error: Code::Blocks compile error was detected.    ^|
    @echo ^|        Please fix the error and try again.         ^|
    @echo ^+----------------------------------------------------^+
    @echo.
    @echo.
    @set RETURN_ERROR_LEVEL=2
    goto Finish

:CompileErrorZIP
    @echo.
    @echo.
    @echo ^+----------------------------------------------------^+
    @echo ^| Error: Code::Blocks compile error was detected.    ^|
    @echo ^|        Mssing zip file(s) was detected.            ^|
    @echo ^|        Please fix the error and try again.         ^|
    @echo ^+----------------------------------------------------^+
    @echo.
    @echo.
    @set RETURN_ERROR_LEVEL=3
    goto Finish

:NoPluginDLLFilesFound
    @echo.
    @echo.
    @echo ^+------------------------------------------------------^+
    @echo ^| Error: Code::Blocks Plugin DLL files not found in:   ^|
    @echo ^|          "%BUILD_DEV_OUTPUT_DIR%\share\codeblocks\plugins"       ^|
    @echo ^+------------------------------------------------------^+
    @echo.
    @echo.
    @set RETURN_ERROR_LEVEL=4
    goto Finish

:ErrNowxWidget
    @echo.
    @echo.
    @echo ^+------------------------------------------------------------------------------------------------------------^+
    @echo ^|     Error: NO WXWIN "%WXWIN%" sub directory found                                                                 ^|
    @echo ^+  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -^|
    @echo ^|  BUILD_BITS=%BUILD_BITS%                                                                                             ^|
    @echo ^|  WXWIDGET_VERSION=%WXWIDGET_VERSION%                                                                                    ^|
    @echo ^|  WX_DIR_VERSION=%WX_DIR_VERSION%                                                                                         ^|
    @echo ^+------------------------------------------------------------------------------------------------------------^+
    @echo. 
    @echo.
    @set RETURN_ERROR_LEVEL=5
    goto Finish

:CONFIGURE_WX32_32_VARS
    set BUILD_BITS=32
    set WXWIDGET_VERSION=3.2.1
    set WX_DIR_VERSION=32
    EXIT /B
:CONFIGURE_WX32_64_VARS
    set BUILD_BITS=64
    set WXWIDGET_VERSION=3.2.1
    set WX_DIR_VERSION=32
    EXIT /B

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

:CopyMSYS2File
	if exist "%GCC_ROOT%\bin\%~1" copy /Y "%GCC_ROOT%\bin\%~1" %BUILD_DEV_OUTPUT_DIR% > nul
	EXIT /B

:Finish
cd /d %CurrentDir%
@endlocal
