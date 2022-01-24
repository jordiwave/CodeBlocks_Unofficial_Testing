# Unofficial GIT Code::Blocks repo that contains the following:

## C::B exe/dll changed/update highlights compared to the SF C:B SVN 12675 source

    1. Incorporated SVN changes up to and including SVN 12675. See https://sourceforge.net/p/codeblocks/code/12675/log/ for change info.

    2. Does not include the CBFortan Plugin or other plugins that are SVN externals.

    3. Has auto-detect/auto add GDB support when options*.xml include GDB info. (SF Ticket 1114)

    4. Auto-detect POP up condition on startup has changed. After initial install will only pop up if the default compiler has issues. (SF Ticket 1117)

    5. Auto-detect compiler dialog has the option to show the installed or all the compilers. (SF Ticket 1117).

    6. The compiler selection dialog has the option of showing the detected compilers or all the compilers. Fix a number of compiler selection dialog bugs. (SF Ticket 1111)

    7. Moved Help->About->Information to Help->"System Information" and added allot of extra info on the CB and PC setup. Added copy to clipboard button that anonymizes the data when copied to the clipboard. (SF Ticket 748)
    
    8. Add ability to copy debug watch variable content to clipboard. Add three extra watches dialog context menu options: copy data, copy symbol and data and copy the symbol tree. (SF Ticket 654)
    
    9. Added Pecan's experimental CB-clangd_client. See https://sourceforge.net/projects/cb-clangd-client

    
## Updated installer highlights compared to the C::B 20.03 NSIS installer:

    1. Option to download and run the following compiler installers from the internet:
		- MinGW-W64
		- TDM
		- MSSY2
		- Cygwin

    2. Optionally remove user application data if found on an unnstall.

    3. Option to un-install if already found when installing.

    4. Optionally checks for fortan plugin and if found adds it to the installer.

    5. Output filename includes date and 32 or 64 bit and version.

    6. Added missing files that are included in the nightly build.

    7. Inno setup only does a full install.

    8. Updated start menu links.

    9. Ability to specify the start menu directory.

    10. Ability to specify the install directory.

    11. On an un-install allot more registry entries are removed (NSIS still leaves a few). Not all entries are removed.
    
    12. To many other changes to mention.... 
    
    13. Check out the https://sourceforge.net/projects/unofficial-cb-installers/files/Installer_Pages.odt file for the installer pages.
    
    14. Now installs Code::Blocks in the "C:\Program Files" directory for the 64 bit installer.
    
    15. Inno installer uses InnoSetup Compiler(ISS) (http://www.jrsoftware.org) aka ISS V6.2.0
    
    16. NSIS installer uses NSIS compiler (http://nsis.sourceforge.net) V3.0.8
    
    17. Inno installer script file is "Installer_ISS_full_only.iss"
    
    18. NSIS installer script file is "Installer_NSIS.nsi"
    
    19. NSIS only - Added XP SP 3 x86 and x64 support - different set of files from https://sourceforge.net/p/codeblocks/code/11196/tree/trunk/src/exchndl /win32/bin and /win64/bin.. NOTE: Inno setup 6 does not support XP anymore. x66 works, but x64 has issue with mscrt.dll function missing.
    
    20. Both Installer can install using non admin (user) account. Admin is needed to install in the C:\Program file.... directory. If you run normally you install as a user, but if you run as admin you are asked which type of installation  you want.
    
    21. Both Installer can optionally open the https://github.com/ssbssa/gdb/releases web page as a sub checkbox in the compiler install page under the MINGW checkbox. This qallows the user to manually install a later GDB for MINGW that works. (SF ticket 1020)
    
    22. Installer code is not a separate repo, but a directory in the C::B GitHub source repo tree. (SF Ticket 1119)
    
    23. Massive NSIS changes (Including updating parts to the new NSIS 3 way of working) (SF Ticket 1119)
    
    24. Inno Setup installer created and tested (SF Ticket 1119)
    
    25. Graphics changes included in the installers (I am not a graphical arteist if you get it!) (SF Ticket 1119)
    
    26. Installer script files support both 32 and 64 bit in the one script file. (SF Ticket 1119)
    
    27. Can build the installer(s) from a batch file now. (SF Ticket 1119)
    
    28. On Windows initial install auto detect GDB installation and configure it for use. (SF Ticket 1114)
    
    29. Asks if you want to uninstall previopus version before installing new version.
    
    30. Install and uninstall checks to see you are running as admin or normal user so the install/uninstall uses the same priveleges as last one.
    
    31. Modified NSI installer script to only show the download compiler page if an internet 
    connection is detected.
    
    32. Modified NSI installer script to support installing the following if they are in the same directory as the installer:
		- i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z
		- x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z

        These can be downloaded from https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/


