Windows Clangd-Client Plugin install process:
============================================
1) Install the LLVM or Clangd.exe as documented in the following file:
    Windows-LLVM-ClangD-Install-Readme.txt
2) Disable the Code completion plugin as follows:
    a) Open the Plugin manager via the Code::Blocks "MainMenu=>Plugins=>Manage plugins..." menu
    b) In the Manage Plugin dialog do the following:
       i) Find and select the "Code completion" plugin via it's title 
       ii) Press the "Disable" button on the right near the top
       iii) If you get any errors please try again.
3) Install the Clangd-Client Plugin using one of the following options, which are documneted later in this readme file:
    a) Install via the Plugin Manager
    b) Manaully install the plugin files
3) Configure the Clangd-Client Plugin for use as follows:
    a) Select the "MainMenu=>Settings->Editor..." menu
    b) In the list on the left click/select the "clangd_client" option.
    c) In the "C/C++ parser" tab change the "Specify clangd executable to use" to reference the clangd.exe you installed via step 1) above. 
         Some examples of this could be:
            C:\msys64\clang64\bin\clangd.exe
            C:\msys64\clang32\bin\clangd.exe
            C:\LLVM\bin\clangd.exe
            C:\comilers\cmang\clangd.exe

Install Clangd-Client Plugin via the Plugin Manager 
---------------------------------------------------
1) Open the Plugin manager via the Code::Blocks "MainMenu=>Plugins=>Manage plugins..." menu
2) In the Manage Plugin dialog do the following:
    a) Press the "Install new" button on the left
    b) Specifiy the plugin "clangd_client.zip" that you downloaded
    c) Once then plugin has installed you need to restart Code::Blocks for the Code::Blocks to work correctly with the plugin otherwise
       you may experience a crash when trying to configure the plugin.


Manually Install Clangd-Client Plugin
-------------------------------------
1) Navigate to the downloaded file (clangd_client.zip) and unzip it to a temporary folder. DO NOT UNZIP in the same directory!!!
2) Copy the unziped clangd_client.zip file to the Code::Blocks ...\share\CodeBlocks folder.
3) Copy the unziped clangd_client.dll file to the Code::Blocks ...\share\Codeblocks\plugins folder
4) Restart Code::Blocks.

Remove Clangd-Client Plugin via the Plugin Manager
--------------------------------------------------
1) Close any openned project or workspace's otherwise you may encounter a crash.
2) Open the Plugin manager via the Code::Blocks "MainMenu=>Plugins=>Manage plugins..." menu
3) In the Manage Plugin dialog do the following:
    a) In the plugin list select the "Clangd_Client" row
    b) Press the "Uninstall" button on the right
    c) If Code::Blocks is installed in one of the "Program Files" directory you will need to run Code::Blocks as administrator 
       in order to uninstall the plugn due to Windows priviledges.


Manually Remove Clangd-Client Plugin
------------------------------------
1) Exit Code::Blocks!
2)  a) If you manually installed the files or used the Plugin manager then you can do the following:
        i)  In the Code::Blocks ...\share\CodeBlocks folder delete the clangd_client.zip file
        ii) In the Code::Blocks ...\share\CodeBlocks\plugins folder delete the clangd_client.dll file
    b) If installed via the Plugin manager then you can delete the files with the following commands:
        i)  del %APPDATA%\CodeBlocks\share\codeblocks\plugins\clangd_client.dll
        ii) del %APPDATA%\CodeBlocks\share\codeblocks\clangd_client.zip
3) If you want to use code completion rememner to re-enable the plugin
