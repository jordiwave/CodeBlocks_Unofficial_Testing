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
			libEditorConfig.la

libEditorConfig_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libEditorConfig_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libEditorConfig_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libEditorConfig_la_SOURCES = \
			EditorConfigUI.cpp \
			EditorConfig.cpp

noinst_HEADERS = \
			EditorConfig.h \
			EditorConfigCommon.h \
			EditorConfigUI.h

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			wxsmith/EditorConfigUI.wxs \
			manifest.xml

pkgdata_DATA = \
			EditorConfig.zip
CLEANFILES = \
			$(pkgdata_DATA)

EditorConfig.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/EditorConfig.zip manifest.xml > /dev/null
