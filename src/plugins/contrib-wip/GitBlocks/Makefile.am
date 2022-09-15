.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I./include

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
			libGitBlocks.la

libGitBlocks_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libGitBlocks_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libGitBlocks_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libGitBlocks_la_SOURCES = \
			src/CloneDialog.cpp \
			src/CommitAllDialog.cpp \
			src/CommitDialog.cpp \
			src/GitBlocks.cpp \
			src/NewBranchDialog.cpp \
			src/RemoveDialog.cpp \
			src/SwitchBranchDialog.cpp \
			src/SetUserDialog.cpp

if CODEBLOCKS_NT
libGitBlocks_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			include/CloneDialog.h \
			include/CommitAllDialog.h \
			include/CommitDialog.h \
			include/GitBlocks.h \
			include/NewBranchDialog.h \
			include/RemoveDialog.h \
			include/SwitchBranchDialog.h \
			include/SetUserDialog.h

pkgdata_DATA = \
			GitBlocks.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			manifest.* \
			*.xrc

EXTRA_DIST = \
			*.cbp \
			wxsmith/cbDiffConfigPanel.wxs \
			wxsmith/cbDiffSelectFiles.wxs \
			$(resources_forZIP)

GitBlocks.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/GitBlocks.zip $(resources_forZIP) > /dev/null
