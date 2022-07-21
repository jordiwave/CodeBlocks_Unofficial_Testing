@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal
SET CurrentDir="%CD%"
@rem set WXWIDGET_VERSION=3.1.7
@rem set WX_DIR_VERSION=31
set WXWIDGET_VERSION=3.2.0
set WX_DIR_VERSION=32

for /f "tokens=4-7 delims=[.] " %%i in ('ver') do (if %%i==Version (set WIN_Version=%%j.%%k) else (set WIN_Version=%%i.%%j))

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
set BUILD_DEV_OUTPUT_DIR=devel%WX_DIR_VERSION%_%BUILD_BITS%
@rem change to the CB source root directory
if exist bootstrap cd src
if exist ..\..\bootstrap cd ..\..\src
if exist ..\..\..\bootstrap cd ..\..\..\src

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
@rem Check if build worked
@rem ----------------------------------------------------------------------------
if not exist "%BUILD_DEV_OUTPUT_DIR%\codeblocks.exe"                                                                goto CompileError
if not exist "%BUILD_DEV_OUTPUT_DIR%\libcodeblocks.la"  if not exist "%BUILD_DEV_OUTPUT_DIR%\codeblocks.dll"            goto CompileError
if not exist "%BUILD_DEV_OUTPUT_DIR%\libcodeblocks.la"  if not exist "%BUILD_DEV_OUTPUT_DIR%\share\codeblocks\todo.zip" goto CompileError

@rem ----------------------------------------------------------------------------
@rem Copy the compiler DLL and wxWidget DLL's into the %BUILD_DEV_OUTPUT_DIR% directory
@rem ----------------------------------------------------------------------------
@echo Copying compiler and wxWidget DLL's into the %BUILD_DEV_OUTPUT_DIR% directory.
if "%BUILD_BITS%" == "32" if exist "%GCC_ROOT%\bin\libgcc_s_dw2-1.dll"    copy /Y "%GCC_ROOT%\bin\libgcc_s_dw2-1.dll"        %BUILD_DEV_OUTPUT_DIR% > nul
if "%BUILD_BITS%" == "64" if exist "%GCC_ROOT%\bin\libgcc_s_seh-1.dll"    copy /Y "%GCC_ROOT%\bin\libgcc_s_seh-1.dll"        %BUILD_DEV_OUTPUT_DIR% > nul
@rem The next DLL is required for some GCC compilers, but not for others. Either way copy it is if exists.
if "%BUILD_BITS%" == "64" if exist "%GCC_ROOT%\bin\libgcc_s_seh_64-1.dll" copy /Y "%GCC_ROOT%\bin\libgcc_s_seh_64-1.dll"     %BUILD_DEV_OUTPUT_DIR% > nul
if exist "%GCC_ROOT%\bin\libwinpthread-1.dll"       copy /Y "%GCC_ROOT%\bin\libwinpthread-1.dll"       %BUILD_DEV_OUTPUT_DIR% > nul
if exist "%GCC_ROOT%\bin\libstdc++-6.dll"           copy /Y "%GCC_ROOT%\bin\libstdc++-6.dll"           %BUILD_DEV_OUTPUT_DIR% > nul

if exist "%WXWIN%\lib\gcc_dll\wxmsw*_gcc_cb.dll"    copy /Y "%WXWIN%\lib\gcc_dll\wxmsw*_gcc_cb.dll"    %BUILD_DEV_OUTPUT_DIR% > nul
if exist "%WXWIN%\lib\gcc_dll\wxmsw*_gl_gcc_cb.dll" copy /Y "%WXWIN%\lib\gcc_dll\wxmsw*_gl_gcc_cb.dll" %BUILD_DEV_OUTPUT_DIR% > nul

if not exist "%BUILD_DEV_OUTPUT_DIR%\exchndl.dll" (
    if "%WIN_Version%" == "10.0" copy /Y "exchndl\win_10\bin\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "6.3" copy /Y "exchndl\win_7\bin\win%BUILD_BITS%\bin\*.*"  %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "6.2" copy /Y "exchndl\win_7\bin\win%BUILD_BITS%\bin\*.*"  %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "5.2" copy /Y "exchndl\win_xp\bin\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if "%WIN_Version%" ==  "5.1" copy /Y "exchndl\win_xp\bin\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
    if not exist "%BUILD_DEV_OUTPUT_DIR%\exchndl.dll" copy /Y "exchndl\win_7\bin\win%BUILD_BITS%\bin\*.*" %BUILD_DEV_OUTPUT_DIR% > nul
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

:renameDLL
::Cuts off 1st three chars
:: Delete renamed file if it exists as the new file could have changes you want!!!
if exist "%fnameDLL:~3%" del "%fnameDLL:~3%"
ren "%fnameDLL%" "%fnameDLL:~3%"
goto :eof

:BuildBitError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^| Error: NO Windows '32' or '64' parameter specified.  ^|
@echo ^|        Please run again with a parameter             ^|
@echo ^+------------------------------------------------------^+
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

:NoPluginDLLFilesFound
@echo.
@echo.
@echo ^+------------------------------------------------------^+
@echo ^| Error: Code::Blocks Plugin DLL files not found in:   ^|
@echo ^|          "%BUILD_DEV_OUTPUT_DIR%\share\codeblocks\plugins"       ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=3
goto Finish

:ErrNowxWidget
@echo.
@echo.
@echo ^+------------------------------------------------------------------------------------------------------------^+
@echo ^|     Error: NO WXWIN "%WXWIN%" sub directory found  ^|
@echo ^+------------------------------------------------------------------------------------------------------------^+
@echo. 
@echo.
@set RETURN_ERROR_LEVEL=4
goto Finish

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

:Finish
cd /d %CurrentDir%
@endlocal
