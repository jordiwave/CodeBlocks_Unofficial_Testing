SUBDIRS = Resources

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
			libToolsPlus.la

libToolsPlus_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libToolsPlus_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libToolsPlus_la_SOURCES = \
			CmdConfigDialog.cpp \
			PipedProcessCtrl.cpp \
			se_globals.cpp \
			ShellCtrlBase.cpp \
			shellproperties.cpp \
			ToolsPlus.cpp

if CODEBLOCKS_NT
libToolsPlus_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			CmdConfigDialog.h \
			PipedProcessCtrl.h \
			se_globals.h \
			ShellCtrlBase.h \
			shellproperties.h \
			ToolsPlus.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			ToolsPlus.zip

CLEANFILES = \
			$(pkgdata_DATA)

ToolsPlus.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/ToolsPlus.zip manifest.xml > /dev/null

