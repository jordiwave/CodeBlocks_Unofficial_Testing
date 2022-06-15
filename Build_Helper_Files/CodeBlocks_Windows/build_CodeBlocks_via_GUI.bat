@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal
SET CurrentDir="%CD%"
set WXWIDGET_VERSION=3.1.7
set WXWIDGET_DLL_FILEVERSION=317u

@rem change to the CB source root directory
cd ..\..\src

set BUILD_BITS=%1
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
if exist "..\..\src\devel31_32" set BUILD_BITS=32
if exist "..\..\src\devel31_64" set BUILD_BITS=64
if exist "src\devel31_32" set BUILD_BITS=32
if exist "src\devel31_64" set BUILD_BITS=64
if "%BUILD_BITS%" == "32" goto BuildBits_Okay
if "%BUILD_BITS%" == "64" goto BuildBits_Okay
goto BuildBitError

:BuildBits_Okay

@rem ----------------------------------------------------------------------------
@rem Hopefully these variables are the only changes you need to configure for your 
@rem Code::Blocks source build
@rem ----------------------------------------------------------------------------

@rem WXWIN is the wxWidgets root directory that you have all ready built from source 
@rem if exist %CD%\..\wxWidgets_github set WXWIN=%CD%\..\wxWidgets_github
@rem if exist %WXWIN% goto wxWidgetCompleted
CALL :NORMALIZEPATH "..\..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"
SET WXWIN=%RETVAL%
if not exist %WXWIN% goto ErrNowxWidget

@rem GCC_ROOT is the root directory of the compiler you are going to use to build C::B with
set GCC_ROOT=C:\msys64\mingw%BUILD_BITS%
@rem set GCC_ROOT=C:\mingw64-winlib
@rem set GCC_ROOT=C:\TDM-GCC-64

@rem ============================================================================
@rem Hopefully you will not have to modify anything below
@rem ============================================================================

set RETURN_ERROR_LEVEL=0

@rem ----------------------------------------------------------------------------
@rem Setup C::B root folder for C::B *binaries* (!)
@rem ----------------------------------------------------------------------------
:CB_ROOTStart
if defined CB_ROOT goto CB_ROOTFinished
if exist "C:\Program Files\CodeBlocks_Experimental"  set CB_ROOT=C:\Program Files\CodeBlocks_Experimental
if defined CB_ROOT goto CB_ROOTFinished
if exist "C:\Program Files\CodeBlocks"               set CB_ROOT=C:\Program Files\CodeBlocks
if defined CB_ROOT goto CB_ROOTFinished
if exist "C:\Program Files (x86)\CodeBlocks"         set CB_ROOT=C:\Program Files (x86)\CodeBlocks
if defined CB_ROOT goto CB_ROOTFinished

:CB_ROOTFinished
if not exist %CB_ROOT%\codeblocks.exe goto ErrNoCBExe

@cls
@echo.
@echo ^+======================================================================================^+
@echo ^|                                                                                      ^|
@echo ^|      Code::Blocks build script for Windows x%BUILD_BITS% and WxWidgets 3.1.x                   ^|
@echo ^|                                                                                      ^|
@echo ^+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ^+
@echo ^|                                                                                      ^|
@echo ^|  Code::Block dir: %CB_ROOT%                           ^|
@echo ^|                                                                                      ^|
@echo ^|  Current dir : %CurrentDir% ^|
@echo ^|                                                                                      ^|
@echo ^|  GCC root dir: "%GCC_ROOT%"                                                   ^|
@echo ^|  Build dir : "%CD%"         ^|
@echo ^|                                                                                      ^|
@echo ^|  WxWidgets dir: "%WXWIN%"              ^|
@echo ^|                                                                                      ^|
@echo ^+======================================================================================^+
@echo.
@echo.

@rem ----------------------------------------------------------------
@rem Ask the user if they want to run the Windows_Ouput_Create.bat after the build
@rem ----------------------------------------------------------------
:QuestionsStart
if exist ".objs31_%BUILD_BITS%"   goto QuestionCleanupStart
if exist "devel31_%BUILD_BITS%"   goto QuestionCleanupStart
if exist "output31_%BUILD_BITS%"  goto QuestionCleanupStart
goto QuestionCleanupFinish

:QuestionCleanupStart
set /p CleanUp=Do you want to delete the previous build directories [Y/N]?
if /I "%CleanUp%" NEQ "Y" @echo Leaving previous build directories as they are.
if /I "%CleanUp%" NEQ "Y" goto QuestionCleanupFinish
@echo Removing previous build directories.
if exist ".objs31_%BUILD_BITS%"   del /q ".objs31_%BUILD_BITS%" > nul
if exist ".objs31_%BUILD_BITS%"   rmdir /S /q ".objs31_%BUILD_BITS%" > nul
if exist "devel31_%BUILD_BITS%"   del  /q "devel31_%BUILD_BITS%" > nul
if exist "devel31_%BUILD_BITS%"   rmdir /S /q "devel31_%BUILD_BITS%" > nul
if exist "output31_%BUILD_BITS%"  rmdir /S /q "output31_%BUILD_BITS%" > nul
if exist "output31_%BUILD_BITS%"  del /q "output31_%BUILD_BITS%" > nul
:QuestionCleanupFinish

:QuestionUpdateStart
@echo.
set /p UserInput=Do you want to run the Windows_Ouput_Create.bat after the compilation finishes [Y/N]?
if /I "%UserInput%" EQU "Y" @echo You have chosen to run the Windows_Ouput_Create.bat after the compilation finishes.
if /I "%UserInput%" NEQ "Y" @echo You have chosen NOT to run the Windows_Ouput_Create.bat.
@echo.
@echo.

:QuestionsFinished

@rem ----------------------------------------------------------------------------
@rem Check variables are setup otherwise display error message
@rem ----------------------------------------------------------------------------
if not exist "%CB_ROOT%"  goto ErrNoCB
if not exist "%GCC_ROOT%" goto ErrNoGCC
set PATH=%CB_ROOT%;%GCC_ROOT%;%GCC_ROOT%\bin;%PATH%

@rem ----------------------------------------------------------------------------
@rem Build command shell and wait until finished before continuing
@rem ----------------------------------------------------------------------------

:BuildStart

@rem Check and set build type
set BUILD_TYPE=--build
if "%1"=="r"        set BUILD_TYPE=--rebuild
if "%1"=="-r"       set BUILD_TYPE=--rebuild
if "%1"=="rebuild"  set BUILD_TYPE=--rebuild
if "%1"=="-rebuild" set BUILD_TYPE=--rebuild

set CB_EXE="%CB_ROOT%\codeblocks.exe"

set CB_PARAMS=--no-dde --multiple-instance --verbose --no-splash-screen --debug-log 
@rem set CB_PARAMS=%CB_PARAMS% --debug-log --log-to-file=Codeblocks_app.log --debug-log-to-file=Codeblocks_debug.log
@rem set CB_PARAMS=%CB_PARAMS% --log-to-file --debug-log-to-file
set CB_PARAMS=%CB_PARAMS% --app-log-filename=Codeblocks_app_%BUILD_BITS%.log --debug-log-filename=Codeblocks_debug_%BUILD_BITS%.log
set CB_PARAMS=%CB_PARAMS% --variable-set=cb_win%BUILD_BITS% --masterpath-set=%GCC_ROOT%
@rem debugging:
@rem set CB_PARAMS=%CB_PARAMS% --no-batch-window-close
@rem FUTURE: set CB_PARAMS=%CB_PARAMS%  --batch-headless-build
if  not exist "CodeBlocks_Windows.workspace" goto ErrProjectFile
set CB_TARGET=--target=All %BUILD_TYPE% "CodeBlocks_Windows.workspace"

@echo Building Code::Blocks. Please wait for the Code::Blocks compilation to finish.
set CB_RUN_COMMAND_LINE=%CB_EXE% %CB_PARAMS% %CB_TARGET%
@echo .
@echo RUNNING: ^"%CB_RUN_COMMAND_LINE%^"
@set start_date=%date% %time%

%CB_RUN_COMMAND_LINE%
IF %ERRORLEVEL% NEQ 0 (
    set RETURN_ERROR_LEVEL=%ERRORLEVEL%
)
set end_date=%date% %time%
@echo.
@echo  - - - - - - - - - - - - - - - - - - - - - - - - -
powershell -command "&{$start_date1 = [datetime]::parse('%start_date%'); $end_date1 = [datetime]::parse('%date% %time%'); echo (-join('Duration in seconds: ', ($end_date1 - $start_date1).TotalSeconds)); }"
@echo  - - - - - - - - - - - - - - - - - - - - - - - - -
@echo.
@echo.
@rem IF NOT "%RETURN_ERROR_LEVEL%" == "0" goto CompileError

:BuildFinished
if not exist "devel31_%BUILD_BITS%\codeblocks.exe"                          goto CompileError
if not exist "devel31_%BUILD_BITS%\codeblocks.exe"                          goto CompileError
if not exist "devel31_%BUILD_BITS%\Addr2LineUI.exe"                         goto CompileError
if not exist "devel31_%BUILD_BITS%\share\codeblocks\plugins\ToolsPlus.dll"  goto CompileError

@rem ----------------------------------------------------------------------------
@rem Copy the compiler DLL and wxWidget DLL's into the devel31_%BUILD_BITS% directory
@rem ----------------------------------------------------------------------------
:CopyFilesStart
@echo Copying compiler and wxWidget DLL's into the devel31_%BUILD_BITS% directory.
if "%BUILD_BITS%" == "32" if exist "%GCC_ROOT%\bin\libgcc_s_dw2-1.dll"        copy "%GCC_ROOT%\bin\libgcc_s_dw2-1.dll"        devel31_%BUILD_BITS% > nul
if "%BUILD_BITS%" == "64" if exist "%GCC_ROOT%\bin\libgcc_s_seh-1.dll"        copy "%GCC_ROOT%\bin\libgcc_s_seh-1.dll"        devel31_%BUILD_BITS% > nul
@rem if exist "%GCC_ROOT%\bin\libgcc_s_seh_64-1.dll"     copy "%GCC_ROOT%\bin\libgcc_s_seh_64-1.dll"     devel31_%BUILD_BITS% > nul
if exist "%GCC_ROOT%\bin\libwinpthread-1.dll"       copy "%GCC_ROOT%\bin\libwinpthread-1.dll"       devel31_%BUILD_BITS% > nul
if exist "%GCC_ROOT%\bin\libstdc++-6.dll"           copy "%GCC_ROOT%\bin\libstdc++-6.dll"           devel31_%BUILD_BITS% > nul

if exist "%WXWIN%\lib\gcc_dll\wxmsw%WXWIDGET_DLL_FILEVERSION%_gcc_cb.dll"    copy "%WXWIN%\lib\gcc_dll\wxmsw%WXWIDGET_DLL_FILEVERSION%_gcc_cb.dll"    devel31_%BUILD_BITS% > nul
if exist "%WXWIN%\lib\gcc_dll\wxmsw%WXWIDGET_DLL_FILEVERSION%_gl_gcc_cb.dll" copy "%WXWIN%\lib\gcc_dll\wxmsw%WXWIDGET_DLL_FILEVERSION%_gl_gcc_cb.dll" devel31_%BUILD_BITS% > nul

:CopyFilesFinish

@rem -------------------------------------------------------------------------------
@rem Run the Windows_Ouput_Create.bat if the user wanted it to run and we did not spawn the build 
@rem -------------------------------------------------------------------------------
:UpdateStart
if "%SpawnBuild%" == "True" goto UpdateFinish
if /I "%UserInput%" NEQ "Y" goto UpdateFinish
@echo.
@echo Running "call Windows_Ouput_Create.bat %BUILD_BITS% %GCC_ROOT%"
call Windows_Ouput_Create.bat %BUILD_BITS% %GCC_ROOT% > nul
@echo.
@echo.
:UpdateFinish
goto Finish

:ErrNoCBExe
@echo.
@echo.
@echo ^+-----------------------------------------------^+
@echo ^|     Error: Could not find C::B executables.   ^|
@echo ^+-----------------------------------------------^+
@echo. 
@echo.
@set RETURN_ERROR_LEVEL=1
goto Finish

:ErrProjectFile
@echo.
@echo.
@echo ^+-----------------------------------------------^+
@echo ^|     Error: Could not find the project file.   ^|
@echo ^+-----------------------------------------------^+
@echo. 
@echo.
@set RETURN_ERROR_LEVEL=2
goto Finish


:ErrNowxWidget
@echo.
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|     Error: NO "wxWidgets-%WXWIDGET_VERSION%" sub directory found  ^|
@echo ^+------------------------------------------------------^+
@echo. 
@echo.
@set RETURN_ERROR_LEVEL=3
goto Finish

:ErrNoCB
@echo.
@echo.
@echo ^+----------------------------------------------------^+
@echo ^| Error: C::B root folder not found.                 ^|
@echo ^|        Please fix the batch file and try again.    ^|
@echo ^+----------------------------------------------------^+
@echo. 
@echo.
@set RETURN_ERROR_LEVEL=4
goto Finish

:ErrNoGCC
@echo.
@echo.
@echo ^+----------------------------------------------------^+
@echo ^| Error: GCC root folder not found.                  ^|
@echo ^|        Please fix the batch file and try again.    ^|
@echo ^+----------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=5
goto Finish

:CompileError
@echo.
@echo.
@echo ^+----------------------------------------------------^+
@echo ^| Error: Code::Blocks compile error was detected.    ^|
@echo ^|        Please fix the error and try again.         ^|
@echo ^+----------------------------------------------------^+
@echo.
@echo.
@set RETURN_ERROR_LEVEL=6
goto Finish

:BuildBitError
@echo.
@echo ^+------------------------------------------------------^+
@echo ^| Error: NO Windows '32' or '64' parameter specified.  ^|
@echo ^|        Please run again with a parameter             ^|
@echo ^+------------------------------------------------------^+
@echo.
@echo.
@goto Finish

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

:Finish
cd /d %CurrentDir%
@endlocal
@rem Exit RETURN_ERROR_LEVEL