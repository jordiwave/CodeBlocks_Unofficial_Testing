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
			libAutoVersioning.la

libAutoVersioning_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libAutoVersioning_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libAutoVersioning_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libAutoVersioning_la_SOURCES = \
			AutoVersioning.cpp \
			avChangesDlg.cpp \
			avHeader.cpp \
			avSvnRevision.cpp \
			avVersionEditorDlg.cpp

if CODEBLOCKS_NT
libAutoVersioning_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			AutoVersioning.h \
			avChangesDlg.h \
			avConfig.h \
			avHeader.h \
			avSvnRevision.h \
			avVersionEditorDlg.h

EXTRA_DIST = \
			wxsmith/avChangesDlg.wxs \
			wxsmith/avVersionEditorDlg.wxs \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			AutoVersioning.zip
CLEANFILES = \
			$(pkgdata_DATA)

AutoVersioning.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/AutoVersioning.zip manifest.xml > /dev/null
