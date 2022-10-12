@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal
SET CurrentDir=%CD%

if "%OLD_path%" == "" (
    set OLD_path=%path%
    set path=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%path%
)

cls
echo.
echo.
echo clang-format current directory is: %CD%
echo.
set /P c=Are you sure you want to continue[Y/N]?
if /I "%c%" EQU "Y" goto :Start_Clang_Format
goto :Finish

:Start_Clang_Format
 for /f "usebackq delims=^=^" %%a in (`"dir *.cpp *.h /b/s" 2^>nul`) do clang-format -i "%%a"


goto Finish
for /f "usebackq delims=^=^" %%a in (`"dir D:\Temp\CodeBLocks_Private_Experimental_GCC\src\base\tinyxml\*.cpp D:\Temp\CodeBLocks_Private_Experimental_GCC\src\base\tinyxml\*.h /b/s" 2^>nul`) do (
    echo clang-format -i "%%a"
    clang-format -i "%%a"
)

:Finish
cd /d "%CurrentDir%"
@endlocal
pause