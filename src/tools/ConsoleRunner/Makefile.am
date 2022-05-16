bin_PROGRAMS = cb_console_runner

if CODEBLOCKS_NT
.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

AM_CPPFLAGS = -D__WXMSW__
endif

cb_console_runner_SOURCES = main.cpp

if !CODEBLOCKS_NT_BUILD_INSTALLER
man_MANS = cb_console_runner.1
endif

EXTRA_DIST = $(man_MANS)
