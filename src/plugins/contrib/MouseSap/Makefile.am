.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(WX_GTK_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libMouseSap.la

libMouseSap_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libMouseSap_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS) $(WX_GTK_LIBS)

libMouseSap_la_SOURCES = \
			MouseSap.cpp \
			MouseSapCfg.cpp

if CODEBLOCKS_NT
libMouseSap_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			MouseSap.h \
			MouseSapCfg.h

EXTRA_DIST = \
			*.cbp \
			manifest.xml

pkgdata_DATA = \
			MouseSap.zip

CLEANFILES = \
			$(pkgdata_DATA)

MouseSap.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/MouseSap.zip manifest.xml > /dev/null
