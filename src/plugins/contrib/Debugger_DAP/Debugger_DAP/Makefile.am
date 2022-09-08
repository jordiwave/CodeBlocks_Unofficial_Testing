SUBDIRS = resources

.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I../DebugAdapterProtocol \
			-Itinyxml2

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
			libdebugger_dap.la

libdebugger_dap_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libdebugger_dap_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libdebugger_dap_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			../DebugAdapterProtocol/libdebugger_wxdap.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

if !CODEBLOCKS_NT
libdebugger_dap_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libdebugger_dap_la_SOURCES = \
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

if CODEBLOCKS_NT
libdebugger_dap_la_SOURCES += \
			resources/manifest.rc
endif

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
