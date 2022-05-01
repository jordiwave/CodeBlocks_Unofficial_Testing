@echo off

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal
SET CurrentDir=%CD%

if exist *.orig (
  del *.orig
  del *.cpp
  del *.h
  "C:\Program Files\7-Zip\7z.exe" x -y *.7z > nul
)

if exist CodeBlocks_Unofficial_Testing (
copy Astylerc.ini CodeBlocks_Unofficial_Testing
cd CodeBlocks_Unofficial_Testing
)
cls
echo.
echo.
echo Astyle current directory is: %CD%
echo.
set /P c=Are you sure you want to continue[Y/N]?
if /I "%c%" EQU "Y" goto :Start_Astyle
goto :Finish

:Start_Astyle
"%CurrentDir%\AStyle.exe" --project=Astylerc.ini *.cpp *.h *.hxx
del /s *.orig

:Finish
cd /d "%CurrentDir%"
if exist CodeBlocks_Unofficial_Testing\Astylerc.ini del CodeBlocks_Unofficial_Testing\Astylerc.ini
@endlocal
pause