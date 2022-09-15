.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I./XMLRPC/include

XmlRpcEmbedderDLLdir = $(libdir)

XmlRpcEmbedderDLL_LTLIBRARIES = \
			libXmlRpcEmbedder.la

libXmlRpcEmbedder_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libXmlRpcEmbedder_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined \
            -lws2_32
endif

libXmlRpcEmbedder_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

libXmlRpcEmbedder_la_SOURCES = \
            ExecHiddenMSW.cpp \
            XMLRPC/src/XmlRpcClient.cpp \
            XMLRPC/src/XmlRpcDispatch.cpp \
            XMLRPC/src/XmlRpcServer.cpp \
            XMLRPC/src/XmlRpcServerConnection.cpp \
            XMLRPC/src/XmlRpcServerMethod.cpp \
            XMLRPC/src/XmlRpcSocket.cpp \
            XMLRPC/src/XmlRpcSource.cpp \
            XMLRPC/src/XmlRpcUtil.cpp \
            XMLRPC/src/XmlRpcValue.cpp \
            XmlRpcEvents.cpp \
            xmlrpc_embedder.cpp

noinst_HEADERS = \
            ExecHiddenMSW.h \
            XMLRPC/include/XmlRpc.h \
            XMLRPC/include/XmlRpcClient.h \
            XMLRPC/include/XmlRpcDispatch.h \
            XMLRPC/include/XmlRpcException.h \
            XMLRPC/include/XmlRpcServer.h \
            XMLRPC/include/XmlRpcServerConnection.h \
            XMLRPC/include/XmlRpcServerMethod.h \
            XMLRPC/include/XmlRpcSocket.h \
            XMLRPC/include/XmlRpcSource.h \
            XMLRPC/include/XmlRpcUtil.h \
            XMLRPC/include/XmlRpcValue.h \
            XMLRPC/include/base64.h \
            XmlRpcEvents.h \
            xmlrpc_embedder.h

EXTRA_DIST = \
			*.cbp
