ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po src man tests

if !CODEBLOCKS_NT_BUILD_INSTALLER
pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = \
			hunspell.pc
endif

EXTRA_DIST = \
			license.myspell license.hunspell \
			ChangeLog.O COPYING.LESSER hunspell.pc.in
