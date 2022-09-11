# Unofficial GIT Code::Blocks repo that contains the following:

## Changes not included in the official C::B source

1. Only detected compilers are shown by default. Option to show all compiler is available.
2. Auto-detect/auto add GDB support.
3. Additional Help->"System Information" to show allot of extra info and ability to copy the data to clipboard button that anonymizes the data.
4. Add ability to copy debug watch variable content to clipboard.
5. Added Pecan's experimental CB-clangd_client. See https://sourceforge.net/projects/cb-clangd-client
6. Removed the code-completion plugin as the clangd_client replaces it and works allot better.
8. Includes debugger_DAP plugin for MAC developers.
9. Includes NSIS installer scripts and updated scripts. Major script changes. 
10. Additional plugins (not fully tested):
        * cbBuildTools
        * cbDiff
        * cbInno
        * cbMarkdown
        * cbMemoryView
        * cbNSIS
        * cbSystemView
        * cbtortoisesvn
        * GDB/MI debugger
        * GitBlocks
11. Many other minor/medium changes to help developers out.
12. Experimental additional plugins that compile and load, but need testing and debugging:
        * Python Debugger
        * Python Interpreter
        * Python Code Completion
        * Python Code Check

If you know or find or see a Code::Blocks plugin that is useful and it is not included in this repo's source please riase an issue and I will have a look to see if it can be included without allot of work.