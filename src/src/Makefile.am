SUBDIRS = resources

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_SQUIRREL_CFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include

if GTK_NOTEBOOK
GTK_NOTEBOOK_FLAGS = \
			-DUSE_GTK_NOTEBOOK
endif

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DDEPSLIB_WINDOWS \
			-DBUILDING_PLUGIN \
			-I../exchndl/win_10/win64/include
else
AM_CPPFLAGS += \
			-DENABLE_BINRELOC \
			-DAPP_PREFIX="\"@prefix@\"" \
			$(WX_GTK_CFLAGS) \
			$(GTK_NOTEBOOK_FLAGS)
endif

bin_PROGRAMS = codeblocks

# wxMac resource fork/unbundled app
install-exec-hook:
			$(POSTLINK_COMMAND) $(DESTDIR)$(bindir)/codeblocks$(EXEEXT) \
			$(srcdir)/resources/icons/app.r
			$(MACSETFILE) -a C $(DESTDIR)$(bindir)/codeblocks$(EXEEXT)

if CODEBLOCKS_NT
codeblocks_LDADD = \
			-lcodeblocks.dll \
			-lexchndl \
			-lgdi32 \
			-lcomctl32 \
			$(WX_LIBS) \
			-lsquirrel

codeblocks_LDFLAGS = \
			-no-undefined \
			-L../sdk/.libs \
			-L../exchndl/win_10/win64/lib \
			-L../sdk/scripting/squirrel/.libs
else
# wxAUI uses GTK+ libraries on wxGTK
# codeblocks_LDFLAGS = $(WX_GTK_LIBS)

codeblocks_LDADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)
endif

codeblocks_SOURCES = \
			app.cpp \
			appglobals.cpp \
			backtracedlg.cpp \
			breakpointsdlg.cpp \
			compilersettingsdlg.cpp \
			cpuregistersdlg.cpp \
			crashhandler.cpp \
			debugger_interface_creator.cpp \
			debuggermenu.cpp \
			debuggersettingscommonpanel.cpp \
			debuggersettingsdlg.cpp \
			debuggersettingspanel.cpp \
			disassemblydlg.cpp \
			dlgabout.cpp \
			dlgaboutplugin.cpp \
			dlghelpsysteminformation.cpp \
			editkeywordsdlg.cpp \
			editorconfigurationdlg.cpp \
			environmentsettingsdlg.cpp \
			examinememorydlg.cpp \
			find_replace.cpp \
			goto_file.cpp \
			infopane.cpp \
			main.cpp \
			notebookstyles.cpp \
			printdlg.cpp \
			projectmanagerui.cpp \
			projectdepsdlg.cpp \
			projectoptionsdlg.cpp \
			recentitemslist.cpp \
			scriptconsole.cpp \
			scriptingsettingsdlg.cpp \
			splashscreen.cpp \
			startherepage.cpp \
			switcherdlg.cpp \
			threadsdlg.cpp \
			virtualbuildtargetsdlg.cpp \
			watchesdlg.cpp

noinst_HEADERS = \
			app.h \
			appglobals.h \
			backtracedlg.h \
			batchbuild.h \
			breakpointsdlg.h \
			compilersettingsdlg.h \
			cpuregistersdlg.h \
			crashhandler.h \
			debugger_interface_creator.h \
			debuggermenu.h \
			debuggersettingscommonpanel.h \
			debuggersettingsdlg.h \
			debuggersettingspanel.h \
			disassemblydlg.h \
			dlgabout.h \
			dlgaboutplugin.h \
			dlghelpsysteminformation.h \
			editkeywordsdlg.h \
			editorconfigurationdlg.h \
			environmentsettingsdlg.h \
			examinememorydlg.h \
			find_replace.h \
			goto_file.h \
			infopane.h \
			main.h \
			notebookstyles.h \
			printdlg.h \
			projectmanagerui.h \
			projectdepsdlg.h \
			projectoptionsdlg.h \
			recentitemslist.h \
			scriptconsole.h \
			scriptingsettingsdlg.h \
			splashscreen.h \
			startherepage.h \
			switcherdlg.h \
			threadsdlg.h \
			virtualbuildtargetsdlg.h \
			watchesdlg.h

if CODEBLOCKS_NT
codeblocks_SOURCES += \
			associations.cpp
noinst_HEADERS += \
			associations.h
endif

if !CODEBLOCKS_NT_BUILD_INSTALLER
man_MANS = \
			codeblocks.1

EXTRA_DIST = \
			associations.cpp \
			associations.h \
			ipc.h \
			ipc.cpp \
			$(man_MANS)
endif