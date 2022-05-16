if CODEBLOCKS_NT
bin_PROGRAMS = CbLauncher

.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

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
			doc/* \
			icons/* \
			*.cbp \
			icon.rc \
			mainpage.h \
			launcher.cpp
