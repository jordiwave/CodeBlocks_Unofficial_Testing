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
			libkeybinder.la

libkeybinder_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libkeybinder_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libkeybinder_la_SOURCES = \
			cbKeybinder.cpp \
			cbKeyConfigPanel.cpp \
			cJSON.cpp \
			clKeyboardBindingConfig.cpp \
			clKeyboardManager.cpp \
			json_node.cpp \
			Keybinder.cpp \
			menuutils.cpp

if CODEBLOCKS_NT
libkeybinder_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			cbKeybinder.h \
			cbKeyConfigPanel.h \
			cJSON.h \
			clKeyboardBindingConfig.h \
			clKeyboardManager.h \
			debugging.h \
			json_node.h \
			Keybinder.h \
			macros.h \
			menuutils.h

EXTRA_DIST = \
			*.cbp manifest.xml

pkgdata_DATA = \
			keybinder.zip

CLEANFILES = \
			$(pkgdata_DATA)

keybinder.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/keybinder.zip manifest.xml > /dev/null
