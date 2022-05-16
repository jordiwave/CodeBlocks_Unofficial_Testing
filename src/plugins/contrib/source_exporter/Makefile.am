SUBDIRS = wxPdfDocument

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_FONTCONFIG_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include \
			-include "sdk.h" \
			-DwxPDF_USE_WXMODULE=0

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXMAKINGLIB_PDFDOC \
			-DWXUSINGDLL
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libexporter.la

libexporter_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libexporter_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			wxPdfDocument/libwxPdfDocument.la \
			$(WX_LIBS) $(CB_FONTCONFIG_LIBS)

libexporter_la_SOURCES = \
			BaseExporter.cpp \
			exporter.cpp \
			HTMLExporter.cpp \
			ODTExporter.cpp \
			PDFExporter.cpp \
			RTFExporter.cpp

if CODEBLOCKS_NT
libexporter_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			BaseExporter.h \
			exporter.h \
			HTMLExporter.h \
			ODTExporter.h \
			PDFExporter.h \
			RTFExporter.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			exporter.zip

CLEANFILES = \
			$(pkgdata_DATA)

exporter.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/exporter.zip manifest.xml > /dev/null
