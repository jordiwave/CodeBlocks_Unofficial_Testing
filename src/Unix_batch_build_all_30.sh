#!/bin/sh

# --------------------------------------------------------------------------
# If your build is very slow and there is a HUGE amount of warning being
#  shown for wxWidget header files then one way of removing these warnings
#  is to modify the wx_config file to change the 
#  GCC "-I<include_path>" parameters tp "-isystem<include_path>" 
#
# On Linux Mint 20.3 the GTK3 wx_config file is:
#  /usr/lib/x86_64-linux-gnu/wx/config/gtk3-unicode-3.0
# --------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Setup C::B workspace to be used to compile C::B
# --------------------------------------------------------------------------
export set CB_WorkSpaceFileName=CodeBlocks_wx30-unix.workspace

# --------------------------------------------------------------------------
# Setup C::B application to run the *binaries* (!)
# --------------------------------------------------------------------------
if [ -e /usr/bin/codeblocks ]; then
    export CB_APP=/usr/bin/codeblocks
else
    echo Cannot find the "/usr/bin/codeblocks" file. Please install Code::Blocks and try again.
    exit 0
fi
# --------------------------------------------------------------------------
# Setup folder to the C::B *sources* from SVN (!)
# --------------------------------------------------------------------------
if [ -e $PWD/$CB_WorkSpaceFileName ]; then
    export CB_SRC=$PWD
else
    echo Cannot find the "$PWD/$CB_WorkSpaceFileName" file. Please fix and try again.
    exit 1
fi

# --------------------------------------------------------------------------
# Usually below here no changes are required.
# --------------------------------------------------------------------------
if [ -e $CB_APP ]; then
  if [ -d $CB_SRC ]; then
    #export CB_PARAMS="--batch-build-notify --no-batch-window-close --multiple-instance --verbose"
    export CB_PARAMS="--batch-build-notify --no-batch-window-close --multiple-instance --log-to-file CB_Build_Log.txt"
    export CB_TARGET="--target=All"
    export CB_CMD="--rebuild $CB_SRC/$CB_WorkSpaceFileName"

    echo $CB_APP $CB_PARAMS $CB_TARGET $CB_CMD
    $CB_APP $CB_PARAMS $CB_TARGET $CB_CMD
    echo "Do not forget to run 'update' after successful build!"
  else
    echo "C::B sources at '$CB_SRC' not found."
  fi
else
  echo "File '$CB_APP' does not exists."
fi

# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
