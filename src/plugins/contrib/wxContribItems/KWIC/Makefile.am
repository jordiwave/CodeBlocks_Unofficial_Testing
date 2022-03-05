SUBDIRS = include/wx/KWIC include/wx/xrc

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(srcdir)/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
wxcontribitemsdir = $(libdir)
else
wxcontribitemsdir = $(libdir)/codeblocks/wxContribItems
endif

wxcontribitems_LTLIBRARIES = \
			libwxkwic.la

libwxkwic_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxkwic_la_LDFLAGS += \
			-Wl,--export-all-symbols \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libwxkwic_la_LIBADD = \
			$(WX_LIBS)

libwxkwic_la_SOURCES = \
			./src/wx/KWIC/AngularMeter.cpp \
			./src/wx/KWIC/AngularRegulator.cpp \
			./src/wx/KWIC/BmpCheckBox.cpp \
			./src/wx/KWIC/BmpSwitcher.cpp \
			./src/wx/KWIC/LCDClock.cpp \
			./src/wx/KWIC/LCDWindow.cpp \
			./src/wx/KWIC/LinearMeter.cpp \
			./src/wx/KWIC/LinearRegulator.cpp \
			./src/wx/KWIC/TimeAlarm.cpp \
			./src/wx/xrc/xh_kwxangularmeterhandler.cpp \
			./src/wx/xrc/xh_kwxangularregulatorhandler.cpp \
			./src/wx/xrc/xh_kwxlcdclockhandler.cpp \
			./src/wx/xrc/xh_kwxlcddisplayhandler.cpp \
			./src/wx/xrc/xh_kwxlinearmeterhandler.cpp \
			./src/wx/xrc/xh_kwxlinearregulatorhandler.cpp

EXTRA_DIST = \
			CREDITS \
			./KWIC-LICENSE
