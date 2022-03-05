
cb_plugin_lib_LTLIBRARIES = \
			GitBlocks.la
cb_plugin_data_DATA = \
			GitBlocks.zip GitBlocks.cbplugin
CLEANFILES = \
			GitBlocks.zip GitBlocks.cbplugin

GitBlocks_la_SOURCES = \
			src/CloneDialog.cpp \
			src/CommitAllDialog.cpp \
			src/CommitDialog.cpp \
			src/GitBlocks.cpp \
			src/NewBranchDialog.cpp \
			src/RemoveDialog.cpp \
			src/SwitchBranchDialog.cpp \
			src/SetUserDialog.cpp

noinst_HEADERS = \
			include/CloneDialog.h \
			include/CommitAllDialog.h \
			include/CommitDialog.h \
			include/GitBlocks.h \
			include/NewBranchDialog.h \
			include/RemoveDialog.h \
			include/SwitchBranchDialog.h \
			include/SetUserDialog.h

GitBlocks_la_CPPFLAGS = \
			$(WX_CFLAGS) \
			$(CB_CFLAGS) \
			-Iinclude

GitBlocks_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version \
			$(WX_LIBS) \
			$(CB_LIBS)

GitBlocks_zip_RESOURCES = \
			manifest.xml

GitBlocks.zip: $(GitBlocks_zip_RESOURCES)
	cd $(srcdir)
	zip -j9 GitBlocks.zip manifest.xml

GitBlocks_cbplugin_RESOURCES = \
			cb_plugin_lib_LTLIBRARIES) GitBlocks.zip

GitBlocks.cbplugin: $(GitBlocks_cbplugin_RESOURCES)
	cd $(srcdir)
	zip -j9 GitBlocks.cbplugin .libs/GitBlocks.so GitBlocks.zip

