@echo off

@echo Cleaning C::B directory build files.

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"
cd ..\..

@rem Remove directories
for /f "usebackq delims=^=^" %%a in (`"dir *.libs /ad/b/s" 2^>nul`)         do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.deps /ad/b/s" 2^>nul`)         do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.dirstamp /ad/b/s" 2^>nul`)     do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.cache /ad/b/s" 2^>nul`)        do rmdir /Q /S  "%%a"

@rem Remove specific files 
for /f "usebackq delims=^=^" %%a in (`"dir "*.Plo" /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.bmarks" /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.depend" /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.layout" /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.lo" /b/s" 2^>nul`)            do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.la" /b/s" 2^>nul`)            do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.pc" /b/s" 2^>nul`)            do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.cbplugin" /b/s" 2^>nul`)      do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "Makefile" /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "Makefile.in" /b/s" 2^>nul`)     do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir ".dirstamp" /b/s" 2^>nul`)       do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "*.gch" /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "z_*_result.txt" /b/s" 2^>nul`)  do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir "compile_commands.json" /b/s" 2^>nul`)   do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.log /b/s" 2^>nul`)             do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir revision.m4 /b/s" 2^>nul`)       do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir missing /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir Makefile.in /b/s" 2^>nul`)       do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir ltmain.sh /b/s" 2^>nul`)         do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir libtool /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir install-sh /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir depcomp /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir configure~ /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir configure /b/s" 2^>nul`)         do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir config.sub /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir config.status /b/s" 2^>nul`)     do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir config.log /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir config.guess /b/s" 2^>nul`)      do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir compile /b/s" 2^>nul`)           do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir codeblocks.spec.fedora /b/s" 2^>nul`) do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir codeblocks.spec /b/s" 2^>nul`)   do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir aclocal.m4 /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir .last_revision /b/s" 2^>nul`)    do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir m4\lt~obsolete.m4 /b/s" 2^>nul`) do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir m4\ltversion.m4 /b/s" 2^>nul`)   do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir m4\ltsugar.m4 /b/s" 2^>nul`)     do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir m4\ltoptions.m4 /b/s" 2^>nul`)   do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir m4\libtool.m4 /b/s" 2^>nul`)     do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir auto_revision.exe /b/s" 2^>nul`)     do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir cb_console_runner.exe /b/s" 2^>nul`) do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir cb_share_config.exe /b/s" 2^>nul`)   do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir codeblocks.exe /b/s" 2^>nul`)        do del /Q %%a
for /f "usebackq delims=^=^" %%a in (`"dir *.o /b/s" 2^>nul`)                   do del /Q %%a

@rem specific files excluding \src\plugins\contrib\SpellChecker\hunspell\po\
@for /f "usebackq delims=^=^" %%A in (`"dir *.Po /a-d/b/s" 2^>nul`) do @(
    if NOT %%~dpA == %CD%\src\plugins\contrib\SpellChecker\hunspell\po\ (
        del /Q %%A 
    )
) 

for /f "usebackq delims=^=^" %%a in (`"dir .objs31_32 /ad/b/s" 2^>nul`)    do rmdir /Q /S  "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir .objs31_64 /ad/b/s" 2^>nul`)    do rmdir /Q /S  "%%a"

IF EXIST "src\build_tools\autorevision\auto_revision.exe"   del /Q  "src\build_tools\autorevision\auto_revision.exe"
  
@rem output directories
IF EXIST "src\devel31_32"  rmdir /Q /S src\devel31_32
IF EXIST "src\.objs31_32"  rmdir /Q /S src\.objs31_32
IF EXIST "src\output31_32" rmdir /Q /S src\output31_32
IF EXIST "src\devel31_64"  rmdir /Q /S src\devel31_64
IF EXIST "src\.objs31_64"  rmdir /Q /S src\.objs31_64
IF EXIST "src\output31_64" rmdir /Q /S src\output31_64

:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal