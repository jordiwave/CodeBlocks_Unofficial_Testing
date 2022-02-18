SUBDIRS = resources

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
			libCccc.la

libCccc_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libCccc_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libCccc_la_SOURCES = \
			Cccc.cpp

noinst_HEADERS = \
			Cccc.h

EXTRA_DIST = \
			$(srcdir)/*.cbp
