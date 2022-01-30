//******************************************************************************
//* Name:      menu.cpp
//* Purpose:   menu stuff
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************


#include "menu.h"
#include "utils.h"
#include <wx/menu.h>

//******************************************************************************

using namespace CBTSVN;

//******************************************************************************

bool CBTSVN::GetIndex(const std::vector<MenuEntry>& menu, const wxString& item, int& index)
{
    if (item.find(_("------"))!=wxString::npos)
    {
        // separator
        index=-1;
        return true;
    }

    for (size_t i=0; i<menu.size(); ++i)
        if (menu.at(i).text.find(item)!=wxString::npos)
        {
            index=i;
            return true;
        }

    return false;
}

//******************************************************************************

std::vector<wxString> CBTSVN::ConvertMenuItemsToStrings(const CBSvnPluginManager::SourceMenu source,
        const std::vector<MenuEntry>& menu)
{
    std::vector<wxString> list;
    for (size_t i=0; i<menu.size(); ++i)
    {
        int id;
        if (source==CBSvnPluginManager::smMainMenu)
            (menu.at(i).required_main) ?  id=-1 : id=menu.at(i).menu_id;
        else
            (menu.at(i).required_popup) ? id=-1 : id=menu.at(i).popup_id;

        // Don't add required menu items to the output
        if (id!=-1)
            list.push_back(menu.at(i).text);
    }
    return list;
}

//******************************************************************************

void CBTSVN::ConvertMenuItemsToIndices(
    const std::vector<CBTSVN::MenuEntry>& menu,
    const std::vector<wxString>& items,
    std::vector<int>& indices)
{
    indices.clear();

    int index;
    for (size_t i=0; i<items.size(); i++)
        if (GetIndex(menu,items.at(i),index))
            indices.push_back(index);
}

//******************************************************************************

void CBTSVN::ConvertIndicesToMenuItems(
    const std::vector<MenuEntry>& menu,
    const std::vector<int>& indices,
    std::vector<wxString>& items)
{
    items.clear();
    for (size_t i=0; i<indices.size(); i++)
        if (indices.at(i)==-1)
            items.push_back(CBTSVN::separator);
        else
        {
            size_t index = indices.at(i);

            // prevent out of index
            if ((index<0) || (index > (menu.size()-1)))
                continue;

            items.push_back(menu.at(index).text);
        }
}

//******************************************************************************

void CBTSVN::MakeMenu(const CBSvnPluginManager::SourceMenu source,
                      const std::vector<MenuEntry>& menu,
                      const std::vector<int> indices,
                      bool file_based,
                      bool project_based,
                      bool workspace_based,
                      bool under_version_control,
                      wxMenu& out_menu)
{
    // add variable menu items
    for (size_t i=0; i<indices.size(); i++)
    {
        if (indices.at(i)==-1)
            out_menu.AppendSeparator();
        else
        {
            size_t index = indices.at(i);

            // prevent out of index
            if ((index<0) || (index > (menu.size()-1)))
                continue;

            int id;
            source==CBSvnPluginManager::smMainMenu ? id=menu.at(index).menu_id : id=menu.at(index).popup_id;

            bool enabled=under_version_control;
            if (!menu.at(index).file_enabled && !menu.at(index).project_enabled && !menu.at(index).workspace_enabled)
                enabled=true;

            if (menu.at(index).kind==MenuEntry::mekNormal)
                out_menu.Append(id, menu.at(index).text, menu.at(index).help);
            else if (menu.at(index).kind==MenuEntry::mekRadio)
                out_menu.AppendRadioItem(id, menu.at(index).text, menu.at(index).help);
            else if (menu.at(index).kind==MenuEntry::mekCheck)
                out_menu.AppendCheckItem(id, menu.at(index).text, menu.at(index).help);

            out_menu.Enable(id,enabled);
        }
    }

    // add required items
    for (size_t i=0; i<menu.size(); i++)
    {
        bool required;
        source==CBSvnPluginManager::smMainMenu ? required=menu.at(i).required_main : required=menu.at(i).required_popup;
        int id;
        source==CBSvnPluginManager::smMainMenu ? id=menu.at(i).menu_id : id=menu.at(i).popup_id;

        if (required)
        {
            if (id==-1)
                out_menu.AppendSeparator();
            else if (menu.at(i).kind==MenuEntry::mekNormal)
                out_menu.Append(id, menu.at(i).text, menu.at(i).help);
            else if (menu.at(i).kind==MenuEntry::mekRadio)
                out_menu.AppendRadioItem(id, menu.at(i).text, menu.at(i).help);
            else if (menu.at(i).kind==MenuEntry::mekCheck)
                out_menu.AppendCheckItem(id, menu.at(i).text, menu.at(i).help);
        }
    }
}

//******************************************************************************

void CBTSVN::MakeMenu(const CBSvnPluginManager::SourceMenu source,
                      const std::vector<MenuEntry>& menu,
                      const std::vector<wxString>& items,
                      bool file_based,
                      bool project_based,
                      bool workspace_based,
                      bool under_version_control,
                      wxMenu& out_menu)
{
    std::vector<int> indices;
    ConvertMenuItemsToIndices(menu,items,indices);
    MakeMenu(source,menu,indices,file_based,project_based,workspace_based,under_version_control,out_menu);
}

//******************************************************************************

void CBTSVN::UpdateMenu(const CBSvnPluginManager::SourceMenu source,
                        const std::vector<MenuEntry>& menu,
                        const std::vector<wxString>& items,
                        bool file_based,
                        bool project_based,
                        bool workspace_based,
                        bool under_version_control,
                        wxMenu* in_menu)
{
    std::vector<int> indices;
    ConvertMenuItemsToIndices(menu,items,indices);
    UpdateMenu(source, menu, indices, file_based, project_based, workspace_based, under_version_control, in_menu);
}

//******************************************************************************

void CBTSVN::UpdateMenu(const CBSvnPluginManager::SourceMenu source,
                        const std::vector<MenuEntry>& menu,
                        const std::vector<int> indices,
                        bool file_based,
                        bool project_based,
                        bool workspace_based,
                        bool under_version_control,
                        wxMenu* in_menu)
{
    // add variable menu items
    for (size_t i=0; i<indices.size(); i++)
    {
        size_t index = indices.at(i);

        // prevent out of index
        if ((index<0) || (index > (menu.size()-1)))
            continue;

        int id;
        source==CBSvnPluginManager::smMainMenu ? id=menu.at(index).menu_id : id=menu.at(index).popup_id;

        bool enabled=under_version_control;
        if (!menu.at(index).file_enabled && !menu.at(index).project_enabled && !menu.at(index).workspace_enabled)
            enabled=true;

        if (in_menu)
            in_menu->Enable(id,enabled);
    }
}

//******************************************************************************
// End of file
