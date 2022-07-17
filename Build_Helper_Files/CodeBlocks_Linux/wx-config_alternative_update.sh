#!/bin/bash

# --------------------------------------------------------------------------------------#
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
# --------------------------------------------------------------------------------------#
# https://wiki.wxwidgets.org/Compiling_and_getting_started
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" != "0" ]; then
    echo "You need to be root. Please run again as root  user!!!"
    exit 1
fi

# -----------------------------------------------------------------

if [ -f "/usr/local/lib/wx/config/gtk3-unicode-3.1" ] ; then
    echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.1 310"
    sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.1 310
fi

if [ -f "/usr/local/lib/wx/config/gtk3-unicode-3.2" ] ; then
    echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.2 320"
    sudo update-alternatives --install /usr/bin/wx-config wx-config /usr/local/lib/wx/config/gtk3-unicode-3.2 320
else
    if [ -f "/opt/wx/lib/wx/config/gtk3-unicode-3.2" ] ; then
        echo "run: sudo update-alternatives --install /usr/bin/wx-config wx-config /opt/wx/lib/wx/config/gtk3-unicode-3.2 320"
        sudo update-alternatives --install /usr/bin/wx-config wx-config /opt/wx/lib/wx/config/gtk3-unicode-3.2 320
    fi
fi
echo "Use 'sudo update-alternatives --config wx-config' to change from wx3.2 to wx3.0 or wx3.1"
update-alternatives --list  wx-config
sudo update-alternatives --config wx-config

# -----------------------------------------------------------------
