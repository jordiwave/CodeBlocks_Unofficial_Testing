# SUBDIRS = bzip2

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
			libtidycmt.la

libtidycmt_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libtidycmt_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libtidycmt_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)


libtidycmt_la_SOURCES = \
			tidycmt.cpp \
			tidycmtsettings.cpp \
			tidycmtsettingswrapper.cpp

noinst_HEADERS = \
			tidycmt.h \
			tidycmtconfig.h \
			tidycmtsettings.h \
			tidycmtsettingswrapper.h

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			manifest.xml \
			TidyCmtSettings.wxs

pkgdata_DATA = \
			tidycmt.zip

CLEANFILES = \
			$(pkgdata_DATA)

tidycmt.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/tidycmt.zip manifest.xml  > /dev/null

