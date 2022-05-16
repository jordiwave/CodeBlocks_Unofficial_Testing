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
			libheaderfixup.la

libheaderfixup_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libheaderfixup_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libheaderfixup_la_SOURCES = \
			bindings.cpp \
			configuration.cpp \
			defaults.cpp \
			execution.cpp \
			fileanalysis.cpp \
			headerfixup.cpp \
			helper.cpp \
			protocol.cpp

if CODEBLOCKS_NT
libheaderfixup_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			bindings.h \
			configuration.h \
			execution.h \
			fileanalysis.h \
			headerfixup.h \
			helper.h \
			protocol.h

EXTRA_DIST = \
			*.cbp \
			wxsmith/configuration.wxs \
			wxsmith/execution.wxs \
			wxsmith/protocol.wxs \
			todo.txt \
			manifest.xml

pkgdata_DATA = \
			headerfixup.zip
CLEANFILES = \
			$(pkgdata_DATA)

headerfixup.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/headerfixup.zip manifest.xml > /dev/null
