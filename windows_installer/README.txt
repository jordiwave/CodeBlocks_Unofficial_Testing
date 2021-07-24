

The setup source file in this directory and sub directories are for use with the following two installers:

* NSIS compiler (http://nsis.sourceforge.net) at least V3.6.1
* InnoSetup Compiler(ISS) (http://www.jrsoftware.org) aka ISS V6.2.0

When editing the ISS install file you can use the "Inno Script Studio" V2.5.1 or later from https://www.kymoto.org/products/inno-script-studio 
to make the editing easier and it also allows you to compiler, debug and test the script within the application.


The following are the NSIS installer files:
    * Installer_NSIS_wx31_32bit.nsi
    * Installer_NSIS_wx31_64bit.nsi

The following are the ISS installer files:
    * Installer_ISS_wx31_32bit.iss
    * Installer_ISS_wx31_64bit.iss


NSIS Status:
------------
1) X Option to to download the mingw64-installer.exe does not work.
2) X Does not remove user application data 
3) Option to un-install if already found when installing working.
4) Optionally checks for fortan plugin and if found adds it to the installer
5) Output filename includes date and 32 or 64 bit and version.
6) Added missing files that are included in the nightly build
7) Install components appears to be okay.
8) Easily swap between 32 and 64 bit build via one define change
9) Need to add the following link for new C++ users:
    http://www.sci.brooklyn.cuny.edu/~goetz/codeblocks/codeblocks-instructions.pdf


ISS Status:
-----------
1) Option to download the mingw64-installer.exe working.
2) Optionally remove user application data if found.
3) Option to un-install if already found when installing working
4) Optionally checks for fortan plugin and if found adds it to the installer not implemented.
5) Output filename includes date and 32 or 64 bit and version.
6) Added missing files that are included in the nightly build
7) X Install components not correct.
8) Easily swap between 32 and 64 bit build via one define change
9) Need to add the following link for new C++ users:
    http://www.sci.brooklyn.cuny.edu/~goetz/codeblocks/codeblocks-instructions.pdf


NSIS V ISS:
-----------
1) Need to compare install dialogs
2) Need to compare un-install dialogs
3) Need to check installed files are the same
4) Need to check fully un-installed : 
    4a) Program files
    4b) user data files
    4c) Registry enrtries
    4d) Menu entries
5) Check installed links 


