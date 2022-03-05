AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(srcdir)/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-DWXMAKINGDLL_CHART
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwxchartctrl.la

libwxchartctrl_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxchartctrl_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxchartctrl_la_LIBADD = \
			$(WX_LIBS)

libwxchartctrl_la_SOURCES = \
			./src/axis.cpp \
			./src/barchartpoints.cpp \
			./src/chart.cpp \
			./src/chartwindow.cpp \
			./src/legend.cpp \
			./src/pie3dchartpoints.cpp \
			./src/points.cpp \
			./src/xaxiswindow.cpp \
			./src/yaxiswindow.cpp \
			./src/bar3dchartpoints.cpp \
			./src/chartcolors.cpp \
			./src/chartctrl.cpp \
			./src/chartsizes.cpp \
			./src/label.cpp \
			./src/legendwindow.cpp \
			./src/piechartpoints.cpp \
			./src/xaxis.cpp \
			./src/yaxis.cpp

sdkdir = $(includedir)/codeblocks/wxContribItems/wxchart/include/wx

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./include/wx/axis.h \
			./include/wx/bar3dchartpoints.h \
			./include/wx/barchartpoints.h \
			./include/wx/chartcolors.h \
			./include/wx/chartctrl.h \
			./include/wx/chartdef.h \
			./include/wx/chart.h \
			./include/wx/chartpoints.h \
			./include/wx/chartpointstypes.h \
			./include/wx/chartsizes.h \
			./include/wx/charttypes.h \
			./include/wx/chartwindow.h \
			./include/wx/legend.h \
			./include/wx/legendwindow.h \
			./include/wx/pie3dchartpoints.h \
			./include/wx/piechartpoints.h \
			./include/wx/points.h \
			./include/wx/xaxis.h \
			./include/wx/xaxiswindow.h \
			./include/wx/yaxis.h \
			./include/wx/yaxiswindow.h
endif

noinst_HEADERS = \
			./include/wx/label.h

EXTRA_DIST = \
			$(srcdir)/include/wx/chartart/*.xpm \
			$(srcdir)/include/wx/chartart/*.bmp

