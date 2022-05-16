SUBDIRS = wxchart wxFlatNotebook wxthings wxImagePanel wxSpeedButton KWIC wxled wxmathplot

EXTRA_DIST = \
			*.cbp

AM_CPPFLAGS = \
			-DDLL_EXPORT \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-D_WIN64 \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE

if !CODEBLOCKS_NT_BUILD_INSTALLER
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
			wxsmith-contrib.pc \
			cb_wxcontrib.pc \
			cb_wxchartctrl.pc \
			cb_wxcustombutton.pc \
			cb_wxflatnotebook.pc \
			cb_wximagepanel.pc \
			cb_wxspeedbutton.pc \
			cb_wxKWIC.pc \
			cb_wxled.pc \
			cb_wxmathplot.pc
endif

