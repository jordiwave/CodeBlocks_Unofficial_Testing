.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I./tinyxml2

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
                libdebugger_gdbmi.la

libdebugger_gdbmi_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libdebugger_gdbmi_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libdebugger_gdbmi_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libdebugger_gdbmi_la_SOURCES = \
                src/actions.cpp \
                src/cmd_queue.cpp \
                src/cmd_result_parser.cpp \
				src/cmd_result_tokens.cpp \
                src/config.cpp \
                src/definitions.cpp	\
				src/escape.cpp \
                src/events.cpp \
                src/frame.cpp \
                src/gdb_executor.cpp	\
				src/helpers.cpp \
                src/plugin.cpp \
                src/updated_variable.cpp
				
noinst_HEADERS = \
                src/config.h \
                src/frame.h \
                src/helpers.h \
                src/gdb_executor.h \
                src/cmd_queue.h \
                src/updated_variable.h \
                src/cmd_result_parser.h \
                src/actions.h \
                src/definitions.h \
                src/events.h \
                src/escape.h \
                src/cmd_result_tokens.h \
                src/plugin.h


################ resources ########################

pkgdata_DATA = \
			debugger_gdbmi.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
            resources/*.xrc \
            resources/manifest.*

EXTRA_DIST = \
			$(resources_forZIP)

debugger_gdbmi.zip: $(debugger_gdbmi_RESOURCES)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/debugger_gdbmi.zip $(resources_forZIP) > /dev/null
