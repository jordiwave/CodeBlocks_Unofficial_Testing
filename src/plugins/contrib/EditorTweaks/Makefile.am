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
			libEditorTweaks.la

libEditorTweaks_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libEditorTweaks_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libEditorTweaks_la_SOURCES = \
			EditorTweaks.cpp \
			EditorTweaksConfDlg.cpp

if CODEBLOCKS_NT
libEditorTweaks_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			EditorTweaks.h \
			EditorTweaksConfDlg.h

EXTRA_DIST = \
			*.cbp \
			EditorTweaksConfDlg.xrc \
			manifest.xml \
			wxsmith/*.wxs

pkgdata_DATA = \
			EditorTweaks.zip

CLEANFILES = \
			$(pkgdata_DATA)

EditorTweaks.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/EditorTweaks.zip manifest.xml *.xrc > /dev/null
