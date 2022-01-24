#!/bin/sh

# ------------------------------------------------
# Setup C::B workspace to be used to compile C::B
# ------------------------------------------------
export set CB_ProjectFileName=CodeBlocks_wx30-unix.cbp

# ------------------------------------------------
# Setup C::B application to run the *binaries* (!)
# ------------------------------------------------
if [ -e /usr/bin/codeblocks ]; then
    export CB_APP=/usr/bin/codeblocks
else
    echo Cannot find the "/usr/bin/codeblocks" file. Please install Code::Blocks and try again.
    exit 0
fi
# ------------------------------------------------
# Setup folder to the C::B *sources* from SVN (!)
# ------------------------------------------------
if [ -e $PWD/$CB_ProjectFileName ]; then
    export CB_SRC=$PWD
else
    echo Cannot find the "$PWD/$CB_ProjectFileName" file. Please fix and try again.
    exit 1
fi

# -------------------------------------------
# Usually below here no changes are required.
# -------------------------------------------
if [ -e $CB_APP ]; then
  if [ -d $CB_SRC ]; then
    export CB_PARAMS="--batch-build-notify --no-batch-window-close --multiple-instance --verbose"
    export CB_TARGET="--target=All"
    export CB_CMD="--rebuild $CB_SRC/$CB_ProjectFileName"

    echo $CB_APP $CB_PARAMS $CB_TARGET $CB_CMD
    $CB_APP $CB_PARAMS $CB_TARGET $CB_CMD
    echo "Do not forget to run 'update' after successful build!"
  else
    echo "C::B sources at '$CB_SRC' not found."
  fi
else
  echo "File '$CB_APP' does not exists."
fi
