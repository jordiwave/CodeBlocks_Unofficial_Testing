bin_PROGRAMS = cbp2make

AM_CPPFLAGS = \
			-Wall \
			-Ilib \
			-Ilib/tinyxml \
			-Isrc

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-pipe \
			-mthreads \
			-D__GNUWIN32__ \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64
endif

cbp2make_SOURCES = \
			src/cbp2make.cpp \
			src/cbproject.cpp \
			src/cbptarget.cpp \
			src/cbpunit.cpp \
			src/cbworkspace.cpp \
			src/depsearch.cpp \
			src/cbhelper.cpp \
			src/makefile.cpp \
			src/platforms.cpp \
			src/toolchains.cpp \
			lib/stlconfig.cpp \
			lib/stlconvert.cpp \
			lib/stlfutils.cpp \
			lib/stlgpm.cpp \
			lib/stlstrings.cpp \
			lib/stlvariables.cpp \
			lib/stringhash.cpp \
			lib/tinyxml/tinystr.cpp \
			lib/tinyxml/tinyxml.cpp \
			lib/tinyxml/tinyxmlerror.cpp \
			lib/tinyxml/tinyxmlparser.cpp \
			src/buildtools.cpp \
			src/cbbuildcfg.cpp \
			src/cbbuildmgr.cpp \
			src/cbglobalvar.cpp
