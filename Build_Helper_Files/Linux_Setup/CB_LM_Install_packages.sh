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

# -----------------------------------------------------------------

# BAT / CMD goto function
function goto
{
    label=$1
    cmd=$(sed -n "/^:[[:blank:]][[:blank:]]*${label}/{:a;n;p;ba};" $0 |
          grep -v ':$')
    eval "$cmd"
    exit
}

#------------------ install wxWidget packages  ------------------
sudo apt install -y libwxbase3.0-dev

#-------- install build packages for codeblocks with gtk2 --------
# NOTE: wxWidget DROPPED GTK2 support version 3.0.4+dfsg-15 (the one included in Ubuntu 20.04)
#    sudo apt install -y libgtk2.0-dev  
# FAIL sudo apt install -y libwxgtk3.0-dev libwxgtk3.0-dbg

#-------- install build packages for codeblocks with gtk3 --------
apt-get -y update
apt-get -y install xterm
curl -O http://archive.ubuntu.com/ubuntu/pool/universe/w/wxwidgets3.0/libwxgtk3.0-gtk3-0v5_3.0.5.1+dfsg-2_amd64.deb
curl -O http://archive.ubuntu.com/ubuntu/pool/universe/w/wxwidgets3.0/libwxgtk3.0-gtk3-dev_3.0.5.1+dfsg-2_amd64.deb
curl -O http://ftp.us.debian.org/debian/pool/main/w/wxwidgets3.0/wx3.0-headers_3.0.5.1+dfsg-2_all.deb -O debian.deb
#BAD: curl -O http://archive.ubuntu.com/ubuntu/pool/universe/w/wxwidgets3.0/wx3.0-headers_3.0.5.1+dfsg-2_all.deb -O ubuntu.deb
curl -O http://archive.ubuntu.com/ubuntu/pool/universe/w/wxwidgets3.0/libwxbase3.0-dev_3.0.5.1+dfsg-2_amd64.deb
curl -O http://archive.ubuntu.com/ubuntu/pool/universe/w/wxwidgets3.0/libwxbase3.0-0v5_3.0.5.1+dfsg-2_amd64.deb

dpkg -i wx3.0-headers_3.0.5*_all.deb
dpkg -i libwxbase3.0-0v5_3.0.5*_amd64.deb
dpkg -i libwxbase3.0-dev_3.0.5*_amd64.deb
dpkg -i libwxgtk3.0-gtk3-0v5*_amd64.deb
dpkg -i libwxgtk3.0-gtk3-dev*_amd64.deb

sudo apt install -y libgtk-3-dev

#------------ install build essentials for codeblocks ------------
sudo apt install -y build-essential
sudo apt install -y subversion git
sudo apt install -y libtool automake autoconf

#------------------ install additional packages for codeblocks --------
sudo apt install -y libboost-dev libboost-system-dev libhunspell-dev libgamin-dev 

#-------------------- install GNU GCC 11  ------------------------
add-apt-repository ppa:ubuntu-toolchain-r/test  --yes
apt-get update
apt-get -y  install cpp-11 gcc-11 g++-11
apt-get -y install gdb
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-9
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
# #update-alternatives --config gcc

#--------------------- Update packages  --------------------
apt-get update
apt-get upgrade --yes

#-------- Update package dependencies if they were not installed, just in case --------
apt-get install -f --yes

:finished