.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS)

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-Dmathplot_EXPORTS
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwxmathplot.la

libwxmathplot_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxmathplot_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxmathplot_la_LIBADD = \
			$(WX_LIBS)

libwxmathplot_la_SOURCES = \
			./mathplot.cpp

if CODEBLOCKS_NT
libwxmathplot_la_SOURCES += \
			manifest.rc
endif

sdkdir = $(includedir)/codeblocks/wxContribItems/wxMathPlot

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./mathplot.h
endif

