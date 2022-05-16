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
			libProfiler.la

libProfiler_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libProfiler_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libProfiler_la_SOURCES = \
			cbprofilerconfig.cpp \
			cbprofiler.cpp \
			cbprofilerexec.cpp

if CODEBLOCKS_NT
libProfiler_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			cbprofilerconfig.h \
			cbprofilerexec.h \
			cbprofiler.h

EXTRA_DIST = \
			*.cbp
