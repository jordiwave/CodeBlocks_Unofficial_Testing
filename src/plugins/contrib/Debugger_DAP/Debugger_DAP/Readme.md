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
| :---------------------------------------------------------- | :----: | :----: | :----: | :----: |
| **Stepping**                                                |        |        |        |        |
|   * Start/Continue       (F8)                               | 02-Aug |  Pass  |   ?    |   ?    |
|   * Break debugger                                          |        |   -    |   -    |   -    |
|   * Stop debugger        (Shift-F8)                         | 02-Aug |  Pass  |   ?    |   ?    |
|   * Run to cursor        (F4)                               |        |   -    |   -    |   -    |
|   * Next line            (F7)                               | 02-Aug |  Pass  |   ?    |   ?    |
|   * Step Into            (Shift-F7)                         | 02-Aug |  Pass  |   ?    |   ?    |
|   * Step out             (Ctrl-F7)                          | 02-Aug |  Pass  |   ?    |   ?    |
|      * Display the return value of a function               |        |   -    |   -    |   -    |
|   * Next instruction     (Alt-F7)                           |        |   -    |   -    |   -    |
|   * Step into instruction(ALT-Shift-F7)                     |        |   -    |   -    |   -    |
|   * Set next statement                                      |        |   -    |   -    |   -    |
|   * Notification that the debugging has ended               |        |   -    |   -    |   -    |
|   * Skipping over functions                                 |        |   -    |   -    |   -    |
|   * Skipping over files                                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Breakpoints**                                             |        |        |        |        |
|   * Add line break point before starting the debugger       | 03-Aug |  Pass  |   ?    |   ?    |
|   * Add line break point after the starting the debugger    | 02-Aug |  Pass  |   ?    |   ?    |
|   * Debug menu option to Toggle line break point (F5)       | 02-Aug |  Pass  |   ?    |   ?    |
|   * Debug menu option to Remove all breakpoints             | 02-Aug |  Pass  |   ?    |   ?    |
|   * Disable/Enable line break point via pop up menu         | 03-Aug |  Pass  |   ?    |   ?    |
|   * Remove line break point                                 | 03-Aug |  Pass  |   ?    |   ?    |
|   * Edit line break point                                   |        |   -    |   -    |   -    |
|     * ignore count before break                             |        |   -    |   -    |   -    |
|     * break when expression is true                         |        |   -    |   -    |   -    |
|   * Break points still there after exit                     |        |   -    |   -    |   -    |
|   * Break points data saved on project close                |        |   -    |   -    |   -    |
|   * Break points removed after closing the project          |        |   -    |   -    |   -    |
|   * Break points removed after changing debugger            |        |   -    |   -    |   -    |
|   * Break points created on project open                    |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Exception Handling**                                      |        |        |        |        |
|   * Linux Debugger catches exceptions and shows call stack  | 06-Jul |  Pass  |   ?    |   ?    |
|   * MacOS Debugger catches exceptions and shows call stack  | 06-Jul |  Pass  |   ?    |   ?    |
|   * Windows Clang64 catches exceptions and shows call stack | 06-Jul |  Pass  |   ?    |   ?    |
|   * Windows MinGW64 catches exceptions and shows call stack | 02-Aug |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Watches**                                                 |        |        |        |        |
|   * watches dialog shows function args and local vars       |        |   -    |   -    |   -    |
|   * Add watch before starting the debugger                  | 03-Aug |  N/A   |  N/A   |  N/A   |
|   * Add watch after starting the debugger                   | 02-Aug |  Pass  |   ?    |   ?    |
|   * Simple data types                                       | 02-Aug |  Pass  |   ?    |   ?    |
|   * Simple structure                                        | 02-Aug |  Pass  |   ?    |   ?    |
|   * Array of simple structures                              | 03-Aug |  Fail  |   ?    |   ?    |
|   * Complex structures                                      |        | ?Fail? |   ?    |   ?    |
|   * Can expand cbProject complex structures                 |        | ?Fail? |   ?    |   ?    |
|   * Edit watches                                            |        |   -    |   -    |   -    |
|   * Watches removed after closing the project               | 03-Aug |  Pass  |   ?    |   ?    |
|   * Watches removed after changing debugger                 | 03-Aug |  Fail  |   ?    |   ?    |
|   * Watches data saved on project close                     |        |   -    |   -    |   -    |
|   * Watches created on project open                         |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Data Breakpoints**                                        |        |        |        |        ||
|   * Display data breakpoints in break point dialog          |        |   -    |   -    |   -    |
|   * Add data break point (right click pop up menu on var)   |        |   -    |   -    |   -    |
|   * Add support for deleting already added data break point |        |   -    |   -    |   -    |
|   * Add remove data breakpoints to Remove all breakpoints   |        |   -    |   -    |   -    |
|   * Add support for multiple data break points              |        |   -    |   -    |   -    |
|   * Data break points data is NOT saved on project close    |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug show Running Threads**                              |        |        |        |        |
|   * Show running threads dialog                             |        |   -    |   -    |   -    |
|   * Close running threads dialog                            |        |   -    |   -    |   -    |
|   * Updates on step/run                                     |        |   -    |   -    |   -    |
|   * Thread dialog cleared on app exit                       |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug show CPU Registers**                                |        |        |        |        |
|   * Show CPU register dialog                                |        |   -    |   -    |   -    |
|   * Close CPU register dialog                               |        |   -    |   -    |   -    |
|   * Update on step                                          |        |   -    |   -    |   -    |
|   * CPU register dialog cleared on exit                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug show Call Stack**                                   |        |        |        |        |
|   * Show call stack dialog                                  | 02-Aug |  Pass  |   ?    |   ?    |
|   * Close call stack dialog                                 | 03-Aug |  Pass  |   ?    |   ?    |
|   * Double click on entry should open and go to the line    | 03-Aug |  Pass  |   ?    |   ?    |
|   * Call stack dialog cleared on stopping debugging         | 03-Aug |  Pass  |   ?    |   ?    |
|   * Call stack dialog cleared on last project close         | 03-Aug |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Debug show Disassembly**                                  |        |        |        |        |
|   * Show disassembly dialog                                 |        |   -    |   -    |   -    |
|   * Close disassembly dialog                                |        |   -    |   -    |   -    |
|   * Assembly only                                           |        |   -    |   -    |   -    |
|   * Mixed mode                                              |        |   -    |   -    |   -    |
|   * Ability to swap between assembly and mixed modes        |        |   -    |   -    |   -    |
|   * Adjust button feature                                   |        |   -    |   -    |   -    |
|   * Save disassembly to a file                              |        |   -    |   -    |   -    |
|   * Disassembly dialog cleared on exit                      |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug -> Memory Dump Dialog**                             |        |        |        |        |
|   * Show memory dump dialog                                 |        |   -    |   -    |   -    |
|   * Close memory dump dialog                                |        |   -    |   -    |   -    |
|   * Show variable memory from watch dialog                  |        |   -    |   -    |   -    |
|   * Memory dump dialog not cleared on exit                  |        |   -    |   -    |   -    |
|   * Memory dump dialog cleared on last project close        |        |   -    |   -    |   -    |
|   * Memory dump watches data saved on project close         |        |   -    |   -    |   -    |
|   * Memory dump watches removed after changing debugger     |        |   -    |   -    |   -    |
|   * Memory dump watches created on project open             |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug -> Memory view Dialog**                             |        |        |        |        |
|   * Show memory view dialog                                 |        |   -    |   -    |   -    |
|   * Close memory view dialog                                |        |   -    |   -    |   -    |
|   * Show memory information                                 |        |   -    |   -    |   -    |
|   * Show variable memory from watch dialog                  |        |   -    |   -    |   -    |
|   * Memory view dialog cleared on last project close        |        |   -    |   -    |   -    |
|   * Memory view watches data saved on project close         |        |   -    |   -    |   -    |
|   * Memory view watches removed after closing the project   |        |   -    |   -    |   -    |
|   * Memory view watches removed after changing debugger     |        |   -    |   -    |   -    |
|   * Memory view watches created on project open             |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Show tty for console projects**                           |        |        |        |        |
|   * Show console on console app                             | 03-Aug |  Pass  |   ?    |   ?    |
|   * Do NOT show console on GUI app                          |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Projects - debugger options dialog**                      |        |        |        |        |
|   * Show debuggeroptionsprjdlg dialog                       |        |   -    |   -    |   -    |
|   * Close debuggeroptionsprjdlg dialog                      |        |   -    |   -    |   -    |
|   * Data saved /loaded                                      |        |   -    |   -    |   -    |
|   * Data used by Plugin                                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **DAP Debugger configuration dialog**                       |        |        |        |        |
|   * Show debugger options dialog                            | 02-Aug |  Pass  |   ?    |   ?    |
|   * Close debugger options dialog                           | 02-Aug |  Pass  |   ?    |   ?    |
|   * Auto detected lldb-vscode exe                           | 02-Aug |  Pass  |   ?    |   ?    |
|   * Default port # configured                               | 02-Aug |  Pass  |   ?    |   ?    |
|   * Executable path save/loaded/used                        | 02-Aug |  Pass  |   ?    |   ?    |
|   * Arguments save/loaded/used                              | 02-Aug |  Pass  |   ?    |   ?    |
|   * Arguments used in starting debugger                     | 02-Aug |  Pass  |   ?    |   ?    |
|   * Disable startup scripts checkbox save/loaded/used       |        |   -    |   -    |   -    |
|   * Watch function arguments checkbox save/loaded/used      |        |   -    |   -    |   -    |
|   * Watch local variables checkbox save/loaded/used         |        |   -    |   -    |   -    |
|   * Catch C++ exceptions checkbox save/loaded/used          | 02-Aug |  Pass  |   ?    |   ?    |
|   * Catch C++ throw checkbox save/loaded/used               | 02-Aug |  Pass  |   ?    |   ?    |
|   * Evaluate expression under cursor cbox save/loaded/used  |        |   -    |   -    |   -    |
|   * Add other projects paths... checkbox save/loaded/used   |        |   -    |   -    |   -    |
|   * Do not run the debugee checkbox save/loaded/used        |        |   -    |   -    |   -    |
|   * Disassembly flavor drop down list save/loaded/used      |        |   -    |   -    |   -    |
|   * Persists checkbox save/load breakpoint/watch etc        |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Remote Debugging**                                        |        |        |        |        |
|   * Use data from debuggeroptionsprjdlg dialog              |        |   -    |   -    |   -    |
|   * Ensure serial (RS232) debugging works                   |        |   -    |   -    |   -    |
|   * Ensure TCP debugging works                              |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Operaiting System**                                       |        |        |        |        |
|   * Works on Windows                                        | 02-Aug |  Pass  |   ?    |   ?    |
|   * Works on Linux                                          | 06-Jul |  Pass  |   ?    |   ?    |
|   * Works on MacOS                                          | 08-Jul |  Pass  |   ?    |   ?    |
|   * Builds on Windows via workspace                         | 02-Aug |  Pass  |   ?    |   ?    |
|   * Builds on Windows via MSYS2 bootstrap/configure/make    |        |   -    |   -    |   -    |
|   * Builds on Linux via workspace                           | 06-Jul |  Pass  |   ?    |   ?    |
|   * Builds on Linux via bootstrap/configure/make            | 06-Jul |  Pass  |   ?    |   ?    |
|   * Builds on MacOS via workspace                           | 08-Jul |  Pass  |   ?    |   ?    |
|   * Builds on MacOS via bootstrap/configure/make            | 08-Jul |  Pass  |   ?    |   ?    |
|   * Create and test DAP cbplugin for Windows                | 06-Jul |  Pass  |   ?    |   ?    |
|   * Create and test DAP cbplugin for Linux                  | 06-Jul |  Pass  |   ?    |   ?    |
|   * Create and test DAP cbplugin for MacOS                  | 08-Jul |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |

The table was last updated on 31-Aug-2022.

NOTES:
 "PASS"     - Testing indicated the feature worked when tested. Please raise an issue if you find it broken.
 "Fail"     - Testing of the feature did not work as expected.
 "?"        - Not tested yet. Do not raise any issues on this item if it does not work as expected.
 "-"        - Functionality that may/does not exist in the DAP debugger yet. Future work.
 "N/A"      - This feature is not applicable for the DAP debugger due.
