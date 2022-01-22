SUBDIRS = src/resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
	-I$(top_srcdir)/src/include \
	-I$(top_srcdir)/src/sdk/wxscintilla/include \
	-I$(top_srcdir)/src/include/tinyxml \
	-Isrc \
	-Isrc/LSPclient/include \
	-Isrc/LSPclient \
	-Isrc/codecompletion \
	-Isrc/unixprocess

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libclangdclient.la

libclangdclient_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libclangdclient_la_LIBADD = \
	../../sdk/libcodeblocks.la \
	-lutil \
	$(WX_LIBS) \
	$(CB_TINYXML_LIBS)

libclangdclient_la_SOURCES = \
	src/ClangLocator.cpp \
	src/codecompletion/ccoptionsdlg.cpp \
	src/codecompletion/ccoptionsprjdlg.cpp \
	src/codecompletion/cctreectrl.cpp \
	src/codecompletion/classbrowser.cpp \
	src/codecompletion/classbrowserbuilderthread.cpp \
	src/codecompletion/codecompletion.cpp \
	src/codecompletion/coderefactoring.cpp \
	src/codecompletion/doxygen_parser.cpp \
	src/codecompletion/gotofunctiondlg.cpp \
	src/codecompletion/insertclassmethoddlg.cpp \
	src/codecompletion/parsemanager.cpp \
	src/codecompletion/parsemanager_base.cpp \
	src/codecompletion/parser/ccdebuginfo.cpp \
	src/codecompletion/parser/cclogger.cpp \
	src/codecompletion/parser/expression.cpp \
	src/codecompletion/parser/LSP_symbolsparser.cpp \
	src/codecompletion/parser/LSP_tokenizer.cpp \
	src/codecompletion/parser/parser.cpp \
	src/codecompletion/parser/parser_base.cpp \
	src/codecompletion/parser/profiletimer.cpp \
	src/codecompletion/parser/searchtree.cpp \
	src/codecompletion/parser/token.cpp \
	src/codecompletion/parser/tokenizer.cpp \
	src/codecompletion/parser/tokentree.cpp \
	src/codecompletion/selectincludefile.cpp \
	src/codecompletion/wxsmith/CCTestFrame.wxs \
	src/LSPclient/src/client.cpp \
	src/LSPclient/lspdiagresultslog.cpp \
	src/unixprocess/asyncprocess/UnixProcess.cpp \
	src/unixprocess/asyncThreadTypes.cpp \
	src/unixprocess/fileutils.cpp \
	src/unixprocess/procutils.cpp \
	src/unixprocess/StringUtils.cpp

noinst_HEADERS = \
	src/ClangLocator.h \
	src/codecompletion/ccoptionsdlg.h \
	src/codecompletion/ccoptionsprjdlg.h \
	src/codecompletion/cctreectrl.h \
	src/codecompletion/classbrowser.h \
	src/codecompletion/classbrowserbuilderthread.h \
	src/codecompletion/codecompletion.h \
	src/codecompletion/coderefactoring.h \
	src/codecompletion/doxygen_parser.h \
	src/codecompletion/gotofunctiondlg.h \
	src/codecompletion/IdleCallbackHandler.h \
	src/codecompletion/insertclassmethoddlg.h \
	src/codecompletion/LSPEventCallbackHandler.h \
	src/codecompletion/parsemanager.h \
	src/codecompletion/parsemanager_base.h \
	src/codecompletion/parser/ccdebuginfo.h \
	src/codecompletion/parser/cclogger.h \
	src/codecompletion/parser/expression.h \
	src/codecompletion/parser/LSP_symbolsparser.h \
	src/codecompletion/parser/LSP_tokenizer.h \
	src/codecompletion/parser/parser.h \
	src/codecompletion/parser/parser_base.h \
	src/codecompletion/parser/profiletimer.h \
	src/codecompletion/parser/searchtree.h \
	src/codecompletion/parser/token.h \
	src/codecompletion/parser/tokenizer.h \
	src/codecompletion/parser/tokentree.h \
	src/codecompletion/selectincludefile.h \
	src/LSPclient/include/client.h \
	src/LSPclient/include/json.hpp \
	src/LSPclient/include/protocol.h \
	src/LSPclient/include/transport.h \
	src/LSPclient/include/uri.h \
	src/LSPclient/lspdiagresultslog.h \
	src/LSP_SymbolKind.h \
	src/unixprocess/asyncprocess/UnixProcess.h \
	src/unixprocess/asyncThreadTypes.h \
	src/unixprocess/fileutils.h \
	src/unixprocess/procutils.h \
	src/unixprocess/StringUtils.h \
	src/Version.h

EXTRA_DIST = \
	$(srcdir)/*.cbp
