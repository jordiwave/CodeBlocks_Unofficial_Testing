/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef __WINDOWSXPLOOKNFEEL_H__
#define __WINDOWSXPLOOKNFEEL_H__

#include "cbplugin.h" // the base class we 're inheriting

class WindowsXPLookNFeel : public cbToolPlugin
{
    public:
        WindowsXPLookNFeel();
        ~WindowsXPLookNFeel();
        int Execute();
};

#endif // __WINDOWSXPLOOKNFEEL_H__

