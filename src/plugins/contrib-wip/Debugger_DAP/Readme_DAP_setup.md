**WINDOWS**
On Windows you can now use the following DAP adapters:
    * MSYS2 MingW64  C:\msys64\mingw64\bin\lldb-vscode.exe
    * MSYS2 Clang64  C:\msys64\clang64\bin\lldb-vscode.exe
    * https://github.com/vadimcn/vscode-lldb using the extension\adapter\codelldb.exe file
    
NOTES:
1) There is a bug in the LLVM 14.0.6 lldb-vscode.exe with Windows directory paths that stops it from working with the plugin. It only works on C: and then needs.....

2) You need to set the PYTHONHOME variable in the Debugger setting dialog for the appropriate DAP adapter you use above as follows:
    * C:\msys64\mingw64\bin\lldb-vscode.exe       PYTHONHOME = C:\msys64\mingw64
    * C:\msys64\clang64\bin\lldb-vscode.exe       PYTHONHOME = C:\msys64\clang64
    * extension\adapter\codelldb.exe              PYTHONHOME = C:\Users\<username>\AppData\Local\Programs\Python\Python310
    
3) The following is a batch file to start the codelldb.exe DAP adapter with debugging output enabled. You will need to adjust it for your Windows configuration:

~~~
echo on
@setlocal
@SET CurrentDir="%CD%"
@set PYTHONHOME=C:\Users\<usrername>\AppData\Local\Programs\Python\Python310\
@set RUST_LOG=debug,codelldb=debug
@set RUST_LOG_STYLE=always
@set RUST_BACKTRACE=full
@cd /d C:\vscode-lldb\extension\adapter
codelldb.exe --port 12345
@endlocal
cd /d %CurrentDir%
~~~

4) The following is a batch file to start the MinGW64 lldb-vscode.exe DAP adapter

~~~
echo on
set PYTHONHOME=C:\msys64\mingw64
C:\msys64\mingw64\bin\lldb-vscode.exe --port 12345
~~~

5) The following is a batch file to start the Clang64 lldb-vscode.exe DAP adapter

~~~
echo on
set PYTHONHOME=C:\msys64\clang64
C:\msys64\clang64\bin\lldb-vscode.exe --port 12345
~~~
5) The following is a batch file to start the LLVM lldb-vscode.exe DAP adapter

~~~
echo on
set PYTHONHOME=C:\Users\<username>\AppData\Local\Programs\Python\Python310\
C:\LLVM\bin\lldb-vscode.exe --port 12345
~~~

**MACOS**

On the Mac Intel computers you can now use the following DAP adapters:
    * LLVM 14.0.6 lldb-vscode  /usr/local/opt/llvm/bin/lldb-vscode
    * https://github.com/vadimcn/vscode-lldb using the extension/adapter/codelldb file
    
NOTES:
1) You will need to stop the Mac converting down dash "--" to an emdash "—" (longer dash). The following web page has info on how to do this:
https://superuser.com/questions/555628/how-to-stop-mac-to-convert-typing-double-dash-to-emdash

From the page for Mac 10.9 and above:
Go to System Preferences, then "Language & Region" then click the "Keyboard Preference ..." button and to go to "Text" tab. 
It is no longer a substitution, however, but instead on the right-hand side of the window there is a 
tickbox "Use smart quotes and dashes". Un-ticking this stops it changing two hyphens into an en-dash.

One little gotcha: you must exit then restart your editor to have this change take effect.

2) The following is a shell script to start the codelldb.exe DAP adapter with debugging output enabled. You will need to adjust it for your Mac configuration:

~~~
#!/bin/sh
if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi
#export PYTHONHOME=C:\Users\andrew\AppData\Local\Programs\Python\Python310
export RUST_LOG=debug,codelldb=debug
export RUST_LOG_STYLE=always
export RUST_BACKTRACE=full
./codelldb --port 12345
~~~
