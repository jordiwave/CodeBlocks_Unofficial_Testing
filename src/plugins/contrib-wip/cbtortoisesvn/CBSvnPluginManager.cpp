//******************************************************************************
//* Name:      CBSvnPluginManager.cpp
//* Purpose:   Plugin manager object (singleton)
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************


#include "CBSvnPluginManager.h"
#include "LogManager.h"


using namespace CBTSVN;

//******************************************************************************

class MenuCmd : public IMenuCmd
{
public:
    MenuCmd():m_FileBased(true), m_ProjectBased(false), m_WorkspaceBased(false),m_custom(false) {}
    bool GetFileBased() const
    {
        return m_FileBased;
    }
    void SetFileBased(bool FileBased)
    {
        m_FileBased=FileBased;
    }
    bool GetProjectBased() const
    {
        return m_ProjectBased;
    }
    void SetProjectBased(bool ProjectBased)
    {
        m_ProjectBased=ProjectBased;
    }
    bool GetWorkspaceBased() const
    {
        return m_WorkspaceBased;
    }
    void SetWorkspaceBased(bool ProjectBased)
    {
        m_WorkspaceBased=ProjectBased;
    }
    void SetCustom(bool custom)
    {
        m_custom=custom;
    }
    bool GetCustom()const
    {
        return m_custom;
    }
    wxString GetFilename() const
    {
        return m_filename;
    }
    void SetFilename(const wxString& filename)
    {
        m_filename=filename;
    }
private:
    wxString m_filename;
    bool m_FileBased;
    bool m_ProjectBased;
    bool m_WorkspaceBased;
    bool m_custom;
};

//******************************************************************************

void CBTSVN::LogMenu(const IMenuCmd& menu)
{
    wxString log;
    log <<
        _("Filename: ") << menu.GetFilename() <<
        _(", Filebased: ") << wxString::Format(_("%d"),menu.GetFileBased()) <<
        _(", Projectbased: ") << wxString::Format(_("%d"),menu.GetProjectBased()) <<
        _(", Workspacebased: ") << wxString::Format(_("%d"),menu.GetWorkspaceBased()) <<
        _(", Custom: ") << wxString::Format(_("%d"),menu.GetCustom());
    Logger::GetInstance().log(log);
};

//******************************************************************************

CBSvnPluginManager::CBSvnPluginManager() :
    m_debug(false)
{
    wxString value;

    m_svn=_("svn.exe");
    if (ReadStringFromGlobalInifile(plugin_name,svnpath,value))
        m_svn=value;

    m_tortoisesvnpath=_("tortoiseproc.exe");
    if (ReadStringFromGlobalInifile(plugin_name, tortoisesvnpath,value))
        m_tortoisesvnpath=value;

    m_MainMenuIntegration=true;
    if (ReadStringFromGlobalInifile(plugin_name, mainmenuintegration,value))
        m_MainMenuIntegration=(value==_("1"));

    m_EditorIntegration=false;
    if (ReadStringFromGlobalInifile(plugin_name, editorintegration,value))
        m_EditorIntegration=(value==_("1"));

    m_ProjectManagerIntegration=false;
    if (ReadStringFromGlobalInifile(plugin_name, projectmanagerintegration,value))
        m_ProjectManagerIntegration=(value==_("1"));

    m_MaxIntegrationPerformance=true;
    if (ReadStringFromGlobalInifile(plugin_name, maxintegrationperformance,value))
        m_MaxIntegrationPerformance=(value==_("1"));

    if (ReadStringFromGlobalInifile(plugin_name, mainmenu, value))
        m_mainmenu=value;

    if (ReadStringFromGlobalInifile(plugin_name, popupmenu, value))
        m_popupmenu=value;

    for (int i=0; i< smNumSourceMenu; ++i)
        menus[i].reset(new MenuCmd);
}

//******************************************************************************

CBSvnPluginManager& CBSvnPluginManager::GetInstance()
{
    static CBSvnPluginManager the_plugin;
    return the_plugin;
}

//******************************************************************************

wxString CBSvnPluginManager::GetSvn() const
{
    return m_svn;
}

//******************************************************************************

void CBSvnPluginManager::SetSvn(const wxString& svn)
{
    if (WriteStringToGlobalInifile(plugin_name, svnpath,svn))
        m_svn=svn;
}

//******************************************************************************

wxString CBSvnPluginManager::GetTortoiseproc() const
{
    return m_tortoisesvnpath;
}

//******************************************************************************

void CBSvnPluginManager::SetTortoiseproc(const wxString& tortoiseproc)
{
    if (WriteStringToGlobalInifile(plugin_name,tortoisesvnpath,tortoiseproc))
        m_tortoisesvnpath=tortoiseproc;
}

//******************************************************************************

bool CBSvnPluginManager::GetMainMenuIntegration() const
{
    return m_MainMenuIntegration;
}

//******************************************************************************

void CBSvnPluginManager::SetMainMenuIntegration(bool enabled)
{
    if (WriteStringToGlobalInifile(plugin_name, mainmenuintegration, wxString::Format(_("%d"),enabled)))
        m_MainMenuIntegration=enabled;
}

//******************************************************************************

bool CBSvnPluginManager::GetEditorIntegration() const
{
    return m_EditorIntegration;
}

//******************************************************************************

void CBSvnPluginManager::SetEditorIntegration(bool enabled)
{
    if (WriteStringToGlobalInifile(plugin_name, editorintegration, wxString::Format(_("%d"),enabled)))
        m_EditorIntegration=enabled;
}

//******************************************************************************

bool CBSvnPluginManager::GetProjectManagerIntegration() const
{
    return m_ProjectManagerIntegration;
}

//******************************************************************************

void CBSvnPluginManager::SetProjectManagerIntegration(bool enabled)
{
    if (WriteStringToGlobalInifile(plugin_name, projectmanagerintegration, wxString::Format(_("%d"),enabled)))
        m_ProjectManagerIntegration=enabled;
}

//******************************************************************************

bool CBSvnPluginManager::GetMaxIntegrationPerformance() const
{
    return m_MaxIntegrationPerformance;
}

//******************************************************************************

void CBSvnPluginManager::SetMaxIntegrationPerformance(bool max)
{
    if (WriteStringToGlobalInifile(plugin_name, maxintegrationperformance, wxString::Format(_("%d"),max)))
        m_MaxIntegrationPerformance=max;
}

//******************************************************************************

wxString CBSvnPluginManager::GetMainMenu() const
{
    return m_mainmenu;
}

//******************************************************************************

void CBSvnPluginManager::SetMainMenu(const wxString& s)
{
    if (WriteStringToGlobalInifile(plugin_name, mainmenu, s))
        m_mainmenu=s;
}

//******************************************************************************

wxString CBSvnPluginManager::GetPopupMenu() const
{
    return m_popupmenu;
}

//******************************************************************************

void CBSvnPluginManager::SetPopupMenu(const wxString& s)
{
    if (WriteStringToGlobalInifile(plugin_name, popupmenu, s))
        m_popupmenu=s;
}

//******************************************************************************

void CBSvnPluginManager::OnLogEvent(const wxString& msg)
{
    if (!m_debug)
        return;

    Manager::Get()->GetLogManager()->Log(_T("CBTortoiseSVN: ") + msg);
}

//******************************************************************************

bool CBSvnPluginManager::FileUnderVersionControl(const wxString& filename)
{
    wxString output;
    wxString command = wxString(_(" status \"")) + filename + wxString(_("\""));
    int exit_code = Run(GetSvn(),_("c:\\"),command, output);
    Logger::GetInstance().log(_("Executed: ") + GetSvn() + command);
    Logger::GetInstance().log(_("Output: ") + output);
    return (exit_code==0) && (output.Find(_("is not a working copy"))==-1) && (output.Find(_("?"))==-1);
}

//******************************************************************************

void CBSvnPluginManager::RunSimpleTortoiseSVNCommand(const IMenuCmd& menu,const wxString& command, const wxString& special_command)
{
    wxString commandline = _("\"") +  GetTortoiseproc() + _("\" ");
    if (special_command==_(""))
    {
        LogMenu(menu);

        wxString filename=menu.GetFilename();

        if (filename==_(""))
            return;

        if (menu.GetProjectBased() || menu.GetWorkspaceBased())
        {
            if (menu.GetCustom())
            {
                if (menu.GetProjectBased())
                    filename=GetCustomDir(plugin_name,filename,project_custom_location,project_custom_relative);
                else
                    filename=GetCustomDir(plugin_name,filename,workspace_custom_location,workspace_custom_relative);
            }
            else
                filename=CBTSVN::GetBaseDir(filename);
        }

        if (menu.GetFileBased())
        {
            if (!FileUnderVersionControl(filename))
            {
                wxMessageBox(_("File is not a working copy or svn.exe not found."), _("Info"),
                             wxOK | wxICON_INFORMATION);
                return;
            }
        }

        commandline += _("/command:")
                       + command
                       + _(" /path:\"")
                       + filename
                       +_("\" /notempfile");
    }
    else
        commandline += special_command;

    Logger::GetInstance().log(_("Executing: ") + commandline);

    DWORD exit_code;
    if (!Run(false, false, commandline, exit_code))
    {
        wxString msg = _("Command \"") + commandline + _("\" failed") + _("\r\n\r\nTry reconfiguring this plugin.");
        wxMessageBox(msg,_("Info"),
                     wxOK | wxICON_ERROR);
    }
}

//******************************************************************************

void CBSvnPluginManager::Initialise()
{
    Logger::GetInstance().Subscribe(*this);
}

//******************************************************************************

void CBSvnPluginManager::Shutdown()
{
    Logger::GetInstance().Unsubscribe(*this);
}

//******************************************************************************
// End of file
