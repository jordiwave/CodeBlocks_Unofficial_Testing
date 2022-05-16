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
			libSymTab.la

libSymTab_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libSymTab_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libSymTab_la_SOURCES = \
			symtabconfig.cpp \
			symtab.cpp \
			symtabexec.cpp

if CODEBLOCKS_NT
libSymTab_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			symtabconfig.h \
			symtabexec.h \
			symtab.h

EXTRA_DIST = \
			*.cbp
