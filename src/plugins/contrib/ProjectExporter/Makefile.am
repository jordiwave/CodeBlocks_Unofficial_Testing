.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

pkgdata_DATA = \
			ProjectExporter.zip

CLEANFILES = \
			$(pkgdata_DATA)

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/tinyxml

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-DCC_NO_COLLAPSE_ITEM \
			-DLOGGING \
			-DDONT_SHOW_SERVER_CONSOLE
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
			-fno-keep-inline-dllexport \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DcbDEBUG \
			-DwxUSE_UNICODE \
			-D_WIN64
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libProjectExporter.la

libProjectExporter_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libProjectExporter_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libProjectExporter_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libProjectExporter_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libProjectExporter_la_SOURCES = \
			AutotoolsExporter.cpp \
			BakefileExporter.cpp \
			CMakeListsExporter.cpp \
			ExporterBase.cpp \
			PremakeDlg.cpp \
			PremakeExporter.cpp \
			ProjectExporter.cpp

noinst_HEADERS = \
			AutotoolsExporter.h \
			BakefileExporter.h \
			CMakeListsExporter.h \
			ExporterBase.h \
			PremakeDlg.h \
			PremakeExporter.h \
			ProjectExporter.h

resources_forZIP = \
			manifest.xml \
			PremakeDlg.xrc

EXTRA_DIST = \
			$(resources_forZIP)

ProjectExporter.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/ProjectExporter.zip $(resources_forZIP) > /dev/null
