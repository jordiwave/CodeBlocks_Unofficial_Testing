# CODE::BLOCKS  DAP DEBUGGER PLUGIN TO DAP PROTOCOL LIBRARY SUPPORT

## Description

The C::B DAP debugger interfaces to wxdap library. This document lists the wxDAP library interfaces and if the DAP plugin includes support for these interfaces.

The DAP debugger plugin does not include all of the features of the wxdap library.

|                   wxdap Feature                                                                   | wxdap  | Pluigin |
| :------------------------------------------------------------------------------------------------ | :----: | :-----: |
| **Mandatory requests**                                                                            |        |         |
| Connect                                                                                           |  [x]   |   [x]   |
| Initialize                                                                                        |  [x]   |   [x]   |
| Launch - This launch request is sent from the client to the debug adapter to start the debuggee   |  [x]   |   [x]   |
| SetBreakpointsFile - Sets all breakpoints for a single source                                     |  [x]   |   [x]   |
| Threads - The request retrieves a list of all threads.                                            |  [x]   |   [ ]   |
| Scopes - The request returns the variable scopes for a given stackframe ID                        |  [x]   |   [x]   |
| GetFrames - return list of frames for a given thread ID                                           |  [x]   |   [x]   |
| Continue - continue the execution                                                                 |  [x]   |   [x]   |
| Next - executes one step for the specified thread (with granularity: line/statement/instruction)  |  [x]   |   [x]   |
| StepIn - step into a function/method and allows all threads to resume running                     |  [X]   |   [X]   |
| StepOut - to step out (return) from a function/method and allows all threads to resume running    |  [X]   |   [X]   |
| Pause - pause the debugger execution                                                              |  [X]   |   [ ]   |
| BreakpointLocations - returns all possible locations for source breakpoints in a given range      |  [X]   |   [ ]   |
| SetFunctionBreakpoints - Replaces all existing function breakpoints with new function breakpoints |  [X]   |   [ ]   |
| Variables - return list of variables                                                              |  [X]   |   [X]   |
| EvaluateExpression - asks the debugger to evaluate an expression                                  |  [X]   |   [ ]   |
|                                                                                                   |        |         |
| **Lower priority requests**                                                                       |        |         |
| NextInstruction - executes one instruction for the specified thread                               |  [X]   |   [X]   |
| Goto - sets the location where the debuggee will continue to run.                                 |  [ ]   |   [ ]   |
| code or to execute code again                                                                     |        |         |
| ReadMemory - Reads bytes from memory at the provided location                                     |  [ ]   |   [ ]   |
| Disassemble - Disassembles code stored at the provided location                                   |  [ ]   |   [ ]   |
|                                                                                                   |        |         |
| ** Events**                                                                                       |        |         |
| Stopped - the execution stopped due to ... (breakpoint hit, exception, step etc)                  |  [x]   |   [x]   |
| Terminated - the debugging session terminated                                                     |  [x]   |   [x]   |
| Exited - the debuggee process exited                                                              |  [x]   |   [x]   |
| Initialized - dap server is initialized                                                           |  [x]   |   [x]   |
| Process - the debuggee process started                                                            |  [x]   |   [ ]   |
| Output - The event indicates that the target has produced some output                             |  [x]   |   [X]   |
| Continued - The debugger continued                                                                |  [x]   |   [ ]   |
| Breakpoint - a breakpoint state changed                                                           |  [x]   |   [ ]   |

The table was last updated on 03-Sep-2022.