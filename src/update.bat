@echo on

@set DEBUG=On
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

@set BUILD_BITS=%1
@if exist "src\devel31_%BUILD_BITS%" goto BuildBits_Okay
@if "%BUILD_BITS%" == "32" goto BuildBits_Okay
@if "%BUILD_BITS%" == "64" goto BuildBits_Okay
@if exist "src\devel31_32" set BUILD_BITS=32
@if exist "src\devel31_64" set BUILD_BITS=64
@if "%BUILD_BITS%" == "32" goto BuildBits_Okay
@if "%BUILD_BITS%" == "64" goto BuildBits_Okay
@goto BuildBitError

:BuildBits_Okay

@echo Coniguring for: %BUILD_BITS% bits

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
@set CB_DEVEL_DIR=%CB_ROOT%\src\devel31_%BUILD_BITS%
@set CB_OUTPUT_DIR=%CB_ROOT%\src\output31_%BUILD_BITS%
@set CB_DEVEL_RESDIR=%CB_DEVEL_DIR%\share\CodeBlocks
@set CB_OUTPUT_RESDIR=%CB_OUTPUT_DIR%\share\CodeBlocks

@REM =============================================

@if not exist %CB_DEVEL_DIR% (
    @echo "ERROR: The developemnt directory does not exist: %CB_DEVEL_DIR% . Please fix and try again."
    @goto Finish
)

@if exist %CB_OUTPUT_DIR% (
    @echo "The output %CB_OUTPUT_DIR% exists, dleteing it."
    rmdir /s /q %CB_OUTPUT_DIR%
)

@REM =============================================

@if "%DEBUG%" == "On" echo "Creating output directory and copying the dev tree to the output"
@call :mkdirSilent "%CB_OUTPUT_DIR%"
@xcopy /S /D /y "%CB_DEVEL_DIR%\*" "%CB_OUTPUT_DIR%\"  > nul     

@REM =============================================
@if "%DEBUG%" == "On" echo "Striping EXE and DLL files in the output directory tree"
@rem @for /f "usebackq delims=^=^" %%a in (`"dir "%CB_OUTPUT_DIR%\*.exe" /b/s" 2^>nul`) do @%STRIP_EXE% %%a  > nul
@rem @for /f "usebackq delims=^=^" %%a in (`"dir "%CB_OUTPUT_DIR%\*.dll" /b/s" 2^>nul`) do @%STRIP_EXE% %%a  > nul

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
@if exist "%CB_HANDLER_DIR%" xcopy /y "%CB_HANDLER_DIR%\*.dll" "%CB_OUTPUT_DIR%\" > nul

@REM =============================================

@goto Finish

:BuildBitError
@echo.
@echo ^+-------------------------------------------------------^+
@echo ^| Error: Could not detect either of the followinf dirs: ^|
@echo ^|          - src\devel31_32                             ^|
@echo ^|          - src\devel31_64                             ^|
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