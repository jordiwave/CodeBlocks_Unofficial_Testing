@echo off

@echo Cleaning C::B directory build files.

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"
del /Q ZZ*.txt

if exist ..\..\..\bootstrap cd ..\..\..
if exist ..\..\bootstrap cd ..\..
if exist ..\bootstrap cd ..
if not exist bootstrap (
ECHO Cannot find bootstrap. Exiting
exit 1
)
echo C::B Root Directory is: %CD%

for /f "usebackq delims=^=^" %%a in (`"dir "MakeFile.am" /b/s" 2^>nul`) do C:\msys64\usr\bin\dos2unix.exe -q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.sh"        /b/s" 2^>nul`) do C:\msys64\usr\bin\dos2unix.exe -q "%%a"
C:\msys64\usr\bin\dos2unix.exe -q M4\*
C:\msys64\usr\bin\dos2unix.exe -q debian\*

C:\msys64\usr\bin\dos2unix.exe -q *.in
C:\msys64\usr\bin\dos2unix.exe -q bootstrap
C:\msys64\usr\bin\dos2unix.exe -q configure.ac

:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal