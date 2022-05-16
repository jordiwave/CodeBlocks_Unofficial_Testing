.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libxpmanifest.la

libxpmanifest_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libxpmanifest_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libxpmanifest_la_SOURCES = \
			windowsxplooknfeel.cpp

if CODEBLOCKS_NT
libxpmanifest_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			windowsxplooknfeel.h

EXTRA_DIST = \
			manifest.xml
pkgdata_DATA = \
			xpmanifest.zip
CLEANFILES = \
			$(pkgdata_DATA)

xpmanifest.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/xpmanifest.zip manifest.xml > /dev/null
