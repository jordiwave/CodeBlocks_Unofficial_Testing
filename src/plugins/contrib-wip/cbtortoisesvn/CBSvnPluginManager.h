//******************************************************************************
//* Name:      CBSvnPluginManager.h
//* Purpose:   Plugin manager object (singleton)
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef _CBSVNPLUGINMANAGER_H
#define _CBSVNPLUGINMANAGER_H

//******************************************************************************

#include <memory>
#include <wx/intl.h>
#include <wx/string.h>
#include "utils.h"

//******************************************************************************

#define MAIN_MENU    (CBSvnPluginManager::GetInstance().GetMenu(CBSvnPluginManager::smMainMenu))
#define POPUP_MENU   (CBSvnPluginManager::GetInstance().GetMenu(CBSvnPluginManager::smPopupMenu))

//******************************************************************************

namespace CBTSVN
{
const wxString plugin_name               = _("CBTortoiseSVN");
const wxString svnpath                   = _("SVNPATH");
const wxString tortoisesvnpath           = _("TORTOISE_SVN_PATH");
const wxString mainmenuintegration       = _("MAINMENU_INTEGRATION");
const wxString editorintegration         = _("EDITOR_INTEGRATION");
const wxString projectmanagerintegration = _("PROJECT_MANAGER_INTEGRATION");
const wxString maxintegrationperformance = _("MAX_INTEGRATION_PERFORMANCE");
const wxString mainmenu                  = _("MAINMENU");
const wxString popupmenu                 = _("POPUPMENU");

const wxString project_custom_location   = _("PROJECT_CUSTOM_LOCATION");
const wxString project_custom_relative   = _("PROJECT_CUSTOM_RELATIVE");
const wxString workspace_custom_location = _("WORKSPACE_CUSTOM_LOCATION");
const wxString workspace_custom_relative = _("WORKSPACE_CUSTOM_RELATIVE");

const wxString separator                 = _("-----------------");

class IMenuCmd : public IInterfaceBase
{
    public:
        virtual bool GetFileBased() const = 0;
        virtual void SetFileBased(bool FileBased) = 0;
        virtual bool GetProjectBased() const = 0;
        virtual void SetProjectBased(bool ProjectBased) = 0;
        virtual bool GetWorkspaceBased() const = 0;
        virtual void SetWorkspaceBased(bool ProjectBased) = 0;
        virtual void SetCustom(bool custom) = 0;
        virtual bool GetCustom() const = 0;
        virtual wxString GetFilename() const = 0;
        virtual void SetFilename(const wxString & filename) = 0;
};

class CBSvnPluginManager :
    ILogSink,
    Noncopyable
{
    public:
        static CBSvnPluginManager & GetInstance();

        // location svn.exe
        wxString GetSvn() const;
        void SetSvn(const wxString & svn);

        // location GetTortoiseproc.exe
        wxString GetTortoiseproc() const;
        void SetTortoiseproc(const wxString & svn);

        // integration
        bool GetMainMenuIntegration() const;
        void SetMainMenuIntegration(bool enabled);

        bool GetEditorIntegration() const;
        void SetEditorIntegration(bool enabled);

        bool GetProjectManagerIntegration() const;
        void SetProjectManagerIntegration(bool enabled);

        bool GetMaxIntegrationPerformance() const;
        void SetMaxIntegrationPerformance(bool max);

        // menu
        wxString GetMainMenu() const;
        void SetMainMenu(const wxString & s);

        wxString GetPopupMenu() const;
        void SetPopupMenu(const wxString & s);

        // FileProjectWorkspaceBased
        enum SourceMenu
        {
            smMainMenu = 0,
            smPopupMenu,
            smNumSourceMenu
        };

        IMenuCmd & GetMenu(const SourceMenu & menu)
        {
            return *menus[menu];
        }

        // logging
        void Togglelogging(bool debug)
        {
            m_debug = !m_debug;
        }

        // lifecycle
        void Initialise();
        void Shutdown();

        // misc.
        bool FileUnderVersionControl(const wxString & filename);
        void RunSimpleTortoiseSVNCommand(const IMenuCmd & menu, const wxString & command, const wxString & special_command = _(""));

    private:
        void OnLogEvent(const wxString & msg);

        CBSvnPluginManager();
        ~CBSvnPluginManager() {};
        std::unique_ptr<IMenuCmd> menus[smNumSourceMenu];
        wxString    m_svn;
        wxString    m_tortoisesvnpath;
        bool        m_MainMenuIntegration;
        bool        m_EditorIntegration;
        bool        m_ProjectManagerIntegration;
        bool        m_MaxIntegrationPerformance;
        bool        m_expert;
        bool        m_debug;
        wxString    m_mainmenu;
        wxString    m_popupmenu;
};

// support functions
void LogMenu(const IMenuCmd & menu);

};

//******************************************************************************
#endif // _CBSVNPLUGINMANAGER_H
