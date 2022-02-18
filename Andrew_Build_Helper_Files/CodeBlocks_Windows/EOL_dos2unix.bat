@echo off

@echo Convert Source EOL from DOS to Unix for consistency

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"
cd ..\..

set D2U=c:\msys64\usr\bin\dos2unix.exe

@rem specific files 
for /f "usebackq delims=^=^" %%a in (`"dir *.h   /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.cpp /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.cbp /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.wxs /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.xrc /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.m4 /b/s" 2^>nul`) do %D2U% -q %%a
for /f "usebackq delims=^=^" %%a in (`"dir Mawefile.am /b/s" 2^>nul`) do %D2U% -q %%a


:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal