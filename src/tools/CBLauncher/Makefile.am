if CODEBLOCKS_NT
bin_PROGRAMS = CbLauncher

AM_CPPFLAGS = \
			-Os \
			-pedantic \
			-Wextra \
			-Wall \
			-fexceptions \
			-D_UNICODE \
			-DUNICODE \
			-DWINVER=0x0500 \
			-D_WIN64

CbLauncher_LDADD = \
			-lgdi32 \
			-loleaut32

CbLauncher_SOURCES = launcher.cpp

endif

EXTRA_DIST = \
			$(srcdir)/doc/* \
			$(srcdir)/icons/* \
			$(srcdir)/*.cbp \
			icon.rc \
			mainpage.h \
			launcher.cpp
