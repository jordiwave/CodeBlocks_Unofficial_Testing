TREELIST_DIST = \
			wxtreelist/wxsTreeListCtrl.cpp \
			wxtreelist/wxsTreeListCtrl.h

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/plugins/contrib/wxSmith \
			-I$(top_srcdir)/src/plugins/contrib/wxSmith/wxwidgets \
			-I$(top_srcdir)/src/plugins/contrib/wxSmith/wxwidgets/properties \
			-I$(top_srcdir)/src/plugins/contrib/wxSmith/properties \
			-I../wxContribItems/wxchart/include \
			-I../wxContribItems/wxFlatNotebook/include \
			-I../wxContribItems/wxImagePanel/include \
			-I../wxContribItems/wxthings/include \
			-I../wxContribItems/KWIC/include \
			$(TREELIST_INCLUDE) \
			-I../wxContribItems/wxSpeedButton \
			-I../wxContribItems/wxled/gizmosled/include \
			-I../wxContribItems/wxled/lcdWindow/include \
			-I../wxContribItems/wxled/led/include \
			-I../wxContribItems/wxled/ledpanel/include \
			-I../wxContribItems/wxmathplot

if CODEBLOCKS_NT
AM_CPPFLAGS += \
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
			libwxsmithcontribitems.la

libwxsmithcontribitems_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxsmithcontribitems_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxsmithcontribitems_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			../wxSmith/libwxsmithlib.la \
			../wxContribItems/wxchart/libwxchartctrl.la \
			../wxContribItems/wxFlatNotebook/libwxflatnotebook.la \
			../wxContribItems/wxImagePanel/libwximagepanel.la \
			../wxContribItems/wxSpeedButton/libwxspeedbutton.la \
			../wxContribItems/KWIC/libwxkwic.la \
			$(TREELIST_LA) \
			../wxContribItems/wxthings/libwxcustombutton.la \
			../wxContribItems/wxled/libwxled.la \
			../wxContribItems/wxmathplot/libwxmathplot.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libwxsmithcontribitems_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

EXTRA_DIST = \
			wxchart/*.xpm \
			wxchart/*.png \
			wximagebutton/*.xpm \
			wximagepanel/*.xpm \
			wxspeedbutton/*.xpm \
			wxthings/*.xpm \
			wxthings/*.png \
			wxflatnotebook/images/*.xpm \
			wxflatnotebook/images/*.png \
			wxSmithKWIC/images/*.xpm \
			wxtreelist/images/*.xpm \
			$(TREELIST_DIST) \
			wxgbsizer/images/*.xpm \
			wxled/images/*.xpm \
			wxSmithPlot/images/*.xpm \
			wxSmithPlot/test/*.h \
			wxSmithPlot/test/*.cpp \
			wxSmithPlot/test/resource.rc \
			wxSmithPlot/test/*.cbp \
			wxSmithPlot/test/wxsmith/*.wxs \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			wxsmithcontribitems.zip
CLEANFILES = \
			$(pkgdata_DATA)

wxsmithcontribitems.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmithcontribitems.zip manifest.xml > /dev/null

libwxsmithcontribitems_la_SOURCES = \
			./wxflatnotebook/wxsFlatNotebook.cpp \
			./wxchart/wxschart.cpp \
			./wxthings/wxscustombutton.cpp \
			./wximagebutton/wxsimagebutton.cpp \
			./wximagepanel/wxsimagepanel.cpp \
			./wxspeedbutton/wxsSpeedButton.cpp \
			./wxSmithKWIC/wxsangularmeter.cpp \
			./wxSmithKWIC/wxsangularregulator.cpp \
			./wxSmithKWIC/wxsbmpcheckbox.cpp \
			./wxSmithKWIC/wxsbmpswitcher.cpp \
			./wxSmithKWIC/wxslcdclock.cpp \
			./wxSmithKWIC/wxslcddisplay.cpp \
			./wxSmithKWIC/wxslinearmeter.cpp \
			./wxSmithKWIC/wxslinearregulator.cpp \
			$(TREELIST_SRC) \
			./wxgbsizer/wxsGridBagSizer.cpp \
			./wxled/wxsLcdWindow.cpp \
			./wxled/wxsLed.cpp \
			./wxled/wxsLedNumber.cpp \
			./wxled/wxsLedPanel.cpp \
			./wxled/wxsStateLed.cpp \
			./wxSmithPlot/wxsAxis.cpp \
			./wxSmithPlot/wxsMarker.cpp \
			./wxSmithPlot/wxsMathPlot.cpp \
			./wxSmithPlot/wxsText.cpp \
			./wxSmithPlot/wxsVector.cpp \
			./wxSmithContribItems.cpp

if CODEBLOCKS_NT
libwxsmithcontribitems_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			./wxflatnotebook/wxsFlatNotebook.h \
			./wxchart/wxschart.h \
			./wxthings/wxscustombutton.h \
			./wximagebutton/wxsimagebutton.h \
			./wximagepanel/wxsimagepanel.h \
			./wxspeedbutton/wxsSpeedButton.h \
			./wxSmithKWIC/wxsangularmeter.h \
			./wxSmithKWIC/wxsangularregulator.h \
			./wxSmithKWIC/wxsbmpcheckbox.h \
			./wxSmithKWIC/wxsbmpswitcher.h \
			./wxSmithKWIC/wxslcdclock.h \
			./wxSmithKWIC/wxslcddisplay.h \
			./wxSmithKWIC/wxslinearmeter.h \
			./wxSmithKWIC/wxslinearregulator.h \
			$(TREELIST_HDR) \
			./wxgbsizer/wxsGridBagSizer.h \
			./wxled/wxsLcdWindow.h \
			./wxled/wxsLed.h \
			./wxled/wxsLedNumber.h \
			./wxled/wxsLedPanel.h \
			./wxled/wxsStateLed.h \
			./wxSmithPlot/wxsAxis.h \
			./wxSmithPlot/wxsMarker.h \
			./wxSmithPlot/wxsMathPlot.h \
			./wxSmithPlot/wxsText.h \
			./wxSmithPlot/wxsVector.h \
			./wxSmithContribItems.h
