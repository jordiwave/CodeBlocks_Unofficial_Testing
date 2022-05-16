SUBDIRS = resources

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
			libValgrind.la

libValgrind_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libValgrind_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libValgrind_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libValgrind_la_SOURCES = \
			Valgrind.cpp \
			ValgrindListLog.cpp \
			valgrind_config.cpp

if CODEBLOCKS_NT
libValgrind_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			Valgrind.h \
			ValgrindListLog.h \
			valgrind_config.h

EXTRA_DIST = \
			*.cbp \
			wxsmith/valgrind_config_panel.wxs
