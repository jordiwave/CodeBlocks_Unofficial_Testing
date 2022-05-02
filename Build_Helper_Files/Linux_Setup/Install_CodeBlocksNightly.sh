#!/bin/bash


if [ "$(id -u)" != "0" ]; then
    echo "Sorry, you are not root. Please change to root and run again!!!"
    exit 1
fi

# -----------------------------------------------------------------
# To enable sending the output of this script to the terminal and to the file specified uncomment the following line:
exec > >(tee -i Install_Common_Script_Output.log) 2>&1
# NOTE if you want to append to the file change the -i to -ia in the line above.


# ----------------------------------------------------------------------------------------------------------------------------
#add-apt-repository ppa:fuscated/codeblocks-nightly --yes
if grep -q apt.xaviou.fr /etc/apt/sources.list /etc/apt/sources.list.d/*; then
    echo "Deb https://apt.xaviou.fr/debian11/ bullseye main allready added"
else
    add-apt-repository "deb https://apt.xaviou.fr/debian11/ bullseye main"
    add-apt-repository "deb-src https://apt.xaviou.fr/debian11/ bullseye main"
fi
if apt-key list 2> /dev/null | grep Xaviou; then
    echo "apt-key for https://apt.xaviou.fr has already been added"
else
    wget -O- https://apt.xaviou.fr/key.asc | apt-key add -
fi    

FilePrefsCB=/etc/apt/preferences.d/codeblocks
if test -f "$FilePrefsCB"; then
    if grep -q apt.xaviou.fr "$FilePrefsCB"; then
        echo "$FilePrefsCB exists and has allready been setup."
    else
        echo "$FilePrefsCB exists, but has not been setup yet."
        echo "Package: *" >> $FilePrefsCB
        echo "Pin: origin apt.xaviou.fr" >> $FilePrefsCB
        echo "Pin-Priority: 999" >> $FilePrefsCB
        cat "$FilePrefsCB"
    fi
else
    echo "$FilePrefsCB DOES not exists!"
    echo "Package: *" >> $FilePrefsCB
    echo "Pin: origin apt.xaviou.fr" >> $FilePrefsCB
    echo "Pin-Priority: 999" >> $FilePrefsCB
    cat "$FilePrefsCB"
fi

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

apt-get -y install codeblocks


apt-get install -f --yes
apt --fix-broken install
