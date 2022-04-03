//******************************************************************************
//* Name:      menu.h
//* Purpose:   menu stuff
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef MENU_H
#define MENU_H

#include <vector>
#include <wx/menu.h>
#include <wx/string.h>
#include "CBSvnPluginManager.h"

namespace CBTSVN
{

struct MenuEntry
{
    enum MenuEntryKind
    {
        mekNormal = 0,
        mekCheck,
        mekRadio
    };

    int menu_id;
    int popup_id;
    MenuEntryKind kind;
    bool file_enabled;
    bool project_enabled;
    bool workspace_enabled;
    bool required_main;
    bool required_popup;
    wxString name;
    wxString text;
    wxString help;

    MenuEntry(int entry_menu_id,
              int entry_popup_id,
              MenuEntryKind entry_kind,
              bool entry_file_enabled,
              bool entry_project_enabled,
              bool entry_workspace_enabled,
              bool entry_required_main,
              bool entry_required_popup,
              wxString entry_name,
              wxString entry_text,
              wxString entry_help)
        :
        menu_id(entry_menu_id),
        popup_id(entry_popup_id),
        kind(entry_kind),
        file_enabled(entry_file_enabled),
        project_enabled(entry_project_enabled),
        workspace_enabled(entry_workspace_enabled),
        required_main(entry_required_main),
        required_popup(entry_required_popup),
        name(entry_name),
        text(entry_text),
        help(entry_help)
    {}
};

bool GetIndex(const std::vector<CBTSVN::MenuEntry> & menu, const wxString & item, int & index);

std::vector<wxString> ConvertMenuItemsToStrings(const CBSvnPluginManager::SourceMenu source,
                                                const std::vector<MenuEntry> & menu);

void ConvertMenuItemsToIndices(const std::vector<MenuEntry> & menu,
                               const std::vector<wxString> & items,
                               std::vector<int> & indices);

void ConvertIndicesToMenuItems(const std::vector<MenuEntry> & menu,
                               const std::vector<int> & indices,
                               std::vector<wxString> & items);

void MakeMenu(const CBSvnPluginManager::SourceMenu source,
              const std::vector<MenuEntry> & menu,
              const std::vector<int> indices,
              bool file_based,
              bool project_based,
              bool workspace_based,
              bool under_version_control,
              wxMenu & out_menu);

void MakeMenu(const CBSvnPluginManager::SourceMenu source,
              const std::vector<MenuEntry> & menu,
              const std::vector<wxString> & items,
              bool file_based,
              bool project_based,
              bool workspace_based,
              bool under_version_control,
              wxMenu & out_menu);

void UpdateMenu(const CBSvnPluginManager::SourceMenu source,
                const std::vector<MenuEntry> & menu,
                const std::vector<wxString> & items,
                bool file_based,
                bool project_based,
                bool workspace_based,
                bool under_version_control,
                wxMenu * in_menu);


void UpdateMenu(const CBSvnPluginManager::SourceMenu source,
                const std::vector<MenuEntry> & menu,
                const std::vector<int> indices,
                bool file_based,
                bool project_based,
                bool workspace_based,
                bool under_version_control,
                wxMenu * in_menu);
}

#endif // MENU_H
