@echo off

@echo Cleaning C::B directory build files.

@REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

@SET CurrentDir="%CD%"
if exist ..\..\..\bootstrap cd ..\..\..
if exist ..\..\bootstrap cd ..\..
if exist ..\bootstrap cd ..
if not exist bootstrap (
ECHO Cannot find bootstrap. Exiting
exit 1
)
echo C::B Root Directory is: %CD%

@rem Remove directories
for /f "usebackq delims=^=^" %%a in (`"dir *.libs           /ad/b/s" 2^>nul`) do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.deps           /ad/b/s" 2^>nul`) do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.dirstamp       /ad/b/s" 2^>nul`) do rmdir /Q /S "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir *.cache          /ad/b/s" 2^>nul`) do rmdir /Q /S "%%a"

if exist ".\autom4te.cache"          rmdir /Q ".\autom4te.cache"
if exist ".\CodeBlocks.app"          rmdir /Q ".\CodeBlocks.app"

@rem Remove specific files 
for /f "usebackq delims=^=^" %%a in (`"dir "*.Plo"                  /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.bmarks"               /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.depend"               /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.layout"               /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.lo"                   /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.la"                   /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.pc"                   /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.cbplugin"             /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir ".dirstamp"              /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.gch"                  /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "compile_commands.json"  /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "*.log"                  /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir ".last_revision"         /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "z_*_result.txt"         /b  " 2^>nul`) do del /Q "%%a"

if exist "bundle.sh"                del /Q "bundle.sh"
if exist "aclocal.m4"               del /Q "aclocal.m4"
if exist "codeblocks.pc"            del /Q "codeblocks.pc"
if exist "codeblocks.plis"          del /Q "codeblocks.plis"
if exist "codeblocks.spec"          del /Q "codeblocks.spec"
if exist "codeblocks.spec.fedora"   del /Q "codeblocks.spec.fedora"
if exist "compile"                  del /Q "compile"
if exist "config.guess"             del /Q "config.guess"
if exist "config.log"               del /Q "config.log"
if exist "config.status"            del /Q "config.status"
if exist "config.sub"               del /Q "config.sub"
if exist "configure"                del /Q "configure"
if exist "configure.in"             del /Q "configure.in"
if exist "configure~"               del /Q "configure~"
if exist "debian\control"           del /Q "debian\control"
if exist "depcomp"                  del /Q "depcomp"
if exist "install-sh"               del /Q "install-sh"
if exist "libtool"                  del /Q "libtool"
if exist "ltmain.sh"                del /Q "ltmain.sh"
if exist "m4\libtool.m4"            del /Q "m4\libtool.m4"
if exist "m4\ltoptions.m4"          del /Q "m4\ltoptions.m4"
if exist "m4\ltsugar.m4"            del /Q "m4\ltsugar.m4"
if exist "m4\ltversion.m4"          del /Q "m4\ltversion.m4"
if exist "m4\lt~obsolete.m4"        del /Q "m4\lt~obsolete.m4"
if exist "missing"                  del /Q "missing"
if exist "revision.m4"              del /Q "revision.m4"
if exist "wxwin.m4"                 del /Q "wxwin.m4"
if exist "zmake.m4"                 del /Q "zmake.m4"
if exist ".\src\include\config.h"    del /Q ".\src\include\config.h"
if exist ".\src\include\config.h.in" del /Q ".\src\include\config.h.in"
if exist ".\src\include\stamp-h1"    del /Q ".\src\include\stamp-h1"
if exist ".\src\src\codeblocks"      del /Q ".\src\src\codeblocks"


for /f "usebackq delims=^=^" %%a in (`"dir "auto_revision.exe"      /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "cb_console_runner.exe"  /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "cb_share_config.exe"    /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "codeblocks.exe"         /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "Makefile"               /b/s" 2^>nul`) do del /Q "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir "Makefile.in"            /b/s" 2^>nul`) do del /Q "%%a"

for /f "usebackq delims=^=^" %%a in (`"dir *.o /b/s" 2^>nul`)                      do del /Q "%%a"


@rem specific files excluding \src\plugins\contrib\SpellChecker\hunspell\po\
@for /f "usebackq delims=^=^" %%A in (`"dir *.Po /a-d/b/s" 2^>nul`) do @(
    if NOT %%~dpA == %CD%\src\plugins\contrib\SpellChecker\hunspell\po\ (
        del /Q %%A 
    )
) 

@rem output directories
for /f "usebackq delims=^=^" %%a in (`"dir .objs3*  /ad/b/s" 2^>nul`)    do rmdir /Q /S  "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir devel3*  /ad/b/s" 2^>nul`)    do rmdir /Q /S  "%%a"
for /f "usebackq delims=^=^" %%a in (`"dir output3* /ad/b/s" 2^>nul`)    do rmdir /Q /S  "%%a"
 
:Finish
@echo Done
@cd /d %CurrentDir%
@endlocal