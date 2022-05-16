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
			-DWXUSINGDLL \
			-DWXMAKINGDLL_THINGS
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwxcustombutton.la

libwxcustombutton_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxcustombutton_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxcustombutton_la_LIBADD = \
			$(WX_LIBS)

libwxcustombutton_la_SOURCES = \
			./src/toggle.cpp

if CODEBLOCKS_NT
libwxcustombutton_la_SOURCES += \
			manifest.rc
endif

sdkdir = $(includedir)/codeblocks/wxContribItems/wxthings/include/wx/things

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./include/wx/things/toggle.h \
			./include/wx/things/thingdef.h
endif

EXTRA_DIST = \
			./src/precomp.cpp ./src/precomp.h
