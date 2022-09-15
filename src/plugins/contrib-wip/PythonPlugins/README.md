# Codeblocks-Python

A variety of plugins for the Code::Blocks IDE that will be useful for python programmers. 

Plugins include:
    * Code completion
    * Visual debugger
    * Python interpreter console
    * Code checker

All plugins assume that python is installed and in the system path.

## Code Completion

* Uses the jedi completion lib (must be installed)
* Provides completion tips for any open python source file
* TODO: Symbol browser pane, showing docstrings alongside completion hints,

![codecompletion1](/screenshots/codecompletion1.png "codecompletion1")

## Visual Debugger

* Uses pdb (installed by default in any python)
* TODO: add support for rpdb2

To use it, make the python debugger the active debugger from the debugger menu

![debugger1](/screenshots/debugger1.png "debugger1")

Then open a source file and press Debug/Continue (from the Debugger toolbar or the Debugger menu).

![debugger2](/screenshots/debugger2.png "debugger2")

## Python Interpreter Console

* Run multiple python interpreters sessions inside a dockable pane in Code::Blocks
* Features syntax highlighting and automatic indentation in the code input control
* TODO: Handling stdin (e.g. raw_input), code completion hints, syntax error hints,
  extracting code/output from the I/O control, numbering instructions and output(?)

## Usage

Show the interpreters panel using the View menu:

![interpreters1](/screenshots/interpreters1.png "interpreters1")

Each interpreter has 2 panes, one for editing and submitting blocks of code, the other for displaying the history of submitted statements and any output

![interpreters2](/screenshots/interpreters2.png "interpreters2")

When the code control has the keyboard focus, press

    * 'Enter' with the keyboard positioned at the end of your block of code to submit it to the interpreter
    * 'Ctrl-Up' and 'Ctrl-Down' to browse through the history of previously submitted statements.


# History

The original source for these plugin is available from:
    * https://github.com/spillz/codeblocks-python

As of 10-Sep-2002 the code has been updated to build with the SF Code::Blocks SVN 12885 trunk source.

# ISSUES

The following is the list of known issues so far found:

1. The python executable name/references are hard coded in the files. Solution could be to add a compiler config setup dialog? All plugins assume that python is installed and in the system path.
2. Images are from a very old version for Code::Blocks and may need updating.
3. Update readme.md file
4. Create makefile.am files
5. Update makefile process to include new makefile.am files
6. Check build on MAcOS
7. Check Debian build process.
8. Test test and bug fix as these plugins compile and run, but need testing, debugging and fixes for issues found.
9. TODO - Symbol browser pane, showing docstrings alongside completion hints,
10. TODO - Interpreter: Handling stdin (e.g. raw_input), code completion hints, syntax error hints,
  extracting code/output from the I/O control, numbering instructions and output(?)
11. Add info on the python debugger dialog to indicate what the options are used for.
12. Add example python hello world for testing with and add instructions for it's use (most C::B devs use C++, not python)