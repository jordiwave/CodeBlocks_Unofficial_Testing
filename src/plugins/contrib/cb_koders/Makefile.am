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
			libcb_koders.la

libcb_koders_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libcb_koders_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libcb_koders_la_SOURCES = \
			kodersdialog.cpp \
			cb_koders.cpp

if CODEBLOCKS_NT
libcb_koders_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			kodersdialog.h \
			cb_koders.h

EXTRA_DIST = \
			*.cbp \
			wxsmith/KodersDialog.wxs \
			manifest.xml

pkgdata_DATA = \
			cb_koders.zip
CLEANFILES = \
			$(pkgdata_DATA)

cb_koders.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/cb_koders.zip manifest.xml > /dev/null
