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
			libProjectOptionsManipulator.la

libProjectOptionsManipulator_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libProjectOptionsManipulator_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libProjectOptionsManipulator_la_SOURCES = \
			ProjectOptionsManipulator.cpp \
			ProjectOptionsManipulatorDlg.cpp \
			ProjectOptionsManipulatorResultDlg.cpp

if CODEBLOCKS_NT
libProjectOptionsManipulator_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			ProjectOptionsManipulator.h \
			ProjectOptionsManipulatorDlg.h \
			ProjectOptionsManipulatorResultDlg.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml \
			wxsmith/*.wxs

pkgdata_DATA = \
			ProjectOptionsManipulator.zip

CLEANFILES = \
			$(pkgdata_DATA)

ProjectOptionsManipulator.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ProjectOptionsManipulator.zip manifest.xml > /dev/null
