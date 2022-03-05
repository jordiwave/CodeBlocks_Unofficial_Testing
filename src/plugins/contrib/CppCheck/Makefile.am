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
			libCppCheck.la

libCppCheck_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libCppCheck_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libCppCheck_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libCppCheck_la_SOURCES = \
			CppCheck.cpp \
			CppCheckListLog.cpp \
			ConfigPanel.cpp

noinst_HEADERS = \
			CppCheck.h \
			CppCheckListLog.h \
			ConfigPanel.h

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			wxsmith
