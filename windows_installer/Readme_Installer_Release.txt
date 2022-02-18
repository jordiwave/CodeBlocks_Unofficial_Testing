Updated for the release on 30-Jan-2022


C::B exe/dll changed/update highlights compared to the last nightly SVN 12675 release (23-Jan-2021):
====================================================================================================
    1. Incorporated SVN changes up to and including SVN 12686. See https://sourceforge.net/p/codeblocks/code/12686/log/ for change info.
    2. Has auto-detect/auto add GDB support when options*.xml include GDB info. (SF Ticket 1114)
    3. Auto-detect POP up condition on startup has changed. After initial install will only pop up if the default compiler has issues. (SF Ticket 1117)
    4. Auto-detect compiler dialog has the option to show the installed or all the compilers. (SF Ticket 1117).
    5. The compiler selection dialog has the option of showing the detected compilers or all the compilers. Fix a number of compiler selection dialog bugs. (SF Ticket 1111)
    6. Moved Help->About->Information to Help->"System Information" and added allot of extra info on the CB and PC setup. Added copy to clipboard button that anonymizes the data when copied to the clipboard. (SF Ticket 748)
    7. Add ability to copy debug watch variable content to clipboard. Add three extra watches dialog context menu options: copy data, copy symbol and data and copy the symbol tree. (SF Ticket 654)
    8. HUGE number of plugin changes as follows:
        * tidycmt (added to installer 24JAN2022)
        * headerguard (added to installer 24JAN2022)
        * loghacker (added to installer 24JAN2022)
        * ModPoller (added to installer 24JAN2022)
        * DisplayEvents (code and installer added 22JAN2022)
        * cb_koders (removed from build and installer 24JAN2022)
        * CBFortan Plugin from SVN C::B get (added code to git on 25JAN2022)
        * https://sourceforge.net/projects/cb-clangd-client (code and installer added 07JAN2022 & updated to V0.2.11 as of 27JAN2022 & local changes)
        * https://github.com/bluehazzard/cbMarkdown (code and installer added 24JAN2022)
        * https://github.com/bluehazzard/cbBuildTools (code and installer added 24JAN2022)
        * https://github.com/bluehazzard/cbMemoryView (code and installer added 24JAN2022)
        * https://github.com/bluehazzard/cbSystemView (code and installer added 24JAN2022)
        * https://github.com/danselmi/cbDiff  (code and installer added 25JAN2022)
        * https://github.com/loandr/GitBlocks (code and installer added 25JAN2022)
        * https://sourceforge.net/projects/fafnir/files/cbNSIS/windows/ (code and installer added 25JAN2022)
        * https://github.com/MortenMacFly/cbtortoisesvn (code and installer added 25JAN2022)
        * https://github.com/zinnjonas/cbInno (code and installer added 25JAN2022)
    9. Modified config code so the configs for this version are installed in the codeblocks_experimental directory in order to allow nightly 
          and this version to co-exist on the one pC.
    10. Added support for export, import and save global variables. (SF Ticket 1184)
    11. Added nww "--variable-set" command line parameter to set the global variable set for batch building win32 & Win64 from the one workspace/project file 
          by using different global variable sets with support for the variables in the project files used. [Trunk CB project files do not support this.]
        
Updated installer highlights compared to the C::B 20.03 NSIS installer:
=======================================================================
    1. Option to download and run the following compiler installers from the internet:
            MinGW-W64
            TDM
            MSSY2
            Cygwin
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
    16. NSIS installer uses NSIS compiler (http://nsis.sourceforge.net) V3.0.8 (3.0.8 from 7-Oct-2021)
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
    31. Modified NSI installer script to only show the download compiler page if an internet connection is detected.
    32. Modified NSI installer script to support installing the following if they are in the same directory as the installer:
            * i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z
            * x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z
          These can be downloaded from https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/
    33. Added the following plugins not included in the 20.03 NSIS:
        * tidycmt (added 24JAN2022)
        * headerguard (added 24JAN2022)
        * loghacker (added 24JAN2022)
        * ModPoller (added 24JAN2022)
        * clangd_client (added 07JAN2022)
        * DisplayEvents (added 22JAN2022)
        * https://github.com/bluehazzard/cbMarkdown (added 24JAN2022)
        * https://github.com/bluehazzard/cbBuildTools (added 24JAN2022)
        * https://github.com/bluehazzard/cbMemoryView (added 24JAN2022)
        * https://github.com/bluehazzard/cbSystemView (added 24JAN2022)
    34. Removed the following plugins:
        * cb_koders (removed 24JAN2022)
    35 Modified install directory and name from codeblocks to codeblocks_experimental in order to allow nightly and this version to co-exist on the one pC.

SOURCE/Compiler Info:
=====================
1. C::B x64 built with MSYS2 mingw64 GCC 11.2 using the latest MSYS 2 release at the time of the build.
2. C::B x86 built with MingGW-32 installer using "GCC 8.1.0 , i686, posix threading, dwarf exception hading and 0 build revision" configuration.

NOTES:
======
    If there are any other plugins you think should be included that are benefit to both PC & Linux devs please let me know. I will not consider plugins that are for only one OS or for embedded systems.

