@echo offf

@rem @set DEBUG=On

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
@setlocal

@rem SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION  
@SETLOCAL ENABLEEXTENSIONS

@REM =============================================

@SET CurrentDir="%CD%"
@if exist ..\..\..\bootstrap cd ..\..\..
@if exist ..\..\bootstrap cd ..\..
@if exist ..\bootstrap cd ..
@if not exist bootstrap (
    @ECHO Cannot find bootstrap. Exiting
    @goto Finish
)

@REM =============================================

@echo C::B Root Directory is: %CD%

@REM =============================================

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

@echo Configured for: %BUILD_BITS% bits

if not "%WX_DIR_VERSION%" == "" goto WX_DIR_VERSION_Okay
if exist "src\devel31*" set WX_DIR_VERSION=31
if exist "src\devel32*" set WX_DIR_VERSION=32
set WX_DIR_VERSION=32

:WX_DIR_VERSION_Okay
@REM =============================================

@rem WATCH this as you need to  have the set outside the second if for it to work!!!!

@if "%2" == "" (
    set STRIP_EXE=strip.exe
) else (
   set STRIP_EXE=%2\bin\strip.exes
)

@if NOT exist "%STRIP_EXE%"  @if exist C:\msys64\mingw64\bin\strip.exe  set STRIP_EXE=C:\msys64\mingw64\bin\strip.exe
@if NOT exist "%STRIP_EXE%"  @if exist C:\msys64\mingw32\bin\strip.exe  set STRIP_EXE=C:\msys64\mingw32\bin\strip.exe
@if NOT exist "%STRIP_EXE%"  @if exist C:\mingw64\bin\strip.exe         set STRIP_EXE=C:\mingw64\bin\strip.exe
@if NOT exist "%STRIP_EXE%"  @if exist C:\mingw32\bin\strip.exe         set STRIP_EXE=C:\mingw32\bin\strip.exe

@if NOT exist "%STRIP_EXE%" (
    @echo "ERROR: Cannot find %STRIP_EXE%. It is not on the path. You can supply a second paramter to specify the GCC root directory or update the path and try again."
    @goto Finish
)


@REM =============================================
@set CB_ROOT=%CD%
@set BUILD_DEV_OUTPUT_DIR=%CB_ROOT%\src\devel%WX_DIR_VERSION%_%BUILD_BITS%
@set BUILD_REL_OUTPUT_DIR=%CB_ROOT%\src\output%WX_DIR_VERSION%_%BUILD_BITS%
@set BUILD_DEV_CB_DIR=%BUILD_DEV_OUTPUT_DIR%\share\CodeBlocks
@set BUILD_REL_CB_DIR=%BUILD_REL_OUTPUT_DIR%\share\CodeBlocks

@REM =============================================

@if not exist %BUILD_DEV_OUTPUT_DIR% goto BUILD_DEV_OUTPUT_DIR_ERROR

@if exist %BUILD_REL_OUTPUT_DIR% (
    @echo "The output %BUILD_REL_OUTPUT_DIR% exists, deleteing it."
    rmdir /s /q %BUILD_REL_OUTPUT_DIR%
)

@REM =============================================

@if "%DEBUG%" == "On" echo "Creating output directory and copying the dev tree to the output"
@call :mkdirSilent "%BUILD_REL_OUTPUT_DIR%"
@xcopy /S /D /y "%BUILD_DEV_OUTPUT_DIR%\*" "%BUILD_REL_OUTPUT_DIR%\"  > nul     

@REM =============================================
@if "%DEBUG%" == "On" echo "Striping EXE and DLL files in the output directory tree"
@rem @for /f "usebackq delims=^=^" %%a in (`"dir "%BUILD_REL_OUTPUT_DIR%\*.exe" /b/s" 2^>nul`) do @%STRIP_EXE% %%a  > nul
@rem @for /f "usebackq delims=^=^" %%a in (`"dir "%BUILD_REL_OUTPUT_DIR%\*.dll" /b/s" 2^>nul`) do @%STRIP_EXE% %%a  > nul

@REM =============================================

@rem Use the correct files for the version of windows being used
@for /f "tokens=4-7 delims=[.] " %%i in ('ver') do @(@if %%i==Version (set WIN_Version=%%j.%%k) else (set WIN_Version=%%i.%%j))
@if "%WIN_Version%" == "10.0"       set CB_HANDLER_WIN_DIR=Win_10
@if "%WIN_Version%" ==  "6.3"       set CB_HANDLER_WIN_DIR=Win_7
@if "%WIN_Version%" ==  "6.2"       set CB_HANDLER_WIN_DIR=Win_7
@if "%WIN_Version%" ==  "5.2"       set CB_HANDLER_WIN_DIR=Win_XP
@if "%WIN_Version%" ==  "5.1"       set CB_HANDLER_WIN_DIR=Win_XP
@if "%CB_HANDLER_WIN_DIR%" == ""    set CB_HANDLER_WIN_DIR=Win_7

@REM Copy these files after stripping symbols otherwise CB will not start as the files will be corrupted
@set CB_HANDLER_DIR=%CB_ROOT%\src\exchndl\%CB_HANDLER_WIN_DIR%\win%BUILD_BITS%\bin
@if exist "%CB_HANDLER_DIR%" xcopy /y "%CB_HANDLER_DIR%\*.dll" "%BUILD_REL_OUTPUT_DIR%\" > nul

@REM =============================================

@goto Finish

:BUILD_DEV_OUTPUT_DIR_ERROR
@echo.
@echo ^+-------------------------------------------------------^+
@echo ^| Error: Could not detect either of the following dirs: ^|
@echo ^|          - src\devel3*_32                             ^|
@echo ^|          - src\devel3*_64                             ^|
@echo ^|                                                       ^|
@echo ^|        Please fix and try again                       ^|
@echo ^+-------------------------------------------------------^+
@echo.
@goto Finish

:BUILD_DEV_OUTPUT_DIR_ERROR
@echo.
@echo ^+-------------------------------------------------------^+
@echo ^| Error: The following directory does not exist:       ^|
@echo ^|         %BUILD_DEV_OUTPUT_DIR% ^|
@echo ^|                                                       ^|
@echo ^|        Please fix and try again                       ^|
@echo ^+-------------------------------------------------------^+
@echo.
@goto Finish

::--------------------------------------------------------
::-- Function section starts below here
::--------------------------------------------------------
rem create a directory if it doesn't exists
:mkdirSilent
@if "%DEBUG%" == "On" echo "Make dir %~1"
@if not exist "%~1" mkdir "%~1"
@GOTO:EOF

::--------------------------------------------------------

:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal