The installers references in this post are *not* official Code::Blocks releases, nor are they thoroughly tested like the official builds done by the Code::Blocks Team.
The installers do, however include changes that make the Code::Blocks installation and setup/configuration on Windows allot easier compared to the official installer.

If you want to check these installers out then I would advise backing up your Code::Blocks install and user data directories before running the installer so you can go back to your current configuration. 
You can also rename Code::Blocks install and user data directories instead of backing them up if you want to be 

Installer download link:
========================
https://sourceforge.net/projects/unofficial-cb-installers/


Installer changed highlights:
=============================
    1) Option to download and run the following compiler installers from the internet:
        MinGW-W64
        TDM
        MSSY2
        Cygwin
    2) Optionally remove user application data if found on an unnstall.
    3) Option to un-install if already found when installing
    4) Optionally checks for fortan plugin and if found adds it to the installer
    5) Output filename includes date and 32 or 64 bit and version.
    6) Added missing files that are included in the nightly build
    7) Inno setup only does a full install.
    9) Updated start menu links
    10) Ability to specify the start menu directory
    11) Ability to specify the install directory
    12) On an uninstall allot more registry entries are removed (NSIS still leaves a few). Not all entries are removed.
    13) To many other changes to mention.... 
    14) Check out the https://sourceforge.net/projects/unofficial-cb-installers/files/Installer_Pages.odt file for the installer pages.
    15) Now installs Code::Blocks in the "C:\Program Files" directory for the 64 bit installer.
    16) Inno installer uses InnoSetup Compiler(ISS) (http://www.jrsoftware.org) aka ISS V6.2.0
    17) NSIS installer uses NSIS compiler (http://nsis.sourceforge.net) V3.7.0
    18) Inno installer script file is Installer_NSIS_wx31_64bit.nsi
    18) NSIS installer script file is Installer_ISS_x64 - Full Only.iss


C::B exe/dll changed/update highlights:
=======================================
    1) Autodetect POP up condition on startup has changed. After initial install will only pop up if the default compiler has issues.
    2) Autodetect compiler dialog has the option to show the installed or all the compilers
    3) The compiler selection dialog has the option of showing the detected or all the compilers.
    3) The compiler selction dialog has a number of bug fixes 
    4) Added support for the following compilers via the XML files:
        mingw32
        mingw64
        msys2-mingw32
        msys2-mingw64
        tdm-32
        tdm-64
    5) Cygwin compiler now detected correctly
    6) Built with MSYS2 mingw64 GCC 10.3 using the latest MSYS 2 release (okay I update it at least once a week).
    7) Installer code is not a separate repo, but a directory in the C::B source tree.
    8) Massive NSIS changes (Including updating parts to the new NSIS 3 way of working)
    9) Inno Setup installer created and tested
    10) Graphics changes included in the installers (I am not a graphical arteist if you get it!)
    11) Installer script files support 32 and 64 bit in the one script now
    11) Can build the installer(s) from a batch file now

SOURCE:
=======
The source code used to produce the installers is currently (until the repo is no longer required or supported):
    https://github.com/acotty/codeblocks_sf/tree/AC-WindowsInstaller

NOTES:
======
1) 32 bit installers have not been built or tested, apart from testing that the scripts can be configured for 32 bit usage and do run.
2) When editing the ISS install file you can use the "Inno Script Studio" V2.5.1 or later from https://www.kymoto.org/products/inno-script-studio 
to make the editing easier and it also allows you to compiler, debug and test the script within the application.


Disclaimer: 
===========
This installer contains binaries that include code changes that are *NOT* included in the Code::Blocks official SVN repository. These installers and the code changes are *not* endorsed by the Code::Blocks Team themselves, so any incorrect behavior you may find therein should be tested for in the official release and nightly versions before you decide to report any bugs.
