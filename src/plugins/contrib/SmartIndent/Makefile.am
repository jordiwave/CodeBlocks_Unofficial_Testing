.rc.lo:
	$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@


AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libSmartIndentHDL.la \
			libSmartIndentPascal.la \
			libSmartIndentCpp.la \
			libSmartIndentPython.la \
			libSmartIndentLua.la \
			libSmartIndentFortran.la \
			libSmartIndentXML.la

libSmartIndentHDL_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

libSmartIndentHDL_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentHDL_la_SOURCES = \
			SRC_HDL/SmartIndentHDL.cpp

if CODEBLOCKS_NT
libSmartIndentHDL_la_SOURCES += \
			SRC_HDL/manifest.rc
endif

libSmartIndentPascal_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentPascal_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentPascal_la_SOURCES = \
			SRC_Pascal/SmartIndentPascal.cpp
if CODEBLOCKS_NT
libSmartIndentPascal_la_SOURCES += \
			SRC_Pascal/manifest.rc
endif

libSmartIndentCpp_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentCpp_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentCpp_la_SOURCES = \
			SRC_CPP/SmartIndentCpp.cpp
if CODEBLOCKS_NT
libSmartIndentCpp_la_SOURCES += \
			SRC_CPP/manifest.rc
endif

libSmartIndentPython_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentPython_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentPython_la_SOURCES = \
			SRC_Python/SmartIndentPython.cpp
if CODEBLOCKS_NT
libSmartIndentPython_la_SOURCES += \
			SRC_Python/manifest.rc
endif

libSmartIndentLua_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentLua_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentLua_la_SOURCES = \
			SRC_LUA/SmartIndentLua.cpp
if CODEBLOCKS_NT
libSmartIndentLua_la_SOURCES += \
			SRC_LUA/manifestrc
endif

libSmartIndentFortran_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentFortran_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentFortran_la_SOURCES = \
			SRC_Fortran/SmartIndentFortran.cpp
if CODEBLOCKS_NT
libSmartIndentFortran_la_SOURCES += \
			SRC_Fortran/manifest.rc
endif

libSmartIndentXML_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-version-info 0:1:0 \
			-no-undefined \
			-avoid-version

libSmartIndentXML_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

libSmartIndentXML_la_SOURCES = \
			SRC_XML/SmartIndentXML.cpp
if CODEBLOCKS_NT
libSmartIndentXML_la_SOURCES += \
			SRC_XML/manifest.rc
endif

noinst_HEADERS = \
			SRC_HDL/SmartIndentHDL.h \
			SRC_Pascal/SmartIndentPascal.h \
			SRC_CPP/SmartIndentCpp.h \
			SRC_Python/SmartIndentPython.h \
			SRC_LUA/SmartIndentLua.h \
			SRC_Fortran/SmartIndentFortran.h \
			SRC_XML/SmartIndentXML.h

EXTRA_DIST = \
			*.cbp \
			SRC_HDL/manifest.xml \
			SRC_Pascal/manifest.xml \
			SRC_CPP/manifest.xml \
			SRC_Python/manifest.xml \
			SRC_LUA/manifest.xml \
			SRC_Fortran/manifest.xml \
			SRC_XML/manifest.xml

pkgdata_DATA = \
			SmartIndentHDL.zip \
			SmartIndentPascal.zip \
			SmartIndentCpp.zip \
			SmartIndentPython.zip \
			SmartIndentLua.zip \
			SmartIndentFortran.zip \
			SmartIndentXML.zip

CLEANFILES = \
			$(pkgdata_DATA)

.NOTPARALLEL: $(pkgdata_DATA)

SmartIndentHDL.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentHDL.zip SRC_HDL/manifest.xml

SmartIndentPascal.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentPascal.zip SRC_Pascal/manifest.xml

SmartIndentCpp.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentCpp.zip SRC_CPP/manifest.xml

SmartIndentPython.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentPython.zip SRC_Python/manifest.xml

SmartIndentLua.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentLua.zip SRC_LUA/manifest.xml

SmartIndentFortran.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentFortran.zip SRC_Fortran/manifest.xml

SmartIndentXML.zip:
	PWD=`pwd` cd $(srcdir) && \
	zip -Rm $(PWD)/SmartIndentXML.zip SRC_XML/manifest.xml

