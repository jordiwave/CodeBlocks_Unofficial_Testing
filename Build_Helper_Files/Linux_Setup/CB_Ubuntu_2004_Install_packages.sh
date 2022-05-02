#!/bin/bash

# -----------------------------------------------------------------

if [ "$(id -u)" != "0" ]; then
    echo "Sorry, you are not root. Please run again as root!!!"
    exit 1
fi

# -----------------------------------------------------------------
# To enable sending the output of this script to the terminal and to the file specified uncomment the following line:
exec > >(tee -i Build_CB_Output.log) 2>&1
# NOTE: if you want to append to the file change the -i to -ia in the line above.

# -----------------------------------------------------------------

# source: https://github.com/arnholm/cpde_3rdparty/blob/master/gcc/codeblocks/build_cb.sh
# discussion: https://forums.codeblocks.org/index.php/topic,24628.msg168076.html#msg168076
#
# Wiki Pages:
# https://wiki.codeblocks.org/index.php/Installing_Code::Blocks_from_source_on_Linux
#   -> WX 2.8
# https://wiki.codeblocks.org/index.php/Installing_Code::Blocks_from_source_on_Arch_Linux

#------------------ install wxWidget packages  ------------------

sudo apt install -y wx3.0-headers wx-common libwxgtk3.0-gtk3-0v5 libwxgtk3.0-gtk3-dev libwxbase3.0-dev
sudo apt install -y build-essential
sudo apt install -y libtool automake autoconf
sudo apt install -y libgtk-3-dev
sudo apt install -y libboost-dev libboost-system-dev libhunspell-dev libgamin-dev 
sudo apt install -y subversion git
sudo apt install -y libbz2-dev debhelper cdbs xsltproc

#-------------------- install GNU GCC 11  ------------------------
add-apt-repository ppa:ubuntu-toolchain-r/test  --yes
apt-get update
apt-get -y  install cpp-11 gcc-11 g++-11
apt-get -y install gdb
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
# #update-alternatives --config gcc

#--------------------- Update packages  --------------------
apt-get update
apt-get upgrade --yes

#-------- Update package dependencies if they were not installed, just in case --------
apt-get install -f --yes
