.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

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
			libdevpakupdater.la

libdevpakupdater_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libdevpakupdater_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libdevpakupdater_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			-lbz2 \
			$(WX_LIBS)


libdevpakupdater_la_SOURCES = \
			cbiniparser.cpp \
			cbnetwork.cpp \
			conf.cpp \
			crc32.cpp \
			devpakinstaller.cpp \
			devpakupdater.cpp \
			mytar.cpp \
			updatedlg.cpp

if CODEBLOCKS_NT
libdevpakupdater_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			cbiniparser.h \
			cbnetwork.h \
			conf.h \
			crc32.h \
			devpakinstaller.h \
			devpakupdater.h \
			mytar.h \
			updatedlg.h


EXTRA_DIST = \
			*.cbp \
			manifest.xml \
			update.xrc

pkgdata_DATA = \
			devpakupdater.zip

CLEANFILES = \
			$(pkgdata_DATA)

devpakupdater.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/devpakupdater.zip update.xrc manifest.xml  > /dev/null

