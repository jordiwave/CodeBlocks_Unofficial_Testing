# SUBDIRS = bzip2

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DcbDEBUG \
			-DwxUSE_UNICODE \
			-D_WIN64
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libheaderguard.la

libheaderguard_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libheaderguard_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libheaderguard_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libheaderguard_la_SOURCES = \
			headerguard.cpp

if CODEBLOCKS_NT
libheaderguard_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			headerguard.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			headerguard.zip

CLEANFILES = \
			$(pkgdata_DATA)

headerguard.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/headerguard.zip manifest.xml  > /dev/null

