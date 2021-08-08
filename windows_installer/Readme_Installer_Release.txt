Updated for the release on 08-Aug-2021.

The installers references in this post are *not* official Code::Blocks releases, nor are they thoroughly tested like the official builds done by the Code::Blocks Team.
The installers do, however include changes that make the Code::Blocks installation and setup/configuration on Windows allot easier compared to the official installer.

If you want to check these installers out then I would advise backing up your Code::Blocks install and user data directories before running the installer so you can go back to your current configuration. 
You can also rename Code::Blocks install and user data directories instead of backing them up if you want to be 

Installer download link:
========================
https://sourceforge.net/projects/unofficial-cb-installers/

CB Sourceforge Tickets 
======================
The release includes code to hopefully resolve or implement the following CB sourceforge tickets:
    1. 909 - Fix Cygwin compiler support
    2. 374 - Additional Windows compiler support via XML files 
                - GCC MSYS2 - MinGW-w32
                - GCC MSYS2 - MinGW-W64
                - GCC TDM-32
                - GCC TDM-64
                - GCC MinGW-w32
                - GCC MinGW-w64
                - GCC LLVM Clang MinGW-W64
                - Other MinGW w32 or w64 compilers like winlib or gytx
    3. 1117 - Auto-detect compiler dialog startup changes
    4. 1111 - Compiler dialog changes for detected compilers and bug fixes
    5. 1119 - Windows installer updates
    6. 1114 - Initial windows install GDB auto detect and configure
    7. 04-Aug-21 Ticket 808 - Updated the default compiler background color to red if invalid. With 1117 this should resolve 808.
    8. 05/06-Aug-21 Ticket 748 - Moved Help->About->Information to Help->"System Information" and added allot of extra info on the CB and PC setup for helping with issues and bugs.
    9. 07-AUG-21 Ticket 654 - Copy Debug Watch variable content to clipboard. Add three extra watches dialog context menu options: copy data, copy symbol and data and copy the symbol tree.
    10. 08-Aug-21 Ticket 1122 - pull upstream changes.  Fix compilation with wx-master
    11. 08-Aug-21 Ticket 1113 - pull upstream changes.  Linux: xfce4 and gnome terminal can not be terminated 

Installer changed highlights:
=============================
    1. Option to download and run the following compiler installers from the internet:
        MinGW-W64
        TDM
        MSSY2
        Cygwin
    2. Optionally remove user application data if found on an unnstall.
    3. Option to un-install if already found when installing
    4. Optionally checks for fortan plugin and if found adds it to the installer
    5. Output filename includes date and 32 or 64 bit and version.
    6. Added missing files that are included in the nightly build
    7. Inno setup only does a full install.
    8. Updated start menu links
    9. Ability to specify the start menu directory
    10. Ability to specify the install directory
    11. On an un-install allot more registry entries are removed (NSIS still leaves a few). Not all entries are removed.
    12. To many other changes to mention.... 
    13. Check out the https://sourceforge.net/projects/unofficial-cb-installers/files/Installer_Pages.odt file for the installer pages.
    14. Now installs Code::Blocks in the "C:\Program Files" directory for the 64 bit installer.
    15. Inno installer uses InnoSetup Compiler(ISS) (http://www.jrsoftware.org) aka ISS V6.2.0
    16. NSIS installer uses NSIS compiler (http://nsis.sourceforge.net) V3.7.0
    17. Inno installer script file is "Installer_ISS_full_only.iss"
    18. NSIS installer script file is "Installer_NSIS.nsi"
    19. 06-AUG-21 NSIS only - Added XP SP 3 x86 and x64 support - different set of files from https://sourceforge.net/p/codeblocks/code/11196/tree/trunk/src/exchndl /win32/bin and /win64/bin.. NOTE: Inno setup 6 does not support XP anymore. x66 works, but x64 has issue with mscrt.dll function missing.
    20. 08-Aug-21 Inno ans NSIS Installer can install using non admin (user) account. Admin is needed to install in the C:\Program file.... directory. If you run normally you install as a user, but if you run as admin you are asked which type of intallation you want.

C::B exe/dll changed/update highlights:
=======================================
    1. Auto-detect POP up condition on startup has changed. After initial install will only pop up if the default compiler has issues.
    2. Auto-detect compiler dialog has the option to show the installed or all the compilers
    3. The compiler selection dialog has the option of showing the detected or all the compilers.
    3. The compiler selection dialog has a number of bug fixes 
    4. Added support for the following compilers via the XML files:
        mingw32
        mingw64
        msys2-mingw32
        msys2-mingw64
        tdm-32
        tdm-64
    5. Cygwin compiler now detected correctly
    6. 64bit built with MSYS2 mingw64 GCC 10.3 using the latest MSYS 2 release (okay I update it at least once a week).
    7. 32bit built with MingGW-64 installer using "GCC 8.1.0 , i686, posix threading, dwarf exception hading and 0 build revision" configuration.
    8. Installer code is not a separate repo, but a directory in the C::B GitHub source repo tree.
    9. Massive NSIS changes (Including updating parts to the new NSIS 3 way of working)
    10. Inno Setup installer created and tested
    11. Graphics changes included in the installers (I am not a graphical arteist if you get it!)
    12. Installer script files support both 32 and 64 bit in the one script file.
    13. Can build the installer(s) from a batch file now
    14. On Windows initial install auto detect GDB installation and configure it for use.
    15. 05-AUG-21 Moved Help->About->Information to Help->"System Information" and added allot of extra info on the CB and PC setup. Added copy to clipboard button that anonymize's the data when copied to the clipboard
    16. 05-AUG-21 Added extra help plugin menu items for the updated documentation included in the installation. These appear as sub menu items in the Help menu.
    17. 05-AUG-21 In Help->"System Information" copy to clipboard button now anonymize's the data when copied to the clipboard and shows the spinning cursor while the data is being worked on.
    18. 06-AUG-21 Added missing INNO installer shortcuts that were in the NSIS installer.
    19. 07-AUG-21 Ticket 654 - Copy Debug Watch variable content to clipboard. Add three extra watches dialog context menu options: copy data, copy symbol and data and copy the symbol tree.
    20. 08-Aug-21 Ticket 1122 - pull upstream changes, so incorporated this change to fix compilation with wx-master
    21. 08-Aug-21 Ticket 1113 - pull upstream changes, so incorporated this change to fix Linux: xfce4 and gnome terminal can not be terminated 
    22. 08-Aug-21 no ticket ref# - pull upstream changes, so incorporated this change for scripting change to Print more information when someone calls a native function from squirrel incorrectly.
    23. 08-Aug-21 Updated the Installer_Pages.odt for installer user install changes.

SOURCE:
=======
The source code used to produce the installers is currently (until the repo is no longer required or supported):
    https://github.com/acotty/codeblocks_sf/tree/AC-WindowsInstaller

NOTES:
======
1. When editing the ISS install file you can use the "Inno Script Studio" V2.5.1 or later from https://www.kymoto.org/products/inno-script-studio 
to make the editing easier and it also allows you to compiler, debug and test the script within the application.
2. 08-Aug-21 Bumped revision from 12487 to 12492 due to pulling code from https://github.com/obfuscated/codeblocks_sf upstream.


Disclaimer: 
===========
This installer contains binaries that include code changes that are *NOT* included in the Code::Blocks official SVN repository. These installers and the code changes are *not* endorsed by the Code::Blocks Team themselves, so any incorrect behavior you may find therein should be tested for in the official release and nightly versions before you decide to report any bugs.


Outstanding Issues to be investigated:
======================================
1. 06-AUG-21 Ticket 1020 - GDB issue - later MINGW32 release - investigate replacement GDB.exe or modify installer to show  extra details to the end user.
    FAILS due to MSYS2 dependencies: https://packages.msys2.org/package/mingw-w64-i686-gdb => https://mirror.msys2.org/mingw/mingw32/mingw-w64-i686-gdb-10.2-2-any.pkg.tar.zst
    RUNS: https://github.com/ssbssa/gdb/releases  ==> https://github.com/ssbssa/gdb/releases/download/gdb-10.2.90.20210425/gdb-10.2.90.20210425-i686.7z the python version has issues with the default mingw32 install
    NOTE: 
2. 08-Aug-2021 Investigate issue where C::B project compiler is not available, but no messages pop up or are displayed to indicate an issue.
3. 08-AUG-21 Topic: [Proposal] Move the custom variables WX_SUFFIX... to global variables. https://forums.codeblocks.org/index.php/topic,21696.msg147584.html#msg147584  / https://github.com/bluehazzard/codeblocks_sf/tree/rework/move_global_var_diag_to_src/1
4. Other installer issues....  Please post ticket reference on the https://forums.codeblocks.org/index.php/topic,24592.0.html thread.
5. Other Windows CB installation or configuration issues with the initial installation... Please post ticket reference on the https://forums.codeblocks.org/index.php/topic,24592.0.html thread.


