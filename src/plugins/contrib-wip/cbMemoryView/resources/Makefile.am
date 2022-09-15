pkgdata_DATA = \
			cbMemoryView.zip

CLEANFILES = \
			$(pkgdata_DATA)

resources_forZIP = \
			manifest.rc \
			manifest.xml \
			MemoryPanel.xrc \
            images/detailed_view_1.png \
            images/general_view_1.png \
            images/menu_1.png

EXTRA_DIST = \
			$(resources_forZIP)

cbMemoryView.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip $(PWD)/cbMemoryView.zip $(resources_forZIP) > /dev/null
