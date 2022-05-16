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
			libcodestat.la

libcodestat_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libcodestat_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libcodestat_la_SOURCES = \
			codestatconfig.cpp \
			codestat.cpp \
			codestatexec.cpp

if CODEBLOCKS_NT
libcodestat_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			codestatconfig.h \
			codestatexec.h \
			codestat.h \
			language_def.h

EXTRA_DIST = \
			*.cbp \
			InstallationInstructions.txt \
			BuildInstructions.txt
