#include <sdk.h> // Code::Blocks SDK

#include <cbproject.h>
#include <configmanager.h>
#include <manager.h>
#include <projectmanager.h>
#include <uservarmanager.h>

#include "configurationpanel.h"
#include "cbNSIS.h"
#include "NSISGUIMain.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
PluginRegistrant<cbNSIS> reg(_T("cbNSIS"));
const int ID_NSIS_NEW = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(cbNSIS, cbPlugin)
    EVT_MENU(ID_NSIS_NEW, cbNSIS::OnNew)
END_EVENT_TABLE()

// constructor
cbNSIS::cbNSIS()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("cbNSIS.zip")))
    {
        NotifyMissingFile(_T("cbNSIS.zip"));
    }
}

// destructor
cbNSIS::~cbNSIS()
{
}

void cbNSIS::OnAttach()
{
    Manager * pm = Manager::Get();
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<cbNSIS, CodeBlocksEvent>(this, &cbNSIS::OnRemNsi));
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void cbNSIS::OnRelease(bool appShutDown)
{
    Disconnect(wxEVT_COMMAND_MENU_SELECTED, ID_NSIS_NEW, wxCommandEventHandler(cbNSIS::OnNew));
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}


void cbNSIS::BuildMenu(wxMenuBar * menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    //NotImplemented(_T("cbNSIS::BuildMenu()"));
    int pos = menuBar->FindMenu(_("&File"));
    wxMenu * FileMenu = menuBar->GetMenu(pos);
    pos = FileMenu->FindItem(_("New"));
    wxMenuItem * menuitm = FileMenu->FindItem(pos);
    wxMenu * NewMenu = menuitm->GetSubMenu();
    NewMenu->Append(ID_NSIS_NEW, _("Nullsoft Installer"));
}

void cbNSIS::OnNew(wxCommandEvent & event)
{
    NSISGUIFrame * frm = new NSISGUIFrame(Manager::Get()->GetAppWindow());
    ConfigManager * pCfg = Manager::Get()->GetConfigManager(_T("gcv"));

    if (!pCfg->Exists(_T("/sets/default/nsis/base")))
    {
        Manager::Get()->GetUserVariableManager()->Replace(_T("nsis"));
    }

    if (Manager::Get()->GetProjectManager()->GetActiveProject())
    {
        frm->SetPaths(Manager::Get()->GetProjectManager()->GetActiveProject()->GetCommonTopLevelPath(), pCfg->Read(_T("/sets/default/nsis/base")));

        if (wxID_YES == cbMessageBox(_("Do you want to add the nsi file to the active project?"), _("New Nullsoft Installer"), wxYES | wxNO))
        {
            frm->AddToProject(true);
        }
        else
        {
            frm->AddToProject(false);
        }
    }
    else
    {
        frm->SetPaths(ConfigManager::GetFolder(sdBase), pCfg->Read(_T("/sets/default/nsis/base")));
        frm->AddToProject(false);
    }

    frm->ShowModal();
    wxDELETE(frm);
}

void cbNSIS::OnRemNsi(CodeBlocksEvent & event)
{
    wxFileName * fn = new wxFileName(event.GetString());

    if (fn->GetExt() == _T(".nsi"))
    {
        wxArrayString ar = event.GetProject()->GetCommandsAfterBuild();

        for (unsigned int i = 0; i < ar.GetCount(); i++)
        {
            if (ar[i].Find(fn->GetFullName()) != wxNOT_FOUND)
            {
                event.GetProject()->RemoveCommandsAfterBuild(ar[i]);
            }
        }
    }

    event.Skip();
}
