@echo off

@echo Convert Source EOL from DOS to Unix for consistency

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"
if exist ..\..\bootstrap cd ..\..
if exist ..\..\..\bootstrap cd ..\..\..

set D2U=c:\msys64\usr\bin\dos2unix.exe

@rem specific files 
@for /f "usebackq delims=^=^" %%a in (`"dir *.h *.cpp *.cbp *.wxs *.wrc *.m4 *.ac Makefile.am *.sh *.in *.xml *.sample *.workspace /b/s" 2^>nul`) do @%D2U% -q %%a

:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal
pause