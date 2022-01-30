@rem load the  NIGHTLY_BUILD_SVN variable from the txt file
for /f "delims== tokens=1,2" %%G in (Build_Version_Number.txt) do set %%G=%%H

"C:\Program Files (x86)\NSIS\makensis.exe" "/DBUILD_TYPE=32" "/DNIGHTLY_BUILD_SVN=%NIGHTLY_BUILD_SVN%" "Installer_NSIS.nsi"
pause