SUBDIRS = resources

.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I./include

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-DCC_NO_COLLAPSE_ITEM \
			-DLOGGING \
			-DDONT_SHOW_SERVER_CONSOLE \
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
			libcbSystemView.la

libcbSystemView_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libcbSystemView_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libcbSystemView_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

libcbSystemView_la_SOURCES = \
            cbSystemView.cpp \
            src/cbSVProjectConfigPanel.cpp \
            src/cbSVDFileReader.cpp \
            src/cbSVPGProperties.cpp \
            src/cbSVWindow.cpp

if CODEBLOCKS_NT
libcbSystemView_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
            cbSystemView.h \
            include/cbSVProjectConfigPanel.h \
            include/cbSVDFileReader.h \
            include/cbSVPGProperties.h \
            include/cbSVWindow.h \
            wxsmith/ProjectConfigPanel.wxs

