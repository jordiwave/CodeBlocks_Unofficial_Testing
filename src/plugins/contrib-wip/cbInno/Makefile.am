if CODEBLOCKS_NT
SUBDIRS = resources

.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

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

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libcbInno.la

libcbInno_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libcbInno_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined

libcbInno_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)
libcbInno_la_SOURCES = \
			src/DAP_Breakpoints.cpp \
			src/DAP_CallStack.cpp \
			src/DAP_Debugger_State.cpp \
			src/DAP_Watches.cpp \
			src/debugger_logger.cpp \
			src/definitions.cpp \
			src/dlg_ProjectOptions.cpp \
			src/dlg_SettingsOptions.cpp \
			src/dlg_WatchEdit.cpp \
			src/plugin.cpp \
			tinyxml2/tinyxml2.cpp

libcbInno_la_SOURCES += \
			resources/manifest.rc

noinst_HEADERS = \
			src/DAP_Breakpoints.h \
			src/DAP_CallStack.h \
			src/DAP_Debugger_State.h \
			src/DAP_Watches.h \
			src/debugger_logger.h \
			src/definitions.h \
			src/dlg_ProjectOptions.h \
			src/dlg_SettingsOptions.h \
			src/dlg_WatchEdit.h \
			src/plugin.h \
			tinyxml2/tinyxml2.h
endif

pkgdata_DATA = \
			cbInno.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			resources/manifest.*

EXTRA_DIST = \
			$(resources_forZIP)

cbInno.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/cbInno.zip $(resources_forZIP) > /dev/null
