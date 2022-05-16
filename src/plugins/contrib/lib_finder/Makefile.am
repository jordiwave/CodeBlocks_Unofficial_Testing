SUBDIRS = lib_finder

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I../wxContribItems/wxFlatNotebook/include \
			-I$(top_srcdir)/src/include/scripting/bindings

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			liblib_finder.la

liblib_finder_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

liblib_finder_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			../wxContribItems/wxFlatNotebook/libwxflatnotebook.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
liblib_finder_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

liblib_finder_la_SOURCES = \
			librariesdlg.cpp \
			dirlistdlg.cpp \
			projectmissinglibs.cpp \
			headersdetectordlg.cpp \
			webresourcesmanager.cpp \
			projectconfiguration.cpp \
			resultmap.cpp \
			libraryresult.cpp \
			projectconfigurationpanel.cpp \
			pkgconfigmanager.cpp \
			librarydetectionmanager.cpp \
			libselectdlg.cpp \
			processingdlg.cpp \
			defsdownloaddlg.cpp \
			lib_finder.cpp

if CODEBLOCKS_NT
liblib_finder_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			librariesdlg.h \
			projectconfiguration.h \
			dirlistdlg.h \
			lib_finder.h \
			projectmissinglibs.h \
			webresourcesmanager.h \
			defsdownloaddlg.h \
			libraryresult.h \
			libselectdlg.h \
			headersdetectordlg.h \
			librarydetectionmanager.h \
			pkgconfigmanager.h \
			resultmap.h \
			processingdlg.h \
			librarydetectionconfig.h \
			projectconfigurationpanel.h

EXTRA_DIST = \
			*.cbp \
			readme.txt \
			manifest.xml \
			wxsmith/*.wxs

pkgdata_DATA = \
			lib_finder.zip
CLEANFILES = \
			$(pkgdata_DATA)

lib_finder.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/lib_finder.zip manifest.xml > /dev/null
