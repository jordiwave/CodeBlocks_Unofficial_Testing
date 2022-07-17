@rem load the  NIGHTLY_BUILD_SVN variable from the txt file
for /f "delims== tokens=1,2" %%G in (Build_Version_Number.txt) do set %%G=%%H

if not "%WX_DIR_VERSION%" == "" goto WX_DIR_VERSION_Okay
if exist "src\devel31*" set WX_DIR_VERSION=31
if exist "src\devel32*" set WX_DIR_VERSION=32
set WX_DIR_VERSION=32

:WX_DIR_VERSION_Okay

if "%GITHUB_ACTIONS%" == "true" (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=64" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_Simple.nsi"
) else (
    "C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=64" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "/DWXVERSION=%WX_DIR_VERSION%" "Installer_NSIS_UMUI.nsi"
)
