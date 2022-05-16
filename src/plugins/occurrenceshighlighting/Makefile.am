SUBDIRS = resources

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/scripting/bindings

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			liboccurrenceshighlighting.la

liboccurrenceshighlighting_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

liboccurrenceshighlighting_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

liboccurrenceshighlighting_la_SOURCES = \
			occurrenceshighlighting.cpp \
			occurrencespanel.cpp \
			occurrenceshighlightingconfigurationpanel.cpp \
			highlighter.cpp

if CODEBLOCKS_NT
liboccurrenceshighlighting_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			occurrenceshighlighting.h \
			occurrencespanel.h \
			occurrenceshighlightingconfigurationpanel.h \
			highlighter.h
