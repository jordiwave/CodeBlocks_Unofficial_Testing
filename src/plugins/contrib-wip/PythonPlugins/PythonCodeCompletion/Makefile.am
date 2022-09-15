.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I../XmlRpcEmbedder/XMLRPC/include \
			-I../XmlRpcEmbedder

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-DCC_NO_COLLAPSE_ITEM \
			-DLOGGING \
			-DDONT_SHOW_SERVER_CONSOLE \
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
			-fno-keep-inline-dllexport \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DcbDEBUG \
			-DwxUSE_UNICODE \
			-D_WIN64
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libPythonCodeCompletion.la

libPythonCodeCompletion_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libPythonCodeCompletion_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libPythonCodeCompletion_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS) \
			../XmlRpcEmbedder/libXmlRpcEmbedder.la

libPythonCodeCompletion_la_SOURCES = \
            PythonCodeCompletion.cpp

if CODEBLOCKS_NT
libPythonCodeCompletion_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
            PythonCodeCompletion.h \
            python/jedi_example.py \
            python/python_completion_server.py \
            python/stdlib_parser.py

pkgdata_DATA = \
			PythonCodeCompletion.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			resources/manifest.* \
			resources/*.xrc

EXTRA_DIST = \
			$(resources_forZIP)

PythonCodeCompletion.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip -jq9 $(PWD)/PythonCodeCompletion.zip $(resources_forZIP) > /dev/null
