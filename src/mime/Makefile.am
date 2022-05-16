# Do not use this file if building WIndows insallter directory tree

if !CODEBLOCKS_NT_BUILD_INSTALLER

desktopdir = $(datadir)/applications
desktop_DATA = \
			codeblocks.desktop

xdgmimedir = $(datadir)/mime/packages
xdgmime_DATA = \
			codeblocks.xml

mimeicondir = $(datadir)/icons/hicolor/48x48/mimetypes
mimeicon_DATA = \
			application-x-codeblocks.png \
			application-x-codeblocks-workspace.png

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = \
			codeblocks.png

EXTRA_DIST = \
			$(desktop_DATA) \
			$(xdgmime_DATA) \
			$(mimeicon_DATA) \
			$(pixmap_DATA)

## Update mime only when system wide installation takes place

install-data-hook: install-xdgmimeDATA
	if [ -f $(DESTDIR)$(datadir)/mime/packages/freedesktop.org.xml ] ; then \
	 if which update-mime-database>/dev/null 2>&1; then \
			update-mime-database $(DESTDIR)$(datadir)/mime; \
	 fi; \
	 if which update-desktop-database>/dev/null 2>&1; then \
			update-desktop-database; \
	 fi; \
	 if which gtk-update-icon-cache>/dev/null 2>&1; then \
			gtk-update-icon-cache --force $(DESTDIR)$(datadir)/icons/hicolor; \
	 fi \
	fi

uninstall-hook:
	if [ -f $(DESTDIR)$(datadir)/mime/packages/freedesktop.org.xml ] ; then \
	 if which update-mime-database>/dev/null 2>&1; then \
			update-mime-database $(DESTDIR)$(datadir)/mime; \
	 fi; \
	 if which update-desktop-database>/dev/null 2>&1; then \
			update-desktop-database; \
	 fi; \
	 if which gtk-update-icon-cache>/dev/null 2>&1; then \
			gtk-update-icon-cache --force $(DESTDIR)$(datadir)/icons/hicolor; \
	 fi \
	fi

endif