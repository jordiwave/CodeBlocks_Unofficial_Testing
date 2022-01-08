#! /bin/sh
# ----------------------------------------------------------------------------------------------------
# This script copies the compile/link output to $(TARGET_DEVEL_DIR). Currently .../trunk/src/devel30
# ----------------------------------------------------------------------------------------------------
if  [ -z "$1" ]; then
    echo parameter is: "$1"
    echo "ERROR: Post processing script file needs the CodeBlocks devel dir as parameter."
    echo "usually /home/<user>/proj/CodeBlocks/trunk/src"

    exit
fi
# The TARGET_DEVEL_DIR should be ...trunk/src
# Get absolute path the $(TARGET_DEVEL_DIR) and append '/src'
DEVEL_DIR=$1
echo "DEVEL_DIR is $DEVEL_DIR"
DEVEL_DIR=$(realpath $DEVEL_DIR)
echo "Absolute path: $DEVEL_DIR"
if [ -d $DEVEL_DIR ]; then
    echo "$DEVEL_DIR exists."
else
    echo "ERROR: $DEVEL_DIR does not exists."
    exit
fi

# create ...trunk/src/devel30/share/codeblocks if it doesn''t exist
if [ ! -d $DEVEL_DIR/devel30/share/codeblocks/plugins ]; then
    cmd="mkdir -p $DEVEL_DIR/devel30/share/codeblocks/plugins"
    echo $cmd
    eval $cmd
fi
echo .
# zip main resource manifest and menu .xrc files
rm -f  $DEVEL_DIR/devel30/share/codeblocks/clangd_client.zip
cmd="zip -jq9 $DEVEL_DIR/devel30/share/codeblocks/clangd_client.zip ./src/resources/manifest.xml ./src/resources/*.xrc"
echo $cmd
eval $cmd

echo .
# Change directory (cd) to the resource directory to avoid long dir names in the zip file and add the images
olddir=$(pwd)   #pushd .

cd ./src/resources
cmd="zip -r9 $DEVEL_DIR/devel30/share/codeblocks/clangd_client.zip images > /dev/null"
echo $cmd; eval $cmd
cd $olddir #popd

echo .
# Copy the plugin binary to the the development plugin directory
# echo "==> current dir is $(pwd) <=="
cmd="cp ./devel30/libclangd_client.so $DEVEL_DIR/devel30/share/codeblocks/plugins/"
echo $cmd; eval $cmd
