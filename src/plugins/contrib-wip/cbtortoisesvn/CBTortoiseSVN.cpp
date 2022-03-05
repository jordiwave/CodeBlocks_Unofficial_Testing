//******************************************************************************
//* Name:      CBTortoiseSVN.cpp
//* Purpose:   Code::Blocks plugin using TortoiseSVN commandline interface
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#include "CBTortoiseSVN.h"
#include "ConfigDialog.h"
#include "TwoPaneSelectionDialog.h"
#include "CustomConfigDialog.h"

#include <cbeditor.h>
#include <cbproject.h>
#include <editormanager.h>

using namespace CBTSVN;

//******************************************************************************

// Main menu
static int idCBTortoiseSVN         = wxNewId();
static int idAdd                   = wxNewId();
static int idRemove                = wxNewId();
static int idCommit                = wxNewId();
static int idDiffWithBase          = wxNewId();
static int idLog                   = wxNewId();
static int idRepobrowser           = wxNewId();
static int idRevert                = wxNewId();
static int idRevisiongraph         = wxNewId();
static int idLock                  = wxNewId();
static int idUnLock                = wxNewId();
static int idModifications         = wxNewId();
static int idUpdate                = wxNewId();
static int idBlame                 = wxNewId();
static int idProperties            = wxNewId();
static int idSettings              = wxNewId();
static int idMakeReadonly          = wxNewId();
static int idMakeReadWrite         = wxNewId();
static int idAbout                 = wxNewId();
static int idConfiguration         = wxNewId();
static int idCustomConfig          = wxNewId();
static int idExplore               = wxNewId();
static int idCheckout              = wxNewId();
static int idCleanup               = wxNewId();
static int idMerge                 = wxNewId();
static int idConflictEditor        = wxNewId();
static int idFileBased             = wxNewId();
static int idProjectBased          = wxNewId();
static int idWorkspaceBased        = wxNewId();
static int idUseCustomSettings     = wxNewId();

// popup menu
static int idPopupAdd              = wxNewId();
static int idPopupRemove           = wxNewId();
static int idPopupCommit           = wxNewId();
static int idPopupProperties       = wxNewId();
static int idPopupDiffWithBase     = wxNewId();
static int idPopupLog              = wxNewId();
static int idPopupRepobrowser      = wxNewId();
static int idPopupRevert           = wxNewId();
static int idPopupRevisiongraph    = wxNewId();
static int idPopupLock             = wxNewId();
static int idPopupUnLock           = wxNewId();
static int idPopupModifications    = wxNewId();
static int idPopupBlame            = wxNewId();
static int idPopupUpdate           = wxNewId();
static int idPopupExplore          = wxNewId();
static int idPopupCleanup          = wxNewId();
static int idPopupMerge            = wxNewId();
static int idPopupConflictEditor   = wxNewId();

//******************************************************************************

// Implement the plugin's hooks
//CB_IMPLEMENT_PLUGIN(CBTortoiseSVN, "CBTortoiseSVN");
// Register the plugin

namespace
{
PluginRegistrant<CBTortoiseSVN> reg(_T("CBTortoiseSVN"));
};

BEGIN_EVENT_TABLE(CBTortoiseSVN, cbPlugin)
    EVT_MENU(idAdd,                   CBTortoiseSVN::OnAdd)
    EVT_MENU(idRemove,                CBTortoiseSVN::OnRemove)
    EVT_MENU(idCommit,                CBTortoiseSVN::OnCommit)
    EVT_MENU(idDiffWithBase,          CBTortoiseSVN::OnDiffWithBase)
    EVT_MENU(idLog,                   CBTortoiseSVN::OnLog)
    EVT_MENU(idRepobrowser,           CBTortoiseSVN::OnRepobrowser)
    EVT_MENU(idRevert,                CBTortoiseSVN::OnRevert)
    EVT_MENU(idRevisiongraph,         CBTortoiseSVN::OnRevisiongraph)
    EVT_MENU(idLock,                  CBTortoiseSVN::OnLock)
    EVT_MENU(idUnLock,                CBTortoiseSVN::OnUnLock)
    EVT_MENU(idModifications,         CBTortoiseSVN::OnCheckForModifications)
    EVT_MENU(idUpdate,                CBTortoiseSVN::OnUpdate)
    EVT_MENU(idBlame,                 CBTortoiseSVN::OnBlame)
    EVT_MENU(idProperties,            CBTortoiseSVN::OnProperties)
    EVT_MENU(idExplore,               CBTortoiseSVN::OnExplore)
    EVT_MENU(idSettings,              CBTortoiseSVN::OnSettings)
    EVT_MENU(idMakeReadonly,          CBTortoiseSVN::OnMakeReadonly)
    EVT_MENU(idMakeReadWrite,         CBTortoiseSVN::OnMakeReadWrite)
    EVT_MENU(idAbout,                 CBTortoiseSVN::OnAbout)
    EVT_MENU(idConfiguration,         CBTortoiseSVN::OnConfiguration)
    EVT_MENU(idCustomConfig,          CBTortoiseSVN::OnCustomConfig)
    EVT_MENU(idFileBased,             CBTortoiseSVN::OnFileProjectWorkspaceBased)
    EVT_MENU(idProjectBased,          CBTortoiseSVN::OnFileProjectWorkspaceBased)
    EVT_MENU(idUseCustomSettings,     CBTortoiseSVN::OnFileProjectWorkspaceBased)
    EVT_MENU(idWorkspaceBased,        CBTortoiseSVN::OnFileProjectWorkspaceBased)
    EVT_MENU(idCheckout,              CBTortoiseSVN::OnCheckout)
    EVT_MENU(idCleanup,               CBTortoiseSVN::OnCleanup)
    EVT_MENU(idMerge,                 CBTortoiseSVN::OnMerge)
    EVT_MENU(idConflictEditor,        CBTortoiseSVN::OnConflictEditor)

    EVT_MENU(idPopupAdd,              CBTortoiseSVN::OnPopupAdd)
    EVT_MENU(idPopupRemove,           CBTortoiseSVN::OnPopupRemove)
    EVT_MENU(idPopupProperties,       CBTortoiseSVN::OnPopupProperties)
    EVT_MENU(idPopupCommit,           CBTortoiseSVN::OnPopupCommit)
    EVT_MENU(idPopupProperties,       CBTortoiseSVN::OnPopupProperties)
    EVT_MENU(idPopupDiffWithBase,     CBTortoiseSVN::OnPopupDiffWithBase)
    EVT_MENU(idPopupLog,              CBTortoiseSVN::OnPopupLog)
    EVT_MENU(idPopupRepobrowser,      CBTortoiseSVN::OnPopupRepobrowser)
    EVT_MENU(idPopupRevert,           CBTortoiseSVN::OnPopupRevert)
    EVT_MENU(idPopupRevisiongraph,    CBTortoiseSVN::OnPopupRevisiongraph)
    EVT_MENU(idPopupLock,             CBTortoiseSVN::OnPopupLock)
    EVT_MENU(idPopupUnLock,           CBTortoiseSVN::OnPopupUnLock)
    EVT_MENU(idPopupModifications,    CBTortoiseSVN::OnPopupCheckForModifications)
    EVT_MENU(idPopupBlame,            CBTortoiseSVN::OnPopupBlame)
    EVT_MENU(idPopupUpdate,           CBTortoiseSVN::OnPopupUpdate)
    EVT_MENU(idPopupExplore,          CBTortoiseSVN::OnPopupExplore)
    EVT_MENU(idPopupCleanup,          CBTortoiseSVN::OnPopupCleanup)
    EVT_MENU(idPopupMerge,            CBTortoiseSVN::OnPopupMerge)
    EVT_MENU(idPopupConflictEditor,   CBTortoiseSVN::OnPopupConflictEditor)

    EVT_UPDATE_UI(idAdd,                CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idCommit,             CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idDiffWithBase,       CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idLog,                CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idRepobrowser,        CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idRevert,             CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idRevisiongraph,      CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idLock,               CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idUnLock,             CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idModifications,      CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idUpdate,             CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idBlame,              CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idProperties,         CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idExplore,            CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idSettings,           CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idMakeReadonly,       CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idMakeReadWrite,      CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idAbout,              CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idConfiguration,      CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idFileBased,          CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idProjectBased,       CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idUseCustomSettings,  CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idWorkspaceBased,     CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idCheckout,           CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idCleanup,            CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idMerge,              CBTortoiseSVN::OnUpdateUI)
    EVT_UPDATE_UI(idConflictEditor,     CBTortoiseSVN::OnUpdateUI)

END_EVENT_TABLE()

//******************************************************************************

void CBTortoiseSVN::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    CBSvnPluginManager::GetInstance().Initialise();
    m_previous_mainmenu=_("no menu was generated");
    BuildMenuEntryList();
}

//******************************************************************************

void CBTortoiseSVN::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
    CBSvnPluginManager::GetInstance().Shutdown();
}

//******************************************************************************

bool CBTortoiseSVN::GetFileUnderVersionControl(CBSvnPluginManager::SourceMenu menu, const wxString& filename)
{
    if (filename==_(""))
        return false;

    if (CBSvnPluginManager::GetInstance().GetMaxIntegrationPerformance())
        return true;

    bool result;
    if (m_cache.find(filename)==m_cache.end())
    {
        result=CBSvnPluginManager::GetInstance().FileUnderVersionControl(filename);
        m_cache[filename]=result;
    }
    else
        result=m_cache[filename];
    return result;
}

//******************************************************************************

void CBTortoiseSVN::OnAdd(wxCommandEvent &event)
{
    Add(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupAdd(wxCommandEvent &event)
{
    Add(POPUP_MENU);
}

void CBTortoiseSVN::OnRemove(wxCommandEvent &event)
{
    Remove(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupRemove(wxCommandEvent &event)
{
    Remove(POPUP_MENU);
}

void CBTortoiseSVN::OnCommit(wxCommandEvent &event)
{
    Commit(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupCommit(wxCommandEvent &event)
{
    Commit(POPUP_MENU);
}

void CBTortoiseSVN::OnDiffWithBase(wxCommandEvent &event)
{
    DiffWithBase(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupDiffWithBase(wxCommandEvent &event)
{
    DiffWithBase(POPUP_MENU);
}

void CBTortoiseSVN::OnLog(wxCommandEvent &event)
{
    Log(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupLog(wxCommandEvent &event)
{
    Log(POPUP_MENU);
}

void CBTortoiseSVN::OnRepobrowser(wxCommandEvent &event)
{
    Repobrowser(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupRepobrowser(wxCommandEvent &event)
{
    Repobrowser(POPUP_MENU);
}

void CBTortoiseSVN::OnRevert(wxCommandEvent &event)
{
    Revert(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupRevert(wxCommandEvent &event)
{
    Revert(POPUP_MENU);
}

void CBTortoiseSVN::OnRevisiongraph(wxCommandEvent &event)
{
    Revisiongraph(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupRevisiongraph(wxCommandEvent &event)
{
    Revisiongraph(POPUP_MENU);
}

void CBTortoiseSVN::OnLock(wxCommandEvent &event)
{
    Lock(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupLock(wxCommandEvent &event)
{
    Lock(POPUP_MENU);
}

void CBTortoiseSVN::OnUnLock(wxCommandEvent &event)
{
    UnLock(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupUnLock(wxCommandEvent &event)
{
    UnLock(POPUP_MENU);
}

void CBTortoiseSVN::OnCheckForModifications(wxCommandEvent &event)
{
    CheckForModifications(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupCheckForModifications(wxCommandEvent &event)
{
    CheckForModifications(POPUP_MENU);
}

void CBTortoiseSVN::OnBlame(wxCommandEvent &event)
{
    Blame(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupBlame(wxCommandEvent &event)
{
    Blame(POPUP_MENU);
}

void CBTortoiseSVN::OnUpdate(wxCommandEvent &event)
{
    Update(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupUpdate(wxCommandEvent &event)
{
    Update(POPUP_MENU);
}

void CBTortoiseSVN::OnProperties(wxCommandEvent &event)
{
    Properties(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupProperties(wxCommandEvent &event)
{
    Properties(POPUP_MENU);
}

void CBTortoiseSVN::OnExplore(wxCommandEvent &event)
{
    Explore(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupExplore(wxCommandEvent &event)
{
    Explore(POPUP_MENU);
}

void CBTortoiseSVN::OnCleanup(wxCommandEvent &event)
{
    Cleanup(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupCleanup(wxCommandEvent &event)
{
    Cleanup(POPUP_MENU);
}

void CBTortoiseSVN::OnMerge(wxCommandEvent &event)
{
    Merge(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupMerge(wxCommandEvent &event)
{
    Merge(POPUP_MENU);
}

void CBTortoiseSVN::OnConflictEditor(wxCommandEvent &event)
{
    ConflictEditor(MAIN_MENU);
}
void CBTortoiseSVN::OnPopupConflictEditor(wxCommandEvent &event)
{
    ConflictEditor(POPUP_MENU);
}

void CBTortoiseSVN::OnCheckout(wxCommandEvent &event)
{
    Checkout(MAIN_MENU);
}

//******************************************************************************

void CBTortoiseSVN::BuildMenu(wxMenuBar *menuBar)
{
    if (!CBSvnPluginManager::GetInstance().GetMainMenuIntegration())
        return;

    MAIN_MENU.SetFileBased(true);

    wxMenu* submenu = new wxMenu;
    std::vector<int> indices = convert(CBSvnPluginManager::GetInstance().GetMainMenu());
    MakeMenu(CBSvnPluginManager::smMainMenu,
             m_menu,
             indices,
             MAIN_MENU.GetFileBased(),
             MAIN_MENU.GetProjectBased(),
             MAIN_MENU.GetWorkspaceBased(),
             true,
             *submenu);

    int ToolsPos = menuBar->FindMenu(_("&Tools"));

    if (ToolsPos == wxNOT_FOUND)
        menuBar->Append(submenu,_("TortoiseSVN"));
    else
        menuBar->Insert(ToolsPos,submenu,_("TortoiseSVN"));

    submenu->FindItem(idUseCustomSettings)->Enable(!MAIN_MENU.GetFileBased());
    submenu->FindItem(idCustomConfig)->Enable(!MAIN_MENU.GetFileBased());

    m_previous_mainmenu=CBSvnPluginManager::GetInstance().GetMainMenu();
}

//******************************************************************************

void CBTortoiseSVN::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    if (!IsAttached())
        return;

    bool file_based=true;
    bool project_based=false;

    if (type==mtEditorManager)
    {
        if (!CBSvnPluginManager::GetInstance().GetEditorIntegration())
            return;

        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (!ed)
            return;

        POPUP_MENU.SetFileBased(true);
        POPUP_MENU.SetProjectBased(false);
        POPUP_MENU.SetWorkspaceBased(false);
        POPUP_MENU.SetFilename(ed->GetFilename());
    }
    else if ((type==mtProjectManager) && menu)
    {
        if (!CBSvnPluginManager::GetInstance().GetProjectManagerIntegration())
            return;

        project_based  = (data && data->GetKind() == FileTreeData::ftdkProject);
        file_based     = (data && data->GetKind() == FileTreeData::ftdkFile);

        if (!(project_based || file_based))
            return;

        if (project_based)
        {
            // popup menu on a project
            POPUP_MENU.SetFileBased(false);
            POPUP_MENU.SetProjectBased(true);
            POPUP_MENU.SetWorkspaceBased(false);
            POPUP_MENU.SetFilename(data->GetProject()->GetFilename());
        }
        else if (file_based)
        {
            // popup menu on a file
            POPUP_MENU.SetFileBased(true);
            POPUP_MENU.SetProjectBased(false);
            POPUP_MENU.SetWorkspaceBased(false);
            POPUP_MENU.SetFilename(data->GetProjectFile()->file.GetFullPath());
        }
    }
    else return;

    bool SVN_file=GetFileUnderVersionControl(CBSvnPluginManager::smPopupMenu, POPUP_MENU.GetFilename());

    wxMenu* submenu = new wxMenu;
    std::vector<int> indices = convert(CBSvnPluginManager::GetInstance().GetPopupMenu());
    MakeMenu(CBSvnPluginManager::smPopupMenu,
             m_menu,
             indices,
             POPUP_MENU.GetFileBased(),
             POPUP_MENU.GetProjectBased(),
             POPUP_MENU.GetWorkspaceBased(),
             SVN_file,
             *submenu);

    menu->AppendSeparator();
    menu->AppendSubMenu(submenu,_("TortoiseSVN"));
}

//******************************************************************************

void CBTortoiseSVN::RemoveMenu(wxMenuBar *menuBar)
{
    wxMenu *menu = 0;
    wxMenuItem *item = menuBar->FindItem(idCBTortoiseSVN, &menu);

    if (menu && item)
        menu->Remove(item);
}

//******************************************************************************

void CBTortoiseSVN::OnUpdateUI(wxUpdateUIEvent &event)
{
    if (!Manager::IsAppShuttingDown())
    {
        wxWindow* pAppWin  = wxTheApp->GetTopWindow();
        if (pAppWin)
        {
            wxMenuBar *mbar = ((wxFrame*)pAppWin)->GetMenuBar();
            if (mbar)
            {
                mbar->Freeze();

                int plugin_pos;
                if ((plugin_pos = mbar->FindMenu(_("TortoiseSVN")))!=wxNOT_FOUND)
                {
                    wxString filename;
                    if (MAIN_MENU.GetFileBased())
                    {
                        filename=GetEditorFilename();
                    }
                    if (MAIN_MENU.GetProjectBased())
                    {
                        filename=GetProjectFilename();
                    }
                    if (MAIN_MENU.GetWorkspaceBased())
                    {
                        filename=GetWorkspaceFilename();
                    }

                    if (filename!=MAIN_MENU.GetFilename())
                    {
                        MAIN_MENU.SetFilename(filename);
                        LogMenu(MAIN_MENU);
                    }

                    bool SVN_file=GetFileUnderVersionControl(CBSvnPluginManager::smMainMenu, MAIN_MENU.GetFilename());

                    std::vector<int> indices = convert(CBSvnPluginManager::GetInstance().GetMainMenu());
                    wxMenu* submenu = mbar->GetMenu(plugin_pos);
                    UpdateMenu(CBSvnPluginManager::smMainMenu,
                               m_menu,
                               indices,
                               MAIN_MENU.GetFileBased(),
                               MAIN_MENU.GetProjectBased(),
                               MAIN_MENU.GetWorkspaceBased(),
                               SVN_file,
                               submenu);
                }
                mbar->Thaw();
            }
        }
    }
    event.Skip();
}

//******************************************************************************

void CBTortoiseSVN::BuildMenuEntryList()
{
#define ADD(a,b,c,d,e,f,g,h,i,j,k) m_menu.push_back(MenuEntry(a,b,c,d,e,f,g,h,i,j,k))
#define KINDNORM   MenuEntry::mekNormal
#define KINDRADIO  MenuEntry::mekRadio
#define KINDCHECK  MenuEntry::mekCheck

    //                                                           <----- based on ---->  <--required-->
    //  id main menu            id popup              kind       file project workspace  main   popup          name                                 text                             help
    ADD(-1,                     -1,                   KINDNORM,  false, false, false,   true,   false, _("workbased_based"),            _("Workspace based commands"),   _("Workspace based commands"));
    ADD(idFileBased,            -1,                   KINDRADIO, false, false, false,   true,   false, _("file_based"),                 _("File based commands"),        _("File based commands"));
    ADD(idProjectBased,         -1,                   KINDRADIO, false, false, false,   true,   false, _("project_based"),              _("Project based commands"),     _("Project based commands"));
    ADD(idWorkspaceBased,       -1,                   KINDRADIO, false, false, false,   true,   false, _("workbased_based"),            _("Workspace based commands"),   _("Workspace based commands"));
    ADD(idUseCustomSettings,    -1,                   KINDCHECK, false, false, false,   true,   false, _("Use custom settings"),        _("Use custom settings"),        _("Use custom settings"));
    ADD(-1,                     -1,                   KINDNORM,  false, false, false,   true,   true,  _(""),                           _(""),                           _(""));
    ADD(idCustomConfig,         -1,                   KINDNORM,  false, false, false,   true,   false, _("Custom"),                     _("Custom settings..."),         _("Custom settings"));
    ADD(-1,                     -1,                   KINDNORM,  false, false, false,   true,   false, _(""),                           _(""),                           _(""));
    ADD(idConfiguration,        idConfiguration,      KINDNORM,  false, false, false,   true,   true,  _("plugin"),                     _("Plugin settings..."),         _("CBTortoiseSVN settings"));
    ADD(-1,                     -1,                   KINDNORM,  false, false, false,   true,   false, _(""),                           _(""),                           _(""));
    ADD(idAbout,                -1,                   KINDNORM,  true,  true,  true,    true,   false, _("about"),                      _("About..."),                   _("About CBTortoiseSVN"));
    ADD(idAdd,                  idPopupAdd,           KINDNORM,  false, false, false,   false,  false, _("add"),                        _("Add..."),                     _("Add"));
    ADD(idRemove,               idPopupRemove,        KINDNORM,  true,  false, false,   false,  false, _("Remove"),                     _("Remove..."),                  _("Remove"));
    ADD(idCommit,               idPopupCommit,        KINDNORM,  true,  true,  true,    false,  false, _("commit"),                     _("Commit..."),                  _("Commit"));
    ADD(idDiffWithBase,         idPopupDiffWithBase,  KINDNORM,  true,  true,  true,    false,  false, _("diff"),                       _("Diff with base..."),          _("Diff with base"));
    ADD(idLog,                  idPopupLog,           KINDNORM,  true,  true,  true,    false,  false, _("log"),                        _("Log..."),                     _("Log"));
    ADD(idRepobrowser,          idPopupRepobrowser,   KINDNORM,  true,  true,  true,    false,  false, _("repobrowser"),                _("Repobrowser..."),             _("Repobowser"));
    ADD(idRevert,               idPopupRevert,        KINDNORM,  true,  true,  true,    false,  false, _("revert"),                     _("Revert..."),                  _("Revert"));
    ADD(idRevisiongraph,        idPopupRevisiongraph, KINDNORM,  true,  true,  true,    false,  false, _("revisiongraph"),              _("Revisiongraph..."),           _("Revisiongraph"));
    ADD(idLock,                 idPopupLock,          KINDNORM,  true,  true,  true,    false,  false, _("lock"),                       _("Lock..."),                    _("Lock"));
    ADD(idUnLock,               idPopupUnLock,        KINDNORM,  true,  true,  true,    false,  false, _("unLock"),                     _("UnLock..."),                  _("UnLock"));
    ADD(idModifications,        idPopupModifications, KINDNORM,  true,  true,  true,    false,  false, _("modifications"),              _("Check for modifications..."), _("Check for modifications"));
    ADD(idUpdate,               idPopupUpdate,        KINDNORM,  true,  true,  true,    false,  false, _("update"),                     _("Update..."),                  _("Update"));
    ADD(idBlame,                idPopupBlame,         KINDNORM,  true,  false, false,   false,  false, _("blame"),                      _("Blame..."),                   _("Blame"));
    ADD(idProperties,           idPopupProperties,    KINDNORM,  false, false, false,   false,  false, _("properties"),                 _("Properties..."),              _("Properties"));
    ADD(idSettings,             idSettings,           KINDNORM,  false, false, false,   false,  false, _("settings"),                   _("TortoiseSVN Settings..."),    _("TortoiseSVN Settings"));
    ADD(idMakeReadonly,         -1,                   KINDNORM,  true,  false, false,   false,  false, _("readonly"),                   _("Make Readonly..."),           _("Make Readonly"));
    ADD(idMakeReadWrite,        -1,                   KINDNORM,  true,  false, false,   false,  false, _("readwrite"),                  _("Make Read/Write..."),         _("Make Read/Write"));
    ADD(idExplore,              idPopupExplore,       KINDNORM,  true,  true,  true,    false,  false, _("explore"),                    _("Explore..."),                 _("Explore"));
    ADD(idCheckout,             -1,                   KINDNORM,  false, false, false,   false,  false, _("checkout"),                   _("Checkout..."),                _("Checkout"));
    ADD(idCleanup,              idPopupCleanup,       KINDNORM,  false, true,  true,    false,  false, _("cleanup"),                    _("Cleanup..."),                 _("Cleanup"));
    ADD(idMerge,                -1,                   KINDNORM,  true,  true,  true,    false,  false, _("merge"),                      _("Merge..."),                   _("Merge"));
    ADD(idConflictEditor,       -1,                   KINDNORM,  true,  true,  true,    false,  false, _("conflict"),                   _("Conflict editor..."),         _("Conflict editor"));
}

//******************************************************************************

void CBTortoiseSVN::Add(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("add"));
}

//******************************************************************************

void CBTortoiseSVN::Remove(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("remove"));
}

//******************************************************************************

void CBTortoiseSVN::Commit(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu,_("commit"));
}

//******************************************************************************

void CBTortoiseSVN::Cleanup(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu,_("cleanup"));
}

//******************************************************************************

void CBTortoiseSVN::DiffWithBase(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("diff"));
}

//******************************************************************************

void CBTortoiseSVN::Log(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("log"));
}

//******************************************************************************

void CBTortoiseSVN::Repobrowser(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("repobrowser"));
}

//******************************************************************************

void CBTortoiseSVN::Revert(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("revert"));
}

//******************************************************************************

void CBTortoiseSVN::Revisiongraph(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("revisiongraph"));
}

//******************************************************************************

void CBTortoiseSVN::Lock(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("lock"));
}

//******************************************************************************

void CBTortoiseSVN::UnLock(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu,_("unlock"));
}

//******************************************************************************

void CBTortoiseSVN::CheckForModifications(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("repostatus"));
}

//******************************************************************************

void CBTortoiseSVN::Blame(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu,_("blame"));
}

//******************************************************************************

void CBTortoiseSVN::Update(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("update"));
}

//******************************************************************************

void CBTortoiseSVN::Merge(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("merge"));
}

//******************************************************************************

void CBTortoiseSVN::ConflictEditor(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _("conflicteditor"));
}

//******************************************************************************

void CBTortoiseSVN::Checkout(const IMenuCmd& menu)
{
    CBSvnPluginManager::GetInstance().RunSimpleTortoiseSVNCommand(menu, _(""), _("/command:checkout"));
}

//******************************************************************************

void CBTortoiseSVN::Properties(const IMenuCmd& menu)
{
    wxString filename=menu.GetFilename();

    if (filename!=_(""))
    {
        if (menu.GetProjectBased()||
                menu.GetWorkspaceBased())
            filename=CBTSVN::GetBaseDir(filename);

        SHELLEXECUTEINFO sei;

        ZeroMemory(&sei,sizeof(sei));
        sei.cbSize = sizeof(sei);
        sei.lpFile = filename.c_str();
        sei.lpVerb = _("properties");
        sei.fMask  = SEE_MASK_INVOKEIDLIST;
        ShellExecuteEx(&sei);
    }
}

//******************************************************************************

void CBTortoiseSVN::Explore(const IMenuCmd& menu)
{
    wxString filename=menu.GetFilename();

    if (filename!=_(""))
    {
        if (menu.GetProjectBased()||
                menu.GetWorkspaceBased())
            filename=CBTSVN::GetBaseDir(filename);

        unsigned long exit_code;
        wxString commandline = _("explorer.exe ") + filename;

        CBTSVN::Logger::GetInstance().log(_("Executing: ") + commandline);

        Run(false,false,commandline,exit_code);
    }
}

//******************************************************************************

void CBTortoiseSVN::OnSettings(wxCommandEvent &event)
{
    DWORD exit_code;
    wxString commandline = _("\"") + CBSvnPluginManager::GetInstance().GetTortoiseproc() + _("\"") + _(" /command:settings");
    CBTSVN::Logger::GetInstance().log(_("Executing: ") + commandline);
    if (!Run(false, false, commandline, exit_code))
        wxMessageBox(_("Command \"") + commandline + _("\" failed"), _("Info"),
                     wxOK | wxICON_ERROR);
}

//******************************************************************************

void CBTortoiseSVN::OnMakeReadonly(wxCommandEvent &event)
{
    wxString filename=MAIN_MENU.GetFilename();

    if (filename!=_(""))
        MakeReadonly(filename.c_str());
}

//******************************************************************************

void CBTortoiseSVN::OnMakeReadWrite(wxCommandEvent &event)
{
    wxString filename=MAIN_MENU.GetFilename();

    if (filename!=_(""))
    {
        MakeReadWrite(filename.c_str());
    }
}

//******************************************************************************

void CBTortoiseSVN::OnConfiguration(wxCommandEvent &event)
{
    ConfigDialog dlg(m_menu, NULL);
    dlg.ShowModal();

    if (m_previous_mainmenu!=CBSvnPluginManager::GetInstance().GetMainMenu())
        RebuildMainMenu();
}

//******************************************************************************

void CBTortoiseSVN::OnCustomConfig(wxCommandEvent &event)
{
    CustomConfigDialog dlg(NULL);
    dlg.ShowModal();
}

//******************************************************************************

void CBTortoiseSVN::RebuildMainMenu()
{
    m_previous_mainmenu=CBSvnPluginManager::GetInstance().GetMainMenu();
    if (Manager::IsAppShuttingDown())
        return;

    wxWindow* pAppWin  = wxTheApp->GetTopWindow();
    if (!pAppWin)
        return;

    wxMenuBar *mbar = ((wxFrame*)pAppWin)->GetMenuBar();
    if (!mbar)
        return;

    int plugin_pos;
    if ((plugin_pos = mbar->FindMenu(_("TortoiseSVN")))==wxNOT_FOUND)
        return;

    std::vector<int> indices = convert(CBSvnPluginManager::GetInstance().GetMainMenu());
    wxMenu* submenu = new wxMenu;

    MakeMenu(CBSvnPluginManager::smMainMenu,
             m_menu,
             indices,
             MAIN_MENU.GetFileBased(),
             MAIN_MENU.GetProjectBased(),
             MAIN_MENU.GetWorkspaceBased(),
             true,
             *submenu);

    submenu->Check(idFileBased,MAIN_MENU.GetFileBased());
    submenu->Check(idProjectBased,MAIN_MENU.GetProjectBased());
    submenu->Check(idWorkspaceBased,MAIN_MENU.GetWorkspaceBased());

    wxMenu* old_menu= mbar->Replace(plugin_pos,submenu,_("TortoiseSVN"));

    if (old_menu)
        delete old_menu;
}

//******************************************************************************

void CBTortoiseSVN::OnFileProjectWorkspaceBased(wxCommandEvent &event)
{
    if (Manager::IsAppShuttingDown())
        return;

    wxWindow* pAppWin  = wxTheApp->GetTopWindow();
    if (!pAppWin)
        return;

    wxMenuBar *mbar = ((wxFrame*)pAppWin)->GetMenuBar();
    if (!mbar)
        return;

    MAIN_MENU.SetFileBased(mbar->IsChecked(idFileBased));
    MAIN_MENU.SetProjectBased(mbar->IsChecked(idProjectBased));
    MAIN_MENU.SetWorkspaceBased(mbar->IsChecked(idWorkspaceBased));
    MAIN_MENU.SetCustom(mbar->IsChecked(idUseCustomSettings));
    POPUP_MENU.SetCustom(mbar->IsChecked(idUseCustomSettings));

    bool filebased=mbar->IsChecked(idFileBased);
    mbar->FindItem(idUseCustomSettings)->Enable(!filebased);
    mbar->FindItem(idCustomConfig)->Enable(!filebased);

    LogMenu(MAIN_MENU);
}

//******************************************************************************

void CBTortoiseSVN::OnAbout(wxCommandEvent &event)
{
    wxString msg;
    msg+= _("SVN interface for Codeblocks based on TortoiseSVN\r\n\r\n");
    msg+= _("Hosted at http://code.google.com/p/cbtortoisesvn/\r\n\r\n");
    msg+= _("By Jan van den Borst");
    wxMessageBox(msg, _("About..."), wxOK|wxICON_INFORMATION);
}

//******************************************************************************

void CBTortoiseSVN::MakeReadonly(LPCTSTR lpFileName)
{
    DWORD dwFileAttributes = GetFileAttributes(lpFileName);

    long mask = dwFileAttributes;

    if (dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        return;

    mask = dwFileAttributes | FILE_ATTRIBUTE_READONLY;
    SetFileAttributes(lpFileName, mask);
}

//******************************************************************************

void CBTortoiseSVN::MakeReadWrite(LPCTSTR lpFileName)
{
    DWORD dwFileAttributes = GetFileAttributes(lpFileName);

    if (!(dwFileAttributes & FILE_ATTRIBUTE_READONLY))
        return;

    long mask = dwFileAttributes & (~FILE_ATTRIBUTE_READONLY);
    SetFileAttributes(lpFileName, mask);
}

//******************************************************************************
// End of file
