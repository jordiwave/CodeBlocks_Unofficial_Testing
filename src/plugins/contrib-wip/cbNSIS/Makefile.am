if CODEBLOCKS_NT

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
			libcbNSIS.la

libcbNSIS_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libcbNSIS_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined

libcbNSIS_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

libcbNSIS_la_SOURCES = \
            AddComp.cpp \
            CompMap.cpp \
            CompTreeCtrl.cpp \
            NSIFile.cpp \
            NSISGUIFile.cpp \
            NSISGUIMain.cpp \
            cbNSIS.cpp

libcbNSIS_la_SOURCES += \
            manifest.rc

noinst_HEADERS = \
            AddComp.h \
            CompMap.h \
            CompTreeCtrl.h \
            NSIFile.h \
            NSISGUIFile.h \
            NSISGUIMain.h \
            cbNSIS.h \
            wx_pch.h

endif

pkgdata_DATA = \
			cbNSIS.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			manifest.*

EXTRA_DIST = \
			$(resources_forZIP)

cbNSIS.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/cbNSIS.zip $(resources_forZIP) > /dev/null
