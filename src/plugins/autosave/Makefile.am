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
			libautosave.la

libautosave_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libautosave_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libautosave_la_SOURCES = \
			autosave.cpp

if CODEBLOCKS_NT
libautosave_la_SOURCES += \
			manifest.rc
endif

noinst_HEADERS = \
			autosave.h

EXTRA_DIST = \
			autosave.xrc \
			manifest.xml

pkgdata_DATA = \
			autosave.zip

CLEANFILES = \
			$(pkgdata_DATA)

autosave.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/autosave.zip manifest.xml *.xrc > /dev/null
