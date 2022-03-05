SUBDIRS = resources

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
			libscriptedwizard.la

libscriptedwizard_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libscriptedwizard_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libscriptedwizard_la_SOURCES = \
			buildtargetpanel.cpp \
			compilerpanel.cpp \
			filepathpanel.cpp \
			genericselectpath.cpp \
			infopanel.cpp \
			genericsinglechoicelist.cpp \
			projectpathpanel.cpp \
			wiz.cpp \
			wizpage.cpp

noinst_HEADERS = \
			buildtargetpanel.h \
			compilerpanel.h \
			filepathpanel.h \
			genericselectpath.h \
			infopanel.h \
			genericsinglechoicelist.h \
			projectpathpanel.h \
			wiz.h \
			wizpage.h
