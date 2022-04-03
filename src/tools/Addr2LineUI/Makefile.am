# only used for CODEBLOCKS_NT
bin_PROGRAMS = Addr2LineUI

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-Wall \
			-pipe \
			-mthreads \
			-D__GNUWIN32__ \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64

Addr2LineUI_LDADD = \
			$(WX_LIBS)

Addr2LineUI_SOURCES = \
		Addr2LineUIApp.cpp \
		Addr2LineUIApp.h \
		Addr2LineUIMain.cpp \
		Addr2LineUIMain.h \
		resource.rc
