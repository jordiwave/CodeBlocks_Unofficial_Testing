#ifndef COMPMAP_H
#define COMPMAP_H

#include "wx_pch.h"

class CompItem;

WX_DECLARE_OBJARRAY(CompItem*, CompItemArray);

struct CompItem
{
    wxString        m_Name;
    wxArrayString   m_Files;
    CompItemArray   m_Childs;
    wxArrayInt      m_Sections;
    int             m_Checked; //0 = emp 1 = check 2 = checkgray
    bool            m_Haschilds;
};

struct NGFOptions
{
    wxString Name;
    wxString OutputName;
    int InstallPath;
    bool Shortcut;
    bool License;
    bool Components;
    bool Directory;
    bool ConfirmUI;
    bool Uninstall;

    wxString LicPath;
    wxString LicText;

    wxArrayString InstallTypes;
    CompItemArray Items;

    wxArrayString Files;
};

#endif // COMPMAP_H

