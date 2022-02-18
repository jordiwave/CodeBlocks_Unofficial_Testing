@echo off
md ..\..\..\devel31_32\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output31_32\share\CodeBlocks\images\settings > nul 2>&1
copy .\Resources\*.png ..\..\..\devel31_32\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\Resources\*.png ..\..\..\output31_32\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0
