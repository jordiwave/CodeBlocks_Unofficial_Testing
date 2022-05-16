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
			libcopystrings.la

libcopystrings_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libcopystrings_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libcopystrings_la_SOURCES = \
			copystrings.cpp

if CODEBLOCKS_NT
libcopystrings_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			copystrings.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			copystrings.zip

CLEANFILES = \
			$(pkgdata_DATA)

copystrings.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/copystrings.zip manifest.xml > /dev/null
