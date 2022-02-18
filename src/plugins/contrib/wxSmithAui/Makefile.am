AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/plugins/contrib/wxSmith \
			-I$(srcdir)/wxAuiManager \
			-I$(srcdir)/wxAuiNotebook \
			-I$(srcdir)/wxAuiToolBar

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
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
			libwxSmithAui.la

libwxSmithAui_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxSmithAui_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxSmithAui_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			../wxSmith/libwxsmithlib.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libwxSmithAui_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

EXTRA_DIST = \
			$(srcdir)/images/*.xpm \
			$(srcdir)/images/*.xcf \
			$(srcdir)/wxsmith/*.wxs \
			$(srcdir)/*.cbp \
			manifest.xml

pkgdata_DATA = \
			wxSmithAui.zip

CLEANFILES = \
			$(pkgdata_DATA)

wxSmithAui.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxSmithAui.zip manifest.xml > /dev/null

libwxSmithAui_la_SOURCES = \
			wxAuiManager/wxsAuiManager.cpp \
			./wxAuiManager/wxsAuiManagerParentQP.cpp \
			./wxAuiManager/wxsAuiPaneInfoProperty.cpp \
			./wxAuiNotebook/wxsAuiNotebook.cpp \
			./wxAuiNotebook/wxSmithAuiNotebook.cpp \
			./wxAuiToolBar/wxsAuiToolBar.cpp \
			./wxAuiToolBar/wxsAuiToolBarItem.cpp \
			./wxAuiToolBar/wxsAuiToolBarItemBase.cpp \
			./wxAuiToolBar/wxsAuiToolBarLabel.cpp \
			./wxAuiToolBar/wxsAuiToolBarSeparator.cpp \
			./wxAuiToolBar/wxsAuiToolBarSpacer.cpp \
			./wxAuiToolBar/wxSmithAuiToolBar.cpp \
			./wxSmithAui.cpp

if !CODEBLOCKS_NT_BUILD_INSTALLER
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
			wxsmithaui.pc
endif

noinst_HEADERS = \
			wxAuiManager/wxsAuiManager.h \
			./wxAuiManager/wxsAuiManagerParentQP.h \
			./wxAuiManager/wxsAuiPaneInfoProperty.h \
			./wxAuiManager/wxSmithAuiManager.h \
			./wxAuiNotebook/wxsAuiNotebook.h \
			./wxAuiNotebook/wxSmithAuiNotebook.h \
			./wxAuiToolBar/wxsAuiToolBar.h \
			./wxAuiToolBar/wxsAuiToolBarItem.h \
			./wxAuiToolBar/wxsAuiToolBarItemBase.h \
			./wxAuiToolBar/wxsAuiToolBarLabel.h \
			./wxAuiToolBar/wxsAuiToolBarSeparator.h \
			./wxAuiToolBar/wxsAuiToolBarSpacer.h \
			./wxAuiToolBar/wxSmithAuiToolBar.h \
			./wxSmithAui.h
