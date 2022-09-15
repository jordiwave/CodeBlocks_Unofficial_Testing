pkgdata_DATA = \
			cbSystemView.zip

CLEANFILES = \
			$(pkgdata_DATA)


resources_forZIP = \
            manifest.rc \
            manifest.xml \
            ProjectConfig.xrc \
            images/collapse_16x16.png \
            images/expand_16x16.png \
            images/ok_16x16.png \
            images/throbber.gif \
            images/throbber_16x16.gif

EXTRA_DIST = \
			$(resources_forZIP)

cbSystemView.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/cbSystemView.zip $(resources_forZIP) > /dev/null
