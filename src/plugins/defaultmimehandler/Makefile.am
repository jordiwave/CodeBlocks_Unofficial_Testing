SUBDIRS = resources

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
			libdefaultmimehandler.la

libdefaultmimehandler_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libdefaultmimehandler_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libdefaultmimehandler_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libdefaultmimehandler_la_SOURCES = \
			defaultmimehandler.cpp \
			editmimetypesdlg.cpp \
			EmbeddedHtmlPanel.cpp

if CODEBLOCKS_NT
libdefaultmimehandler_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			defaultmimehandler.h \
			editmimetypesdlg.h \
			mimetypesarray.h \
			EmbeddedHtmlPanel.h
