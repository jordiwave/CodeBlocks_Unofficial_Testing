SUBDIRS = resources

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(WX_GTK_CFLAGS) \
			$(WX_X11_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/plugins/contrib/codesnippets

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libcodesnippets.la

libcodesnippets_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libcodesnippets_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS) \
			$(WX_X11_LIBS)

if !CODEBLOCKS_NT
libcodesnippets_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libcodesnippets_la_SOURCES = \
			codesnippets.cpp \
			codesnippetstreectrl.cpp \
			codesnippetswindow.cpp \
			settingsdlg.cpp \
			settingsdlgform.cpp \
			snippetitemdata.cpp \
			snippetproperty.cpp \
			snippetpropertyform.cpp \
			snippetsconfig.cpp \
			snippetsimages.cpp \
			version.cpp

noinst_HEADERS = \
			codesnippets.h \
			codesnippetstreectrl.h \
			codesnippetswindow.h \
			menuidentifiers.h \
			settingsdlg.h \
			settingsdlgform.h \
			snippetitemdata.h \
			snippetproperty.h \
			snippetpropertyform.h \
			snippetsconfig.h \
			snippetsimages.h \
			FileImport.h \
			version.h \
			wx_pch.h

if !CODEBLOCKS_NT_BUILD_INSTALLER
man_MANS = \
			codesnippets.1

EXTRA_DIST = \
			$(srcdir)/*.cbp \
			generic.rc \
			generic.Manifest \
			$(srcdir)/update* \
			readme.html \
			license.txt \
			manifest.xml \
			$(man_MANS)
endif

