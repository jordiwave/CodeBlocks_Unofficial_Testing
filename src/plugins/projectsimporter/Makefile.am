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
			libprojectsimporter.la

libprojectsimporter_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libprojectsimporter_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libprojectsimporter_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libprojectsimporter_la_SOURCES = \
			projectsimporter.cpp \
			devcpploader.cpp \
			msvc10loader.cpp \
			msvc7loader.cpp \
			msvc7workspaceloader.cpp \
			msvcloader.cpp \
			msvcworkspacebase.cpp \
			msvcworkspaceloader.cpp

if CODEBLOCKS_NT
libprojectsimporter_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			projectsimporter.h \
			devcpploader.h \
			msvc10loader.h \
			msvc7loader.h \
			msvc7workspaceloader.h \
			msvcloader.h \
			msvcworkspacebase.h \
			msvcworkspaceloader.h
