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
			SmartIndentHDL.cpp

if CODEBLOCKS_NT
libSmartIndentHDL_la_SOURCES += \
			manifest_hdl.rc
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
			SmartIndentPascal.cpp
if CODEBLOCKS_NT
libSmartIndentPascal_la_SOURCES += \
			manifest_pascal.rc
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
			SmartIndentCpp.cpp
if CODEBLOCKS_NT
libSmartIndentCpp_la_SOURCES += \
			manifest_cpp.rc
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
			SmartIndentPython.cpp
if CODEBLOCKS_NT
libSmartIndentPython_la_SOURCES += \
			manifest_python.rc
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
			SmartIndentLua.cpp
if CODEBLOCKS_NT
libSmartIndentLua_la_SOURCES += \
			manifest_lua.rc
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
			SmartIndentFortran.cpp
if CODEBLOCKS_NT
libSmartIndentFortran_la_SOURCES += \
			manifest_fortran.rc
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
			SmartIndentXML.cpp
if CODEBLOCKS_NT
libSmartIndentXML_la_SOURCES += \
			manifest_xml.rc
endif

noinst_HEADERS = \
			SmartIndentHDL.h \
			SmartIndentPascal.h \
			SmartIndentCpp.h \
			SmartIndentPython.h \
			SmartIndentLua.h \
			SmartIndentFortran.h \
			SmartIndentXML.h

EXTRA_DIST = \
			*.cbp \
			manifest_hdl.xml \
			manifest_pascal.xml \
			manifest_cpp.xml \
			manifest_python.xml \
			manifest_lua.xml \
			manifest_fortran.xml \
			manifest_xml.xml

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

SmartIndentHDL.zip: manifest_hdl.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_hdl.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentHDL.zip manifest.xml

SmartIndentPascal.zip: manifest_pascal.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_pascal.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentPascal.zip manifest.xml

SmartIndentCpp.zip: manifest_cpp.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_cpp.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentCpp.zip manifest.xml

SmartIndentPython.zip: manifest_python.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_python.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentPython.zip manifest.xml

SmartIndentLua.zip: manifest_lua.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_lua.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentLua.zip manifest.xml

SmartIndentFortran.zip: manifest_fortran.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_fortran.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentFortran.zip manifest.xml

SmartIndentXML.zip: manifest_xml.xml
	PWD=`pwd` cd $(srcdir) && \
	cp manifest_xml.xml ./manifest.xml && \
	zip -Rm $(PWD)/SmartIndentXML.zip manifest.xml

