SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

if !CODEBLOCKS_NT_BUILD_INSTALLER
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = codeblocks.pc
endif

# Add some files needed only when making a bundle

if DARWIN
osxbundledir = $(pkgdatadir)/osx_bundle
dist_osxbundle_DATA = \
			bundle.sh \
			codeblocks.plist

osxbundle_iconsdir = $(osxbundledir)/icons
dist_osxbundle_icons_DATA = \
			$(top_srcdir)/src/src/resources/icons/*.icns
endif

EXTRA_DIST = \
			bootstrap \
			codeblocks.spec \
			codeblocks.spec.fedora \
			codeblocks.plist \
			Readme_Build_Debian_deb_packages.txt
			Readme_Build_Linux_by_Makefile.txt
			Readme_Build_Linux_by_Workspace.txt
			Readme_Build_Windows_by_Workspace.txt
			Readme_Build_Windows_MSYS2_by_Makefile.txt
			Readme_License_GPL3.TXT
			Readme_Release_Checklist.txt
			$(srcdir)/update* \
			ChangeLog.xsl \
			codeblocks.appdata.xml \
			codeblocks-contrib.metainfo.xml \
			debian

clean-zipfiles:
	find . -name "*.zip" | xargs rm -f

clean-bin:
	find . -name "*.la" | xargs rm -f
	rm -f src/src/codeblocks

if !CODEBLOCKS_NT_BUILD_INSTALLER
appdatadir = $(datarootdir)/metainfo
dist_appdata_DATA = \
			$(top_srcdir)/codeblocks.appdata.xml
endif