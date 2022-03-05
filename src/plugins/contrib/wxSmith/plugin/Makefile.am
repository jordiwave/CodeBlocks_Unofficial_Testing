AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-D__WXMSW__ \
			-DBUILDING_PLUGIN \
			-DHAVE_W32API_H \
			-pipe \
			-fmessage-length=0 \
			-fexceptions
endif

pluginlib_LTLIBRARIES = \
			libwxsmith.la

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

libwxsmith_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxsmith_la_LDFLAGS += \
			-enable-auto-image-base \
			-Wl,--export-all-symbols \
			-add-stdcall-alias \
			-enable-auto-import
			-no-undefined
endif

libwxsmith_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			../libwxsmithlib.la \
			$(WX_LIBS)

libwxsmith_la_SOURCES = \
			wxsmithpluginregistrants.cpp
