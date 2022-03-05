if BUILD_AUTOVERSIONING
MAYBE_AUTOVERSIONING=AutoVersioning
MAYBE_APPDATA=appdata
endif

if BUILD_BROWSETRACKER
MAYBE_BROWSETRACKER=BrowseTracker
MAYBE_APPDATA=appdata
endif

if BUILD_BYOGAMES
MAYBE_BYOGAMES=byogames
MAYBE_APPDATA=appdata
endif

if BUILD_CBKODERS
MAYBE_CBKODERS=cb_koders
MAYBE_APPDATA=appdata
endif

if BUILD_CODESNIPPETS
MAYBE_CODESNIPPETS=codesnippets
MAYBE_APPDATA=appdata
endif

if BUILD_CODESTAT
MAYBE_CODESTAT=codestat
MAYBE_APPDATA=appdata
endif

if BUILD_COPYSTRINGS
MAYBE_COPYSTRINGS=copystrings
MAYBE_APPDATA=appdata
endif

if BUILD_CSCOPE
MAYBE_CSCOPE=Cscope
MAYBE_APPDATA=appdata
endif

if BUILD_DOXYBLOCKS
MAYBE_DOXYBLOCKS=DoxyBlocks
MAYBE_APPDATA=appdata
endif

if BUILD_DRAGSCROLL
MAYBE_DRAGSCROLL=dragscroll
MAYBE_APPDATA=appdata
endif

if BUILD_EDITORCONFIG
MAYBE_EDITORCONFIG=EditorConfig
MAYBE_APPDATA=appdata
endif

if BUILD_EDITORTWEAKS
MAYBE_EDITORTWEAKS=EditorTweaks
MAYBE_APPDATA=appdata
endif

if BUILD_ENVVARS
MAYBE_ENVVARS=envvars
MAYBE_APPDATA=appdata
endif

if BUILD_FILEMANAGER
MAYBE_FILEMANAGER=FileManager
MAYBE_APPDATA=appdata
endif

if BUILD_HELP
MAYBE_HELP=help_plugin
MAYBE_APPDATA=appdata
endif

if BUILD_HEADERFIXUP
MAYBE_HEADERFIXUP=headerfixup
MAYBE_APPDATA=appdata
endif

if BUILD_KEYBINDER
MAYBE_KEYBINDER=keybinder
MAYBE_APPDATA=appdata
endif

if BUILD_LIBFINDER
MAYBE_LIBFINDER=wxContribItems lib_finder
MAYBE_APPDATA=appdata
endif

if BUILD_PROFILER
MAYBE_PROFILER=profiler
MAYBE_APPDATA=appdata
endif

if BUILD_PROJECTOPTIONSMANIPULATOR
MAYBE_PROJECTOPTIONSMANIPULATOR=ProjectOptionsManipulator
MAYBE_APPDATA=appdata
endif

if BUILD_REGEX
MAYBE_REGEX=regex_testbed
MAYBE_APPDATA=appdata
endif

if BUILD_REOPENEDITOR
MAYBE_REOPENEDITOR=ReopenEditor
MAYBE_APPDATA=appdata
endif

if BUILD_RNDGEN
MAYBE_RNDGEN=rndgen
MAYBE_APPDATA=appdata
endif

if BUILD_EXPORTER
MAYBE_EXPORTER=source_exporter
MAYBE_APPDATA=appdata
endif

if BUILD_SMARTINDENT
MAYBE_SMARTINDENT=SmartIndent
MAYBE_APPDATA=appdata
endif

if BUILD_SPELLCHECKER
MAYBE_SPELLCHECKER=SpellChecker
MAYBE_APPDATA=appdata
endif

if BUILD_SYMTAB
MAYBE_SYMTAB=symtab
MAYBE_APPDATA=appdata
endif

if BUILD_THREADSEARCH
MAYBE_THREADSEARCH=wxContribItems ThreadSearch
MAYBE_APPDATA=appdata
endif

if BUILD_TOOLSPLUS
MAYBE_TOOLSPLUS=ToolsPlus
MAYBE_APPDATA=appdata
endif

if ! CODEBLOCKS_NT
# Valgrind is not applicable for Windows at the moment
if BUILD_VALGRIND
MAYBE_VALGRIND=Valgrind
MAYBE_APPDATA=appdata
endif
endif

if BUILD_WXCONTRIB
MAYBE_WXCONTRIB=wxContribItems
MAYBE_APPDATA=appdata
endif

if BUILD_WXSMITH
MAYBE_WXSMITH=wxSmith
MAYBE_APPDATA=appdata
endif

if BUILD_WXSMITHCONTRIB
MAYBE_WXSMITHCONTRIB=wxSmith wxContribItems wxSmithContribItems
MAYBE_APPDATA=appdata
endif

if BUILD_WXSMITHAUI
MAYBE_WXSMITHAUI=wxSmith wxSmithAui
MAYBE_APPDATA=appdata
endif

if BUILD_HEXEDITOR
MAYBE_HEXEDITOR=HexEditor
MAYBE_APPDATA=appdata
endif

if BUILD_INCSEARCH
MAYBE_INCSEARCH=IncrementalSearch
MAYBE_APPDATA=appdata
endif

if BUILD_MOUSESAP
MAYBE_MOUSESAP=MouseSap
MAYBE_APPDATA=appdata
endif

if BUILD_NASSISHNEIDERMAN
MAYBE_NASSISHNEIDERMAN=NassiShneiderman
MAYBE_APPDATA=appdata
endif

if BUILD_CCCC
MAYBE_CCCC=Cccc
MAYBE_APPDATA=appdata
endif

if BUILD_CPPCHECK
MAYBE_CPPCHECK=CppCheck
MAYBE_APPDATA=appdata
endif

if ENABLE_FORTRAN
MAYBE_FORTRAN=FortranProject
MAYBE_APPDATA=appdata
endif

if CODEBLOCKS_NT
MAYBE_DEVPAK_PLUGIN=devpak_plugin
MAYBE_APPDATA=appdata
endif

SUBDIRS = \
			$(MAYBE_AUTOVERSIONING) \
			$(MAYBE_BROWSETRACKER) \
			$(MAYBE_BYOGAMES) \
			$(MAYBE_CBKODERS) \
			$(MAYBE_CCCC) \
			$(MAYBE_CODESNIPPETS) \
			$(MAYBE_CODESTAT) \
			$(MAYBE_COPYSTRINGS) \
			$(MAYBE_CPPCHECK) \
			$(MAYBE_CSCOPE) \
			$(MAYBE_DOXYBLOCKS) \
			$(MAYBE_DRAGSCROLL) \
			$(MAYBE_EDITORCONFIG) \
			$(MAYBE_EDITORTWEAKS) \
			$(MAYBE_ENVVARS) \
			$(MAYBE_FILEMANAGER) \
			$(MAYBE_HEADERFIXUP) \
			$(MAYBE_HELP) \
			$(MAYBE_HEXEDITOR) \
			$(MAYBE_INCSEARCH) \
			$(MAYBE_KEYBINDER) \
			$(MAYBE_LIBFINDER) \
			$(MAYBE_MOUSESAP) \
			$(MAYBE_NASSISHNEIDERMAN) \
			$(MAYBE_PROFILER) \
			$(MAYBE_PROJECTOPTIONSMANIPULATOR) \
			$(MAYBE_EXPORTER) \
			$(MAYBE_SYMTAB) \
			$(MAYBE_SMARTINDENT) \
			$(MAYBE_SPELLCHECKER) \
			$(MAYBE_REGEX) \
			$(MAYBE_REOPENEDITOR) \
			$(MAYBE_RNDGEN) \
			$(MAYBE_THREADSEARCH) \
			$(MAYBE_TOOLSPLUS) \
			$(MAYBE_VALGRIND) \
			$(MAYBE_WXCONTRIB) \
			$(MAYBE_WXSMITH) \
			$(MAYBE_WXSMITHCONTRIB) \
			$(MAYBE_WXSMITHAUI) \
			$(MAYBE_FORTRAN) \
			$(MAYBE_DEVPAK_PLUGIN) \
			$(MAYBE_APPDATA)

DIST_SUBDIRS = $(SUBDIRS)

OLD_DIST_SUBDIRS = \
			appdata \
			AutoVersioning \
			BrowseTracker \
			byogames \
			cb_koders \
			Cccc \
			codesnippets \
			codestat \
			copystrings \
			CppCheck \
			Cscope \
			devpak_plugin \
			DoxyBlocks \
			dragscroll \
			EditorConfig \
			EditorTweaks \
			envvars \
			FileManager \
			headerfixup \
			help_plugin \
			HexEditor \
			keybinder \
			IncrementalSearch \
			lib_finder \
			MouseSap \
			NassiShneiderman \
			profiler \
			ProjectOptionsManipulator \
			source_exporter \
			SmartIndent \
			SpellChecker \
			symtab \
			regex_testbed \
			ReopenEditor \
			rndgen \
			ThreadSearch \
			ToolsPlus \
			Valgrind \
			wxContribItems \
			wxSmith \
			wxSmithContribItems \
			wxSmithAui \
			wxSmithDemo \
			wxSmithIExplore \
			wxSmithSTC
