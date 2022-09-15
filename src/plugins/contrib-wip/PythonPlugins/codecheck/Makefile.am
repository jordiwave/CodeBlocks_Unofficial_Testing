.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

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
			libPythonCodeChecker.la

libPythonCodeChecker_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libPythonCodeChecker_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libPythonCodeChecker_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

libPythonCodeChecker_la_SOURCES = \
            asyncprocess.cpp \
            codechecker.cpp

if CODEBLOCKS_NT
libPythonCodeChecker_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
            asyncprocess.h \
            codechecker.h


pkgdata_DATA = \
			PythonCodeChecker.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			resources/manifest.* \
			resources/*.xrc

EXTRA_DIST = \
			$(resources_forZIP)

PythonCodeChecker.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip -jq9 $(PWD)/PythonCodeChecker.zip $(resources_forZIP) > /dev/null
