.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(srcdir)/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-DWXMAKINGDLL_FNB
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwxflatnotebook.la

libwxflatnotebook_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxflatnotebook_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxflatnotebook_la_LIBADD = \
			$(WX_LIBS)

libwxflatnotebook_la_SOURCES = \
			./src/wxFlatNotebook/fnb_customize_dlg.cpp \
			./src/wxFlatNotebook/fnb_resources.cpp \
			./src/wxFlatNotebook/popup_dlg.cpp \
			./src/wxFlatNotebook/renderer.cpp \
			./src/wxFlatNotebook/wxFlatNotebook.cpp \
			./src/wxFlatNotebook/xh_fnb.cpp

if CODEBLOCKS_NT
libwxflatnotebook_la_SOURCES += \
			manifest.rc
endif

sdkdir = $(includedir)/codeblocks/wxContribItems/wxFlatNotebook/include/wx/wxFlatNotebook

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./include/wx/wxFlatNotebook/wxFlatNotebook.h \
			./include/wx/wxFlatNotebook/wxFlatNotebookSDK.h \
			./include/wx/wxFlatNotebook/wxFNBDropTarget.h \
			./include/wx/wxFlatNotebook/fnb_singleton.h \
			./include/wx/wxFlatNotebook/fnb_smart_ptr.h \
			./include/wx/wxFlatNotebook/renderer.h
endif

noinst_HEADERS = \
			./include/wx/wxFlatNotebook/fnb_customize_dlg.h \
			./include/wx/wxFlatNotebook/fnb_resources.h \
			./include/wx/wxFlatNotebook/popup_dlg.h \
			./include/wx/wxFlatNotebook/xh_fnb.h

EXTRA_DIST = \
			README
