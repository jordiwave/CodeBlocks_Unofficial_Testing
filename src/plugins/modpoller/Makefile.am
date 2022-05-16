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
			libModPoller.la

libModPoller_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libModPoller_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libModPoller_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			-lbz2 \
			$(WX_LIBS)

libModPoller_la_SOURCES = \
			ModPoller.cpp

if CODEBLOCKS_NT
libModPoller_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			ModPoller.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			ModPoller.zip

CLEANFILES = \
			$(pkgdata_DATA)

ModPoller.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/ModPoller.zip manifest.xml  > /dev/null

