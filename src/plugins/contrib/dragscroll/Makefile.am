AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/src \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libdragscroll.la

libdragscroll_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libdragscroll_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libdragscroll_la_SOURCES = \
			dragscroll.cpp \
			dragscrollcfg.cpp \
			dragscrollevent.cpp

noinst_HEADERS = \
			dragscroll.h \
			dragscrollcfg.h \
			dragscrollevent.h

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			manifest.xml

pkgdata_DATA = \
			dragscroll.zip
CLEANFILES = \
			$(pkgdata_DATA)

dragscroll.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/dragscroll.zip manifest.xml > /dev/null
