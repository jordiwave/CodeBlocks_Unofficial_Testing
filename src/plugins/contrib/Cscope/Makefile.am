.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libCscope.la

libCscope_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libCscope_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libCscope_la_SOURCES = \
			CscopeConfig.cpp \
			CscopeEntryData.cpp \
			CscopeParserThread.cpp \
			CscopePlugin.cpp \
			CscopeProcess.cpp \
			CscopeStatusMessage.cpp \
			CscopeTab.cpp \
			CscopeView.cpp \
			CscopeConfigPanel.cpp

if CODEBLOCKS_NT
libCscope_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			CscopeConfig.h \
			CscopeEntryData.h \
			CscopeParserThread.h \
			CscopePlugin.h \
			CscopeProcess.h \
			CscopeStatusMessage.h \
			CscopeTab.h \
			CscopeView.h \
			CscopeConfigPanel.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			Cscope.zip

CLEANFILES = \
			$(pkgdata_DATA)

Cscope.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Cscope.zip manifest.xml > /dev/null
