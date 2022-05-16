SUBDIRS = resources

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libabbreviations.la

libabbreviations_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libabbreviations_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libabbreviations_la_SOURCES = \
			abbreviations.cpp \
			abbreviationsconfigpanel.cpp

if CODEBLOCKS_NT
libabbreviations_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			abbreviations.h \
			abbreviationsconfigpanel.h
