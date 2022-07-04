# CODE::BLOCKS  DAP DEBUGGER PLUGIN

## Description

This GitHub repo contains the source code for a Code::Blocks DAP debugger plugin. 
The debugger is a work in progress and the source code may be ahead of this readme.md doc. 
You can use it now to do simple debugging and testing on a **MacOS** & Windows & Linux.


## Updates / Testing Results / Dev Work

If you need help with using the plugin then wait for it to be production ready.
Please use the discussion for any test results that pass and are shown as working in the table.
Please create a new issue (if it does not already exist) for any test in the table below that fail and are shown as working in the table.
If you fix any outstanding issue please fork this repo and create a pull request.


## OUTSTANDING ITEMS

### High Priority

1. Update ticket 1114 to add support for the CLANG compiler to detect debugger and add it as a debugger automatically. 
2. Use in anger to fix C::B issue(s) on MacOS. In other words use it in the real world.
3. Add missing features in the table below to make debugging better in the real world environment.

### Medium Priority

1. Add missing features in the table below.
3. When time permits re-test all items!!!! 

### Low Priority / Future Work

1. Update table below for outstanding issues/items
3. Create a C::B SF ticket that has a link github repo with source in it for someone else to look at and merge.

## Testing/Coding/Feature Check List

|                   Item                                      |   Date    |   Result   |
|-------------------------------------------------------------|-----------|------------|
| **Stepping**                                                |           |            |
|   * Start/Continue       (F8)                               |  26-Jun   |   Pass     |
|   * Break debugger                                          |           |            |
|   * Stop debugger        (Shift-F8)                         |  26-Jun   |   Pass     |
|   * Run to cursor        (F4)                               |           |            |
|   * Next line            (F7)                               |  26-Jun   |   Pass     |
|   * Step Into            (Shift-F7)                         |           |            |
|   * Step out             (Ctrl-F7)                          |           |            |
|      * Display the return value of a function               |           |            |
|   * Next instruction     (Alt-F7)                           |           |            |
|   * Step into instruction(ALT-Shift-F7)                     |           |            |
|   * Set next statement                                      |           |            |
|   * Notification that the debugging has ended               |           |            |
|   * Skipping over functions                                 |           |            |
|   * Skipping over files                                     |           |            |
|                                                             |           |            |
| **Watches**                                                 |           |            |
|   * watches dialog shows function args and local vars       |           |            |
|   * Add watch before starting the debugger                  |           |            |
|   * Add watch after starting the debugger                   |           |            |
|   * Simple data types                                       |           |            |
|   * Simple structure                                        |           |            |
|   * Array of simple structures                              |           |            |
|   * Complex structures                                      |           |            |
|   * Can expand cbProject complex structures                 |           |            |
|   * Edit watches                                            |           |            |
|   * Watches data saved on project close                     |           |            |
|   * Watches removed after closing the project               |           |            |
|   * Watches removed after changing debugger                 |           |            |
|   * Watches created on project open                         |           |            |
|                                                             |           |            |
| **Breakpoints**                                             |           |            |
|   * Add line break point before starting the debugger       |  27-Jun   |    Pass    |
|   * Add line break point after the starting the debugger    |  27-Jun   |    Pass    |
|   * Debug menu option to Toggle line break point (F5)       |           |            |
|   * Debug menu option to Remove all breakpoints             |           |            |
|   * Disable/Enable line break point via pop up menu         |           |            |
|   * Remove line break point                                 |           |            |
|   * Edit line break point                                   |           |            |
|     * ignore count before break                             |           |            |
|     * break when expression is true                         |           |            |
|   * Break points still there after GDB exit                 |           |            |
|   * Break points data saved on project close                |  27-Jun   |    Pass    |
|   * Break points removed after closing the project          |           |            |
|   * Break points removed after changing debugger            |           |            |
|   * Break points created on project open                    |  27-Jun   |    Pass    |
|                                                             |           |            |
| **Data Breakpoints**                                        |           |            |
|   * Display data breakpoints in break point dialog          |           |            |
|   * Add data break point (right click pop up menu on var)   |           |            |
|   * Add support for deleting already added data break point |           |            |
|   * Add remove data breakpoints to Remove all breakpoints   |           |            |
|   * Add support for multiple data break points              |           |            |
|   * Data break points data is NOT saved on project close    |           |            |
|                                                             |           |            |
| **Debug show Running Threads**                              |           |            |
|   * Show running threads dialog                             |           |            |
|   * Close running threads dialog                            |           |            |
|   * Updates on step/run                                     |           |            |
|   * Thread dialog cleared on app exit                       |           |            |
|                                                             |           |            |
| **Debug show CPU Registers**                                |           |            |
|   * Show CPU register dialog                                |           |            |
|   * Close CPU register dialog                               |           |            |
|   * Update on step                                          |           |            |
|   * CPU register dialog cleared on GDB exit                 |           |            |
|                                                             |           |            |
| **Debug show Call Stack**                                   |           |            |
|   * Show call stack dialog                                  |           |            |
|   * Close call stack dialog                                 |           |            |
|   * Double click on entry should open and go to the line    |           |            |
|   * Call stack dialog cleared on last project close         |           |            |
|                                                             |           |            |
| **Debug show Disassembly**                                  |           |            |
|   * Show disassembly dialog                                 |           |            |
|   * Close disassembly dialog                                |           |            |
|   * Assembly only                                           |           |            |
|   * Mixed mode                                              |           |            |
|   * Ability to swap between assembly and mixed modes        |           |            |
|   * Adjust button feature                                   |           |            |
|   * Save disassembly to a file                              |           |            |
|   * Disassembly dialog cleared on GDB exit                  |           |            |
|                                                             |           |            |
| **Debug -> Memory Dump Dialog**                             |           |            |
|   * Show memory dump dialog                                 |           |            |
|   * Close memory dump dialog                                |           |            |
|   * Show variable memory from watch dialog                  |           |            |
|   * Memory dump dialog not cleared on GDB exit              |           |            |
|   * Memory dump dialog cleared on last project close        |           |            |
|   * Memory dump watches data saved on project close         |           |            |
|   * Memory dump watches removed after changing debugger     |           |            |
|   * Memory dump watches created on project open             |           |            |
|                                                             |           |            |
| **Debug -> Memory view Dialog**                             |           |            |
|   * Show memory view dialog                                 |           |            |
|   * Close memory view dialog                                |           |            |
|   * Show memory information                                 |           |            |
|   * Show variable memory from watch dialog                  |           |            |
|   * Memory view dialog cleared on last project close        |           |            |
|   * Memory view watches data saved on project close         |           |            |
|   * Memory view watches removed after closing the project   |           |            |
|   * Memory view watches removed after changing debugger     |           |            |
|   * Memory view watches created on project open             |           |            |
|                                                             |           |            |
| **Show tty for console projects**                           |           |            |
|   * Show console on console app                             |  26-Jun   |   Pass     |
|   * Do NOT show console on GUI app                          |           |            |
|                                                             |           |            |
| **Projects - debugger options dialog**                      |           |            |
|   * Show debuggeroptionsprjdlg dialog                       |           |            |
|   * Close debuggeroptionsprjdlg dialog                      |           |            |
|   * Data saved /loaded                                      |           |            |
|   * Data used by GDB/MI                                     |           |            |
|                                                             |           |            |
| **GDB/MI Debugger configuration dialog**                    |           |            |
|   * Show debugger options dialog                            |           |            |
|   * Close debugger options dialog                           |           |            |
|   * Executable path save/loaded/used                        |           |            |
|   * Arguments save/loaded/used                              |           |            |
|   * Arguments used in starting debugger                     |           |            |
|   * Debugger init commands save/loaded/used                 |           |            |
|   * Disable startup scripts checkbox save/loaded/used       |           |            |
|   * Watch function arguments checkbox save/loaded/used      |           |            |
|   * Watch local variables checkbox save/loaded/used         |           |            |
|   * Catch C++ exceptions checkbox save/loaded/used          |           |            |
|   * Evaluate expression under cursor cbox save/loaded/used  |           |            |
|   * Add other projects paths... checkbox save/loaded/used   |           |            |
|   * Do not run the debugee checkbox save/loaded/used        |           |            |
|   * Use python pretty printer checkbox save/loaded/used     |           |            |
|   * Disassembly flavor drop down list save/loaded/used      |           |            |
|   * Persists checkbox save/load breakpoint/watch etc        |           |            |
|                                                             |           |            |
| **Checkpoints on Linux**                                    |           |            |
|   * Add checkpoint                                          |           |            |
|   * Delete checkpoint                                       |           |            |
|   * Project checkpoint data saved on project close          |           |            |
|   * Project checkpoint removed after closing the project    |           |            |
|   * Project checkpoint removed after changing debugger      |           |            |
|                                                             |           |            |
| **Pretty Printing**                                         |           |            |
|   * Ensure pretty printing works                            |           |            |
|   * Document how pretty printing works/is configured        |           |            |
|                                                             |           |            |
| **Remote Debugging**                                        |           |            |
|   * Use data from debuggeroptionsprjdlg dialog              |           |            |
|   * Ensure serial (RS232) debugging works                   |           |            |
|   * Ensure TCP debugging works                              |           |            |
|                                                             |           |            |
| **Operaiting System**                                       |           |            |
|   * Works on Windows                                        |  26-Jun   |   Pass     |
|   * Works on Linux                                          |           |            |
|   * Works on MacOS                                          |           |            |
|   * Builds on Windows via workspace                         |  26-Jun   |   Pass     |
|   * Builds on Windows via MSYS2 makefile                    |           |            |
|   * Builds on Linux via workspace                           |           |            |
|   * Builds on Linux via makefile                            |           |            |
|   * Builds on MacOS via workspace                           |           |            |
|   * Builds on MacOS via makefile                            |           |            |
|   * Create and test DAP cbplugin for Windows                |           |            |
|   * Create and test DAP cbplugin for Linux                  |           |            |
|   * Create and test DAP cbplugin for MacOS                  |           |            |
|                                                             |           |            |

The table above was last updated on 26-Jun-2022.

NOTES:
 "PASS"     - initial testing showed the item worked as expected, but changes since may have broken it... Please raise an issue if you find it broken.
 "WIP"      - working on this item
 "*Broken*" - during testing the feature/item did not work or work as expected
 "* NEW *"  - new feature that is not (or does not work) in the existing GDB debugger
 "To test"  - not tested or not tested as it relies on other items and as such it is not worth testing now.
 "Future"   - new functionality that another developer will need help with or do if time does not permit
