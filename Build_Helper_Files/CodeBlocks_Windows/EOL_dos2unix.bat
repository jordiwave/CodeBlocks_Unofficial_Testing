@echo off

@echo Convert Source EOL from DOS to Unix for consistency

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir=%CD%

if exist bootstrap goto start
if exist ..\..\bootstrap cd ..\..
if exist ..\..\..\bootstrap cd ..\..\..

:start
"%CurrentDir%\CheckFilesEOL.exe "

:Finish
@echo Done
@cd /d "%CurrentDir%"
@rem pause
@endlocal