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
			libReopenEditor.la

libReopenEditor_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libReopenEditor_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libReopenEditor_la_SOURCES = \
			ReopenEditor.cpp \
			ReopenEditorConfDLg.cpp \
			ReopenEditorListView.cpp

if CODEBLOCKS_NT
libReopenEditor_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			ReopenEditor.h \
			ReopenEditorConfDLg.h \
			ReopenEditorListView.h

EXTRA_DIST = \
			*.cbp \
			*.xrc \
			wxsmith/*.wxs \
			manifest.xml

pkgdata_DATA = \
			ReopenEditor.zip

CLEANFILES = \
			$(pkgdata_DATA)

ReopenEditor.zip: manifest.xml ReopenEditorConfDLg.xrc
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/ReopenEditor.zip manifest.xml *.xrc > /dev/null
