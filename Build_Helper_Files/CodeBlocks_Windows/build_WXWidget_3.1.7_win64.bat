@echo off

@rem ------------------------------------------------------------------------------------------------------------
@rem GIT NOTES:
@rem
@rem @IF NOT EXIST wxWidget_github git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets wxWidget_github
@rem
@rem YOU NEED TO EDIT the following files to change the "#define wxUSE_GRAPHICS_DIRECT2D 0" from 0 to 1:
@rem        include\wx\msw\setup.h
@rem        include\wx\mswu\setup.h
@rem
@rem ------------------------------------------------------------------------------------------------------------

@rem ------------------------------------------------------------------------------------------------------------
@rem Hopefully these variables are the only changed when you need to configure for your Code::Blocks source build
@rem ------------------------------------------------------------------------------------------------------------

set WXWIDGET_VERSION=3.1.7
@rem set BUILD_BITS=32
set BUILD_BITS=64
set BUILD_TYPE=debug
@rem set BUILD_TYPE=release

set BUILD_FLAGS=SHARED=1 MONOLITHIC=1 BUILD=%BUILD_TYPE% UNICODE=1 VENDOR=cb CFLAGS="-m%BUILD_BITS%" CXXFLAGS="-std=gnu++17 -m%BUILD_BITS%" CPPFLAGS="-m%BUILD_BITS%" LDFLAGS="-m%BUILD_BITS%"

@rem https://forums.wxwidgets.org/viewtopic.php?t=42817
@rem mingw32-make     -f makefile.gcc BUILD=release CPP="gcc -E -D_M_IX86 -m32" CFLAGS="-m32" CXXFLAGS="-m32 -std=gnu++11" CPPFLAGS="-m32" LDFLAGS="-m32" WINDRES="windres --use-temp-file -F pe-i386" setup_h
@rem mingw32-make -j4 -f makefile.gcc BUILD=release CPP="gcc -E -D_M_IX86 -m32" CFLAGS="-m32" CXXFLAGS="-m32 -std=gnu++11" CPPFLAGS="-m32" LDFLAGS="-m32" WINDRES="windres --use-temp-file -F pe-i386"


@rem GCC_ROOT is the root directory of the compiler you are going to use to build C::B with
set GCC_ROOT=C:\msys64\mingw%BUILD_BITS%
@rem set GCC_ROOT=C:\TDM-GCC-%BUILD_BITS%
@rem set GCC_ROOT=C:\mingw%BUILD_BITS%
set SED_EXE=C:\msys64\usr\bin\sed.exe

@rem ==========================================================================================================
@rem Hopefully you will not have to modify anything below
@rem ==========================================================================================================

CALL :NORMALIZEPATH "..\..\..\..\Libraries\wxWidgets-%WXWIDGET_VERSION%_win%BUILD_BITS%"
SET WXWIN=%RETVAL%
if not exist %WXWIN% goto ErrNowxWidget

@rem ------------------------------------------------------------------------------------------------------------
@rem save for later
@rem ------------------------------------------------------------------------------------------------------------
SET CurrentDir="%CD%"

@rem ------------------------------------------------------------------------------------------------------------
@rem Backup path if not all ready backed up so it can be reverted at the end 
@rem ------------------------------------------------------------------------------------------------------------
set PATH_ORIGINAL=%PATH%

@rem ------------------------------------------------------------------------------------------------------------
@rem Update Path to include GCC root folder and "bin" subfolder
@rem ------------------------------------------------------------------------------------------------------------
:start
set PATH=%GCC_ROOT%;%GCC_ROOT%\bin;%PATH%

@cls
@echo.
@echo +==============================================================+
@echo ^| %GCC_ROOT%   WxWidgets %WXWIDGET_VERSION%   win%BUILD_BITS%  build script    ^|
@echo +==============================================================+
@echo ^|                                                              ^|
@echo ^| GCC_ROOT: %GCC_ROOT%                                  ^|
@echo ^|                                                              ^|
@echo ^| WXWIN: %WXWIN% ^|
@echo ^|                                                              ^|
@echo +==============================================================+
@echo.

cd /d %WXWIN%\build\MSW
@echo. > "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"
@echo ================================================== > "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"
@echo START BUILDING WXWIDGET > %\ZZ_WXWIDGET_BUILD.txt"
@set start_date=%date% %time%

@rem ==========================================================================================================
@rem Update the following files to change the "#define wxUSE_GRAPHICS_DIRECT2D 0" from 0 to 1:
@rem        include\wx\msw\setup.h
@rem        lib\gcc_dll\mswu\wx\setup.h

if exist "%WXWIN%\\include\\wx\\msw\\setup.h" (
    cd %WXWIN%\include\wx\msw
    echo "sed update File: %CD%\\setup.h #define wxUSE_GRAPHICS_DIRECT2D from 0 to 1"
    %SED_EXE% -i 's\\#define wxUSE_GRAPHICS_DIRECT2D 0\\#define wxUSE_GRAPHICS_DIRECT2D 1\\g' setup.h
)

if exist "%WXWIN%\\lib\\gcc_dll\\mswu\\wx\\setup.h" (
    cd %WXWIN%\lib\gcc_dll\mswu\wx
    echo "sed update File: %CD%\\setup.h #define wxUSE_GRAPHICS_DIRECT2D from 0 to 1"
    %SED_EXE% -i 's\\#define wxUSE_GRAPHICS_DIRECT2D 0\\#define wxUSE_GRAPHICS_DIRECT2D 1\\g' setup.h
)

@rem if exist "%WXWIN%\\build\\msw\\makefile.gcc" (
@rem     cd %WXWIN%\build\msw
@rem     echo "sed update File: %CD%\\makefile.gcc $(SETUPHDIR)\\wx\\setup.h: to $(SETUPHDIR)\\wx\\setup.h: $(SETUPHDIR)\\wx"
@rem     echo %SED_EXE% -i "s^\$\(SETUPHDIR\)\\wx\\setup.h:^\$\(SETUPHDIR\)\\wx\\setup.h:\ \$\(SETUPHDIR\)\\wx^g" makefile.gcc
@rem )

cd /d %WXWIN%\build\MSW

@rem ==========================================================================================================

:start-build
@echo.
@echo ^+----------------------------------------^+
@echo ^|      WxWidget win%BUILD_BITS% %BUILD_TYPE% clean       ^|
@echo ^+----------------------------------------^+
@echo mingw32-make -j -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe clean
@mingw32-make -j -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe clean  > %CurrentDir%\wxwidget_%BUILD_TYPE%_build.log
@IF %ERRORLEVEL% NEQ 0 goto FAIL
@echo.
@echo ^+----------------------------------------^+
@echo ^|   WxWidget win%BUILD_BITS% %BUILD_TYPE% build setup.h  ^|
@echo ^+----------------------------------------^+
@echo mingw32-make -j1 -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe setup_h
@mingw32-make -j1 -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe setup_h >> %CurrentDir%\wxwidget_%BUILD_TYPE%_build.log
@IF %ERRORLEVEL% NEQ 0 goto FAIL
@echo.
@echo ^+----------------------------------------^+
@echo ^|  WxWidget win%BUILD_BITS% %BUILD_TYPE% build library   ^|
@echo ^+----------------------------------------^+
@echo mingw32-make -j -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe
@mingw32-make -j -f makefile.gcc %BUILD_FLAGS% SHELL=cmd.exe >> %CurrentDir%\wxwidget_%BUILD_TYPE%_build.log
@IF %ERRORLEVEL% EQU 0 goto PASS
@echo.
@echo ** CURRENT Directory: %CD%
@echo.
goto FAIL

@rem ==========================================================================================================

:ErrNowxWidget
@echo on
echo Could not find %WXWIN%
@echo.
@echo.
@echo ^+------------------------------------------------------^+
@echo ^|     Error: NO "wxWidgets-%WXWIDGET_VERSION%" sub directory found  ^|
@echo ^+------------------------------------------------------^+
@echo. 
@echo.
goto FINISHED

:FAIL
@echo.
@echo ^+==================================^+
@echo ^|  Build failed, see above why.    ^|
@echo ^+==================================^+
@echo.
@echo.
goto FINISHED

@rem ==========================================================================================================

:PASS
@echo.
@echo ^+=================================^+
@echo ^|  Build sucesfully completed.    ^|
@echo ^|  Build sucesfully completed.    ^|
@echo ^|  Build sucesfully completed.    ^|
@echo ^+=================================^+
@echo.
@echo.
set end_date=%date% %time%
@echo  START: %start_date% >> "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"
@echo  END:   %end_date% >> "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"
@echo  ^+ - - - - - - - - - - - - - - - - - - - - - - - - - ^+
powershell -command "&{$start_date1 = [datetime]::parse('%start_date%'); $end_date1 = [datetime]::parse('%date% %time%'); echo (-join('| Duration in seconds: ', ($end_date1 - $start_date1).TotalSeconds, '                       |')); }"
@powershell -command "&{$start_date1 = [datetime]::parse('%start_date%'); $end_date1 = [datetime]::parse('%date% %time%'); echo (-join('Duration in seconds:         ', ($end_date1 - $start_date1).TotalSeconds)); }" >> "%CurrentDir%\ZZ_BUILD.txt"
@echo  ^+ - - - - - - - - - - - - - - - - - - - - - - - - - ^+
@echo ================================================== >> "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"
@echo.  >> "%CurrentDir%\ZZ_WXWIDGET_BUILD.txt"

goto FINISHED

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B

@rem ==========================================================================================================

:FINISHED
set PATH=%PATH_ORIGINAL%
set PATH_ORIGINAL=
set GCC_ROOT=
set WXWIN=
set BUILD_BITS=
set BUILD_TYPE=
set BUILD_FLAGS=

@rem ==========================================================================================================

:Exit
cd /d %CurrentDir%
set CurrentDir=
