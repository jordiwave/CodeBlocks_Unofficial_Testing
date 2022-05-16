.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-Iinclude

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwximagepanel.la

libwximagepanel_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwximagepanel_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwximagepanel_la_LIBADD = \
			$(WX_LIBS)

libwximagepanel_la_SOURCES = \
			src/wxImagePanel.cpp

if CODEBLOCKS_NT
libwximagepanel_la_SOURCES += \
			manifest.rc
endif

sdkdir = $(includedir)/codeblocks/wxContribItems/wxImagePanel/include/wx

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			include/wx/wxImagePanel.h
endif

