SUBDIRS = resources

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
			libtodo.la

libtodo_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libtodo_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la $(WX_LIBS)

libtodo_la_SOURCES = \
			addtododlg.cpp \
			asktypedlg.cpp \
			todo_parser.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

noinst_HEADERS = \
			addtododlg.h \
			asktypedlg.h \
			todo_parser.h \
			todolist.h \
			todolistview.h \
			todosettingsdlg.h
