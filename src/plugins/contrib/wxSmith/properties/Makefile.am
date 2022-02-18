AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-D__WXMSW__ \
			-DBUILDING_PLUGIN \
			-DHAVE_W32API_H \
			-pipe \
			-fmessage-length=0 \
			-fexceptions
endif

noinst_LTLIBRARIES = \
			libwxsmith_properties.la

libwxsmith_properties_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libwxsmith_properties_la_LDFLAGS += \
			-enable-auto-image-base \
			-Wl,--export-all-symbols \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
#			-lgdi32
else
libwxsmith_properties_la_LDFLAGS += \
			-version-info 0:1:0
endif

libwxsmith_properties_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la

libwxsmith_properties_la_SOURCES = \
			./wxsarraystringeditordlg.cpp \
			./wxsboolproperty.cpp \
			./wxspropertystream.cpp \
			./wxsflagsproperty.cpp \
			./wxsfloatproperty.cpp \
			./wxsemptyproperty.cpp \
			./wxsstringproperty.cpp \
			./wxslongproperty.cpp \
			./wxsenumproperty.cpp \
			./wxsarraystringproperty.cpp \
			./wxsquickpropspanel.cpp \
			./wxsproperty.cpp \
			./wxspropertygridmanager.cpp \
			./wxscustomeditorproperty.cpp \
			./wxspropertycontainer.cpp \
			./wxstwolongproperty.cpp \
			./wxseditenumproperty.cpp 

sdkdir = $(includedir)/wxsmith/properties

if !CODEBLOCKS_NT_BUILD_INSTALLER
sdk_HEADERS = \
			./wxspropertycontainer.h \
			./wxsproperties.h \
			./wxsboolproperty.h \
			./wxspropertystream.h \
			./wxsflagsproperty.h \
			./wxsfloatproperty.h \
			./wxsemptyproperty.h \
			./wxslongproperty.h \
			./wxsarraystringeditordlg.h \
			./wxsenumproperty.h \
			./wxsarraystringproperty.h \
			./wxsstringproperty.h \
			./wxsquickpropspanel.h \
			./wxsproperty.h \
			./wxspropertygridmanager.h \
			./wxscustomeditorproperty.h \
			./wxstwolongproperty.h \
			./wxseditenumproperty.h
endif