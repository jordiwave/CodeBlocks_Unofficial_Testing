# CODE::BLOCKS  DAP DEBUGGER PLUGIN

## Description

The C::B DAP debugger is able to do simple debugging on **MacOS** & Windows & Linux.


## Updates / Testing Results / Dev Work

The DAP debugger plugin does not include all of the features the existing GDB annotations debugger has, but it does work on the MacOS.

If you find issue that is listed in the table below that indicates it is passing then please raise an issue.


## OUTSTANDING ITEMS

### High Priority

1. Update table below for outstanding issues/items

### Medium Priority

1. Add missing features in the table below in order to make debugging better in the real world environment.

### Low Priority / Future Work

1. When time permits re-test all items!!!! 

## Testing/Coding/Feature Check List

|                   Debugging Feature                         | Date   |codelldb|  Linux | MacOS  |
|-------------------------------------------------------------|--------|--------|--------|--------|
| **Stepping**                                                |        |        |        |        |
|   * Start/Continue       (F8)                               | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Break debugger                                          |        | Future | Future | Future |
|   * Stop debugger        (Shift-F8)                         | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Run to cursor        (F4)                               |        | Future | Future | Future |
|   * Next line            (F7)                               | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Step Into            (Shift-F7)                         | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Step out             (Ctrl-F7)                          | 02-Aug |  Pass  |  TBA   |  TBA   |
|      * Display the return value of a function               |        | Future | Future | Future |
|   * Next instruction     (Alt-F7)                           |        | Future | Future | Future |
|   * Step into instruction(ALT-Shift-F7)                     |        | Future | Future | Future |
|   * Set next statement                                      |        | Future | Future | Future |
|   * Notification that the debugging has ended               |        | Future | Future | Future |
|   * Skipping over functions                                 |        | Future | Future | Future |
|   * Skipping over files                                     |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Breakpoints**                                             |        |        |        |        |
|   * Add line break point before starting the debugger       | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Add line break point after the starting the debugger    | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Debug menu option to Toggle line break point (F5)       | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Debug menu option to Remove all breakpoints             | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Disable/Enable line break point via pop up menu         | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Remove line break point                                 | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Edit line break point                                   |        | Future | Future | Future |
|     * ignore count before break                             |        | Future | Future | Future |
|     * break when expression is true                         |        | Future | Future | Future |
|   * Break points still there after exit                     |        | Future | Future | Future |
|   * Break points data saved on project close                |        | Future | Future | Future |
|   * Break points removed after closing the project          |        | Future | Future | Future |
|   * Break points removed after changing debugger            |        | Future | Future | Future |
|   * Break points created on project open                    |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Exception Handling**                                      |        |        |        |        |
|   * Linux Debugger catches exceptions and shows call stack  | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * MacOS Debugger catches exceptions and shows call stack  | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Windows Clang64 catches exceptions and shows call stack | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Windows MinGW64 catches exceptions and shows call stack | 02-Aug |  Pass  |  TBA   |  TBA   |
|                                                             |        |        |        |        |
| **Watches**                                                 |        |        |        |        |
|   * watches dialog shows function args and local vars       |        | Future | Future | Future |
|   * Add watch before starting the debugger                  | 03-Aug |  N/A   |  N/A   |  N/A   |
|   * Add watch after starting the debugger                   | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Simple data types                                       | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Simple structure                                        | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Array of simple structures                              | 03-Aug |  Fail  |  TBA   |  TBA   |
|   * Complex structures                                      |        | ?Fail? |  TBA   |  TBA   |
|   * Can expand cbProject complex structures                 |        | ?Fail? |  TBA   |  TBA   |
|   * Edit watches                                            |        | Future | Future | Future |
|   * Watches removed after closing the project               | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Watches removed after changing debugger                 | 03-Aug |  Fail  |  TBA   |  TBA   |
|   * Watches data saved on project close                     |        | Future | Future | Future |
|   * Watches created on project open                         |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Data Breakpoints**                                        |        |        |        |        ||
|   * Display data breakpoints in break point dialog          |        | Future | Future | Future |
|   * Add data break point (right click pop up menu on var)   |        | Future | Future | Future |
|   * Add support for deleting already added data break point |        | Future | Future | Future |
|   * Add remove data breakpoints to Remove all breakpoints   |        | Future | Future | Future |
|   * Add support for multiple data break points              |        | Future | Future | Future |
|   * Data break points data is NOT saved on project close    |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Debug show Running Threads**                              |        |        |        |        |
|   * Show running threads dialog                             |        | Future | Future | Future |
|   * Close running threads dialog                            |        | Future | Future | Future |
|   * Updates on step/run                                     |        | Future | Future | Future |
|   * Thread dialog cleared on app exit                       |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Debug show CPU Registers**                                |        |        |        |        |
|   * Show CPU register dialog                                |        | Future | Future | Future |
|   * Close CPU register dialog                               |        | Future | Future | Future |
|   * Update on step                                          |        | Future | Future | Future |
|   * CPU register dialog cleared on exit                     |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Debug show Call Stack**                                   |        |        |        |        |
|   * Show call stack dialog                                  | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Close call stack dialog                                 | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Double click on entry should open and go to the line    | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Call stack dialog cleared on stopping debugging         | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Call stack dialog cleared on last project close         | 03-Aug |  Pass  |  TBA   |  TBA   |
|                                                             |        |        |        |        |
| **Debug show Disassembly**                                  |        |        |        |        |
|   * Show disassembly dialog                                 |        | Future | Future | Future |
|   * Close disassembly dialog                                |        | Future | Future | Future |
|   * Assembly only                                           |        | Future | Future | Future |
|   * Mixed mode                                              |        | Future | Future | Future |
|   * Ability to swap between assembly and mixed modes        |        | Future | Future | Future |
|   * Adjust button feature                                   |        | Future | Future | Future |
|   * Save disassembly to a file                              |        | Future | Future | Future |
|   * Disassembly dialog cleared on exit                      |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Debug -> Memory Dump Dialog**                             |        |        |        |        |
|   * Show memory dump dialog                                 |        | Future | Future | Future |
|   * Close memory dump dialog                                |        | Future | Future | Future |
|   * Show variable memory from watch dialog                  |        | Future | Future | Future |
|   * Memory dump dialog not cleared on exit                  |        | Future | Future | Future |
|   * Memory dump dialog cleared on last project close        |        | Future | Future | Future |
|   * Memory dump watches data saved on project close         |        | Future | Future | Future |
|   * Memory dump watches removed after changing debugger     |        | Future | Future | Future |
|   * Memory dump watches created on project open             |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Debug -> Memory view Dialog**                             |        |        |        |        |
|   * Show memory view dialog                                 |        | Future | Future | Future |
|   * Close memory view dialog                                |        | Future | Future | Future |
|   * Show memory information                                 |        | Future | Future | Future |
|   * Show variable memory from watch dialog                  |        | Future | Future | Future |
|   * Memory view dialog cleared on last project close        |        | Future | Future | Future |
|   * Memory view watches data saved on project close         |        | Future | Future | Future |
|   * Memory view watches removed after closing the project   |        | Future | Future | Future |
|   * Memory view watches removed after changing debugger     |        | Future | Future | Future |
|   * Memory view watches created on project open             |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Show tty for console projects**                           |        |        |        |        |
|   * Show console on console app                             | 03-Aug |  Pass  |  TBA   |  TBA   |
|   * Do NOT show console on GUI app                          |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Projects - debugger options dialog**                      |        |        |        |        |
|   * Show debuggeroptionsprjdlg dialog                       |        | Future | Future | Future |
|   * Close debuggeroptionsprjdlg dialog                      |        | Future | Future | Future |
|   * Data saved /loaded                                      |        | Future | Future | Future |
|   * Data used by Plugin                                     |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **DAP Debugger configuration dialog**                       |        |        |        |        |
|   * Show debugger options dialog                            | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Close debugger options dialog                           | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Auto detected lldb-vscode exe                           | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Default port # configured                               | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Executable path save/loaded/used                        | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Arguments save/loaded/used                              | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Arguments used in starting debugger                     | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Disable startup scripts checkbox save/loaded/used       |        | Future | Future | Future |
|   * Watch function arguments checkbox save/loaded/used      |        | Future | Future | Future |
|   * Watch local variables checkbox save/loaded/used         |        | Future | Future | Future |
|   * Catch C++ exceptions checkbox save/loaded/used          | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Catch C++ throw checkbox save/loaded/used               | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Evaluate expression under cursor cbox save/loaded/used  |        | Future | Future | Future |
|   * Add other projects paths... checkbox save/loaded/used   |        | Future | Future | Future |
|   * Do not run the debugee checkbox save/loaded/used        |        | Future | Future | Future |
|   * Disassembly flavor drop down list save/loaded/used      |        | Future | Future | Future |
|   * Persists checkbox save/load breakpoint/watch etc        |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Remote Debugging**                                        |        |        |        |        |
|   * Use data from debuggeroptionsprjdlg dialog              |        | Future | Future | Future |
|   * Ensure serial (RS232) debugging works                   |        | Future | Future | Future |
|   * Ensure TCP debugging works                              |        | Future | Future | Future |
|                                                             |        |        |        |        |
| **Operaiting System**                                       |        |        |        |        |
|   * Works on Windows                                        | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Works on Linux                                          | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Works on MacOS                                          | 08-Jul |  Pass  |  TBA   |  TBA   |
|   * Builds on Windows via workspace                         | 02-Aug |  Pass  |  TBA   |  TBA   |
|   * Builds on Windows via MSYS2 bootstrap/configure/make    |        | Future | Future | Future |
|   * Builds on Linux via workspace                           | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Builds on Linux via bootstrap/configure/make            | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Builds on MacOS via workspace                           | 08-Jul |  Pass  |  TBA   |  TBA   |
|   * Builds on MacOS via bootstrap/configure/make            | 08-Jul |  Pass  |  TBA   |  TBA   |
|   * Create and test DAP cbplugin for Windows                | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Create and test DAP cbplugin for Linux                  | 06-Jul |  Pass  |  TBA   |  TBA   |
|   * Create and test DAP cbplugin for MacOS                  | 08-Jul |  Pass  |  TBA   |  TBA   |
|                                                             |        |        |        |        |

The table above was updated on 2-Aug-2022.

NOTES:
 "PASS"     - initial testing showed the item worked as expected, but changes since may have broken it... Please raise an issue if you find it broken.
 "WIP"      - working on this item
 "*Broken*" - during testing the feature/item did not work or work as expected
 "* NEW *"  - new feature that is not (or does not work) in the existing GDB debugger
 "To test"  - not tested or not tested as it relies on other items and as such it is not worth testing now.
 "Future"   - new functionality that another developer will need help with or do if time does not permit
