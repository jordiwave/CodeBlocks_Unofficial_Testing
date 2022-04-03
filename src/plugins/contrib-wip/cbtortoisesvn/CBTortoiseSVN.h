//******************************************************************************
//* Name:      CBTortoiseSVN.h
//* Purpose:   Code::Blocks plugin using TortoiseSVN commandline interface
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef _CBTORTOISESVN_H
#define _CBTORTOISESVN_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include <map>
#include <vector>
#include "CBSvnPluginManager.h"
#include "menu.h"

//******************************************************************************

class CBTortoiseSVN : public cbPlugin
{
    public:
        void BuildMenu(wxMenuBar * menuBar);
        void RemoveMenu(wxMenuBar * menuBar);
        void OnAttach(); // fires when the plugin is attached to the application
        void OnRelease(bool appShutDown); // fires when the plugin is released from the application

    private:
        // events
        void OnUpdateUI(wxUpdateUIEvent & event);

        //************************************************

        void OnAdd(wxCommandEvent & event);
        void OnRemove(wxCommandEvent & event);
        void OnCommit(wxCommandEvent & event);
        void OnDiffWithBase(wxCommandEvent & event);
        void OnLog(wxCommandEvent & event);
        void OnRepobrowser(wxCommandEvent & event);
        void OnRevert(wxCommandEvent & event);
        void OnRevisiongraph(wxCommandEvent & event);
        void OnLock(wxCommandEvent & event);
        void OnUnLock(wxCommandEvent & event);
        void OnProperties(wxCommandEvent & event);
        void OnSettings(wxCommandEvent & event);
        void OnCheckForModifications(wxCommandEvent & event);
        void OnUpdate(wxCommandEvent & event);
        void OnFileProjectWorkspaceBased(wxCommandEvent & event);
        void OnBlame(wxCommandEvent & event);
        void OnMakeReadonly(wxCommandEvent & event);
        void OnMakeReadWrite(wxCommandEvent & event);
        void OnExplore(wxCommandEvent & event);
        void OnConfiguration(wxCommandEvent & event);
        void OnCustomConfig(wxCommandEvent & event);
        void OnAbout(wxCommandEvent & event);
        void OnCheckout(wxCommandEvent & event);
        void OnCleanup(wxCommandEvent & event);
        void OnMerge(wxCommandEvent & event);
        void OnConflictEditor(wxCommandEvent & event);

        //************************************************

        void OnPopupAdd(wxCommandEvent & event);
        void OnPopupRemove(wxCommandEvent & event);
        void OnPopupProperties(wxCommandEvent & event);
        void OnPopupCommit(wxCommandEvent & event);
        void OnPopupDiffWithBase(wxCommandEvent & event);
        void OnPopupLog(wxCommandEvent & event);
        void OnPopupRepobrowser(wxCommandEvent & event);
        void OnPopupRevert(wxCommandEvent & event);
        void OnPopupRevisiongraph(wxCommandEvent & event);
        void OnPopupLock(wxCommandEvent & event);
        void OnPopupUnLock(wxCommandEvent & event);
        void OnPopupCheckForModifications(wxCommandEvent & event);
        void OnPopupBlame(wxCommandEvent & event);
        void OnPopupUpdate(wxCommandEvent & event);
        void OnPopupExplore(wxCommandEvent & event);
        void OnPopupCleanup(wxCommandEvent & event);
        void OnPopupMerge(wxCommandEvent & event);
        void OnPopupConflictEditor(wxCommandEvent & event);

        //************************************************

        void Add(const CBTSVN::IMenuCmd & menu);
        void Remove(const CBTSVN::IMenuCmd & menu);
        void Commit(const CBTSVN::IMenuCmd & menu);
        void Cleanup(const CBTSVN::IMenuCmd & menu);
        void Properties(const CBTSVN::IMenuCmd & menu);
        void DiffWithBase(const CBTSVN::IMenuCmd & menu);
        void Log(const CBTSVN::IMenuCmd & menu);
        void Repobrowser(const CBTSVN::IMenuCmd & menu);
        void Revert(const CBTSVN::IMenuCmd & menu);
        void Revisiongraph(const CBTSVN::IMenuCmd & menu);
        void Lock(const CBTSVN::IMenuCmd & menu);
        void UnLock(const CBTSVN::IMenuCmd & menu);
        void CheckForModifications(const CBTSVN::IMenuCmd & menu);
        void Blame(const CBTSVN::IMenuCmd & menu);
        void Update(const CBTSVN::IMenuCmd & menu);
        void Explore(const CBTSVN::IMenuCmd & menu);
        void Checkout(const CBTSVN::IMenuCmd & menu);
        void Merge(const CBTSVN::IMenuCmd & menu);
        void ConflictEditor(const CBTSVN::IMenuCmd & menu);

        //************************************************

        void BuildMenuEntryList();
        void RebuildMainMenu();

        void MakeReadonly(LPCTSTR lpFileName);
        void MakeReadWrite(LPCTSTR lpFileName);
    private:
        std::vector<CBTSVN::MenuEntry> m_menu;
        std::vector<wxString> m_popup_menu;
        wxString m_previous_mainmenu;

        std::map<wxString, int> m_cache; //for increased performance
        void BuildModuleMenu(const ModuleType type, wxMenu * menu, const FileTreeData * data = 0);
        bool BuildToolBar(wxToolBar * toolBar)
        {
            return false;
        }
        void RemoveToolBar(wxToolBar * toolBar) {}
        bool GetFileUnderVersionControl(CBTSVN::CBSvnPluginManager::SourceMenu menu, const wxString & filename);
        DECLARE_EVENT_TABLE();
};

#endif // _CBTORTOISESVN_H

