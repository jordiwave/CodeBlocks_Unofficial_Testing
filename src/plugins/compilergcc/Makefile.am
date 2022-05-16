SUBDIRS = depslib resources

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/plugins/compilergcc/depslib/src

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DDEPSLIB_WINDOWS \
			-DBUILDING_PLUGIN \
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DcbDEBUG \
			-DwxUSE_UNICODE \
			-D_WIN64
else
AM_CXXFLAGS = \
			-DDEPSLIB_UNIX
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libcompiler.la

libcompiler_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libcompiler_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libcompiler_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			depslib/src/libdepslib.la \
			$(WX_LIBS)

libcompiler_la_SOURCES = \
			advancedcompileroptionsdlg.cpp \
			compiler_defs.cpp \
			compilerCYGWIN.cpp \
			compilererrors.cpp \
			compilerflagdlg.cpp \
			compilergcc.cpp \
			compilerGDC.cpp \
			compilerICC.cpp \
			compilerIAR.cpp \
			compilerKeilC51.cpp \
			compilerLCC.cpp \
			compilermessages.cpp \
			compilerMINGW.cpp \
			compilerMINGWgenerator.cpp \
			compilerMSVC.cpp \
			compilerMSVC8.cpp \
			compilerMSVC10.cpp \
			compileroptionsdlg.cpp \
			compilerOW.cpp \
			compilerOWgenerator.cpp \
			compilerG95.cpp \
			compilerGNUARM.cpp \
			compilerGNUFortran.cpp \
			compilerXML.cpp \
			directcommands.cpp

if CODEBLOCKS_NT
libcompiler_la_SOURCES += \
			resources/manifest.rc
endif

noinst_HEADERS = \
			advancedcompileroptionsdlg.h \
			compiler_defs.h \
			compilerCYGWIN.h \
			compilererrors.h \
			compilerflagdlg.h \
			compilergcc.h \
			compilerG95.h \
			compilerGDC.h \
			compilerICC.h \
			compilerIAR.h \
			compilerKeilC51.h \
			compilerLCC.h \
			compilermessages.h \
			compilerMINGW.h \
			compilerMINGWgenerator.h \
			compilerMSVC.h \
			compilerMSVC8.h \
			compilerMSVC10.h \
			compileroptionsdlg.h \
			compilerOW.h \
			compilerOWgenerator.h \
			compilerGNUARM.h \
			compilerGNUFortran.h \
			compilerXML.h \
			directcommands.h
