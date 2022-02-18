AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(srcdir)/include

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
			-version-info 0:1:0

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

sdkdir = $(includedir)/codeblocks/wxContribItems/wxSpeedButton

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./wxSpeedButton.h
endif

