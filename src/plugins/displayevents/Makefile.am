.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libDisplayEvents.la

libDisplayEvents_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libDisplayEvents_la_LIBADD = \
			PLUGIN_WX_LIBS) \
			$(top_srcdir)/src/sdk/libcodeblocks.la

libDisplayEvents_la_SOURCES = \
			DisplayEvents.cpp

if CODEBLOCKS_NT
libDisplayEvents_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			DisplayEvents.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			DisplayEvents.zip

CLEANFILES = \
			$(pkgdata_DATA)

IncrementalSearch.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/DisplayEvents.zip manifest.xml > /dev/null
