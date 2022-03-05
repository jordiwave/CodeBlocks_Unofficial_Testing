AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			librndgen.la

librndgen_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

librndgen_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

librndgen_la_SOURCES = \
			rndgen.cpp

noinst_HEADERS = \
			rndgen.h

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			manifest.xml

pkgdata_DATA = \
			rndgen.zip

CLEANFILES = \
			$(pkgdata_DATA)

rndgen.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/rndgen.zip manifest.xml > /dev/null
