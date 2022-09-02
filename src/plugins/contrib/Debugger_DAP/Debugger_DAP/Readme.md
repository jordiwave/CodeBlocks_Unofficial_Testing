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

|                   Debugging Feature                         | Date   |clang64 |  Linux | MacOS  |
| :---------------------------------------------------------- | :----: | :----: | :----: | :----: |
| **Stepping**                                                |        |        |        |        |
|   * Start/Continue       (F8)                               | 31-Aug |  Pass  |   ?    |   ?    |
|   * Break debugger                                          |        |   -    |   -    |   -    |
|   * Stop debugger        (Shift-F8)                         | 31-Aug |  Pass  |   ?    |   ?    |
|   * Run to cursor        (F4)                               |        |   -    |   -    |   -    |
|   * Next line            (F7)                               | 31-Aug |  Pass  |   ?    |   ?    |
|   * Step Into            (Shift-F7)                         | 31-Aug |  Pass  |   ?    |   ?    |
|   * Step out             (Ctrl-F7)                          | 31-Aug |  Pass  |   ?    |   ?    |
|      * Display the return value of a function               |        |   -    |   -    |   -    |
|   * Next instruction     (Alt-F7)                           |        |   -    |   -    |   -    |
|   * Step into instruction(ALT-Shift-F7)                     |        |   -    |   -    |   -    |
|   * Set next statement                                      |        |   -    |   -    |   -    |
|   * Notification that the debugging has ended               | 01-Sep |  Pass  |   ?    |   ?    |
|   * Skipping over functions                                 |        |   -    |   -    |   -    |
|   * Skipping over files                                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Breakpoints**                                             |        |        |        |        |
|   * Add line break point before starting the debugger       | 31-Aug |  Pass  |   ?    |   ?    |
|   * Add line break point after the starting the debugger    | 31-Aug |  Pass  |   ?    |   ?    |
|   * Debug menu option to Toggle line break point (F5)       | 31-Aug |  Pass  |   ?    |   ?    |
|   * Debug menu option to Remove all breakpoints             | 31-Aug |  Pass  |   ?    |   ?    |
|   * Disable/Enable line break point via pop up menu         | 31-Aug |  Pass  |   ?    |   ?    |
|   * Remove line break point                                 | 31-Aug |  Pass  |   ?    |   ?    |
|   * Edit line break point                                   |        |   -    |   -    |   -    |
|     * ignore count before break                             |        |   -    |   -    |   -    |
|     * break when expression is true                         |        |   -    |   -    |   -    |
|   * Break points still there after stopping/exit debugger   | 31-Aug |  Pass  |   ?    |   ?    |
|   * Break points data saved on project close                | 01-Sep |  Pass  |   ?    |   ?    |
|   * Break points removed after closing the project          | 01-Sep |  Pass  |   ?    |   ?    |
|   * Break points removed after changing debugger            |        |   -    |   -    |   -    |
|   * Break points created on project open                    | 01-Sep |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Exception Handling**                                      |        |        |        |        |
|   * Windows Clang64 catches exceptions and shows call stack | 31-Jul |  Pass  |   ?    |   ?    |
|   * Windows MinGW64 catches exceptions and shows call stack | 01-Sep |  Pass  |   ?    |   ?    |
|   * Linux Debugger catches exceptions and shows call stack  | 06-Jul |  TEST  |   ?    |   ?    |
|   * MacOS Debugger catches exceptions and shows call stack  | 06-Jul |  TEST  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Watches**                                                 |        |        |        |        |
|   * watches dialog shows function args and local vars       |        |   -    |   -    |   -    |
|   * Add watch before starting the debugger                  | 31-Aug |  N/A   |  N/A   |  N/A   |
|   * Add watch after starting the debugger                   | 31-Aug |  Pass  |   ?    |   ?    |
|   * Simple data types                                       | 31-Aug |  Pass  |   ?    |   ?    |
|   * Simple structure                                        | 02-Sep |  Pass  |   ?    |   ?    |
|   * Array of simple structures                              | 02-Sep |  Pass  |   ?    |   ?    |
|   * Complex structures                                      | 02-Sep |  Pass  |   ?    |   ?    |
|   * Edit watches                                            |        |   -    |   -    |   -    |
|   * Watches data saved on project close                     | 01-Sep |  Pass  |   ?    |   ?    |
|   * Watches removed after closing the project               | 31-Aug |  Pass  |   ?    |   ?    |
|   * Watches removed after changing debugger                 |        |   -    |   -    |   -    |
|   * Watches created on project open                         | 01-Sep |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Data Breakpoints**                                        |        |        |        |        |
|   * Display data breakpoints in break point dialog          |        |   -    |   -    |   -    |
|   * Add data break point (right click pop up menu on var)   |        |   -    |   -    |   -    |
|   * Add support for deleting already added data break point |        |   -    |   -    |   -    |
|   * Add remove data breakpoints to Remove all breakpoints   |        |   -    |   -    |   -    |
|   * Add support for multiple data break points              |        |   -    |   -    |   -    |
|   * Data break points data saved on project close           |        |   -    |   -    |   -    |
|   * Data break points removed after closing the project     |        |   -    |   -    |   -    |
|   * Data break points removed after changing debugger       |        |   -    |   -    |   -    |
|   * Data break points created on project open               |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug  Menu ->  Running Threads Dialog**                  |        |        |        |        |
|   * Show running threads dialog                             |        |   -    |   -    |   -    |
|   * Close running threads dialog                            |        |   -    |   -    |   -    |
|   * Updates on step/run                                     |        |   -    |   -    |   -    |
|   * Thread dialog cleared on app exit                       |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug Menu -> Breakpoints Dialog**                        |        |        |        |        |
|   * Show Breakpoints dialog                                 | 31-Aug |  Pass  |   ?    |   ?    |
|   * Close Breakpoints dialog                                | 31-Aug |  Pass  |   ?    |   ?    |
|   * Pop up context menu on a breakpoint:                    |        |        |        |        |
|       * Open in editor                                      | 31-Aug |  Pass  |   ?    |   ?    |
|       * Edit                                                |        |   -    |   -    |   -    |
|       * Disable                                             | 01-Sep |  Pass  |   ?    |   ?    |
|       * Remove                                              | 31-Aug |  Pass  |   ?    |   ?    |
|       * Remove all                                          | 31-Aug |  Pass  |   ?    |   ?    |
|       * Show temporary                                      |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug Menu -> CPU Registers Dialog**                      |        |        |        |        |
|   * Show CPU register dialog                                |        |   -    |   -    |   -    |
|   * Close CPU register dialog                               |        |   -    |   -    |   -    |
|   * Update on step                                          |        |   -    |   -    |   -    |
|   * CPU register dialog cleared on exit                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug Menu -> Call Stack Dialog**                         |        |        |        |        |
|   * Show call stack dialog                                  | 31-Aug |  Pass  |   ?    |   ?    |
|   * Close call stack dialog                                 | 31-Aug |  Pass  |   ?    |   ?    |
|   * Double click on entry should open and go to the line    | 31-Aug |  Pass  |   ?    |   ?    |
|   * Call stack dialog cleared on stopping debugging         | 31-Aug |  Pass  |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Debug Menu -> Disassembly Dialog**                        |        |        |        |        |
|   * Show disassembly dialog                                 |        |   -    |   -    |   -    |
|   * Close disassembly dialog                                |        |   -    |   -    |   -    |
|   * Assembly only                                           |        |   -    |   -    |   -    |
|   * Mixed mode                                              |        |   -    |   -    |   -    |
|   * Ability to swap between assembly and mixed modes        |        |   -    |   -    |   -    |
|   * Adjust button feature                                   |        |   -    |   -    |   -    |
|   * Save disassembly to a file                              |        |   -    |   -    |   -    |
|   * Disassembly dialog cleared on exit                      |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Debug Menu -> Memory Dump Dialog**                        |        |        |        |        |
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
|   * Show console on console app                             | 31-Aug |  Pass  |   ?    |   ?    |
|   * printf display on console on console app                | 01-Sep |**FAIL**|   ?    |   ?    |
|   * Do NOT show console on GUI app                          |        |   ?    |   ?    |   ?    |
|                                                             |        |        |        |        |
| **Projects - debugger options dialog**                      |        |        |        |        |
|   * Show debuggeroptionsprjdlg dialog                       |        |   -    |   -    |   -    |
|   * Close debuggeroptionsprjdlg dialog                      |        |   -    |   -    |   -    |
|   * Data saved /loaded                                      |        |   -    |   -    |   -    |
|   * Data used by Plugin                                     |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **DAP Debugger configuration dialog**                       |        |        |        |        |
|   * Show debugger options dialog                            | 31-Aug |  Pass  |   ?    |   ?    |
|   * Close debugger options dialog                           | 31-Aug |  Pass  |   ?    |   ?    |
|   * Auto detected lldb-vscode exe                           | 01-Sep |  Pass  |   ?    |   ?    |
|   * Executable path save/loaded/used                        | 31-Aug |  Pass  |   ?    |   ?    |
|   * Network port # save/loaded/used                         | 31-Aug |  Pass  |   ?    |   ?    |
|   * Run DAP server checkbox save/loaded/used                | 31-Aug |  Pass  |   ?    |   ?    |
|   * Stop at main checkbox save/loaded/used                  | 31-Aug |  Pass  |   ?    |   ?    |
|   * Enable exceptions C++ catch checkbox save/loaded/used   | 31-Aug |  Pass  |   ?    |   ?    |
|   * Enable exceptions C++ throw checkbox save/loaded/used   | 31-Aug |  Pass  |   ?    |   ?    |
|   * Persists checkbox save/load breakpoint/watch etc        | 01-Sep |  Pass  |   ?    |   ?    |
|   * Choose directory configuration options save/loaded/used | 31-Aug |  Pass  |   ?    |   ?    |
|   * Choose assembly flavor options save/loaded/used         |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Remote Debugging**                                        |        |        |        |        |
|   * Use data from debuggeroptionsprjdlg dialog              |        |   -    |   -    |   -    |
|   * Ensure serial (RS232) debugging works                   |        |   -    |   -    |   -    |
|   * Ensure TCP debugging works                              |        |   -    |   -    |   -    |
|                                                             |        |        |        |        |
| **Operating System**                                        |        | Windows| Linux  | MacOS  |
|   * Builds via workspace                                    | 31-Aug |  Pass  |   ?    |   ?    |
|   * Builds bootstrap/configure/make                         | 06-Jul |  TEST  |  TEST  | TEST   |
|   * Builds on Linux via Debian process                      | 06-Jul |  N/A   |  TEST  |  N/A   |
|   * Create DAP cbplugin                                     |        |   ?    |   ?    |   ?    |
|   * Test DAP cbplugin with Latest nightly                   |        |   ?    |   ?    |   ?    |
|                                                             |        |        |        |        |

The table was last updated on 31-Aug-2022.

NOTES:
 "TEST"     - To be tested, passed when last tested.
 "Pass"     - Testing indicated the feature worked when tested. Please raise an issue if you find it broken.
 "Fail"     - Testing of the feature did not work as expected.
 "?"        - Not tested yet. Do not raise any issues on this item if it does not work as expected.
 "-"        - Functionality that may/does not exist in the DAP debugger yet. Future work.
 "N/A"      - This feature is not applicable for the DAP debugger due.
