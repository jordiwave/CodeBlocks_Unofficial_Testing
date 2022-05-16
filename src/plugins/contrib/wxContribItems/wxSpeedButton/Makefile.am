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
			libwxspeedbutton.la

libwxspeedbutton_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxspeedbutton_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxspeedbutton_la_LIBADD = \
			$(WX_LIBS)

libwxspeedbutton_la_SOURCES = \
			./wxSpeedButton.cpp

if CODEBLOCKS_NT
libwxspeedbutton_la_SOURCES += \
			manifest.rc
endif

sdkdir = $(includedir)/codeblocks/wxContribItems/wxSpeedButton

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./wxSpeedButton.h
endif

