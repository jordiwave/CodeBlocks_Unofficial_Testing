SUBDIRS = include base build_tools mime sdk src plugins tools templates scripts wxsmith

dist_pkgdata_DATA = \
			tips.txt

EXTRA_DIST = \
			*.cbp \
			*.workspace \
			mac_pack \
			update* \
			batch* \
			sdk.doxy

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DDEPSLIB_WINDOWS
endif
