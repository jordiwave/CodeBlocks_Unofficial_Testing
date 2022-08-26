/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef USERVARDLGS_H
#define USERVARDLGS_H

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "uservarmanager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "infowindow.h"

    #include <wx/button.h>
    #include "scrollingdialog.h"
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
    #include <wx/textdlg.h>
    #include <wx/splitter.h>
    #include <wx/choice.h>
    #include <wx/listbox.h>
#endif


class GetUserVariableDialog : public wxScrollingDialog
{

        friend UserVariableManager;

    public:
        GetUserVariableDialog(wxWindow *parent, const wxString &old);

        wxString GetVariable()
        {
            return m_SelectedVar;
        }
    private:
        void OnOK(cb_unused wxCommandEvent& event);
        void OnCancel(cb_unused wxCommandEvent& event);
        void OnConfig(cb_unused wxCommandEvent& event);
        void OnActivated(wxTreeEvent& event);

        void Load();

        wxString GetSelectedVariable();
    private:
        wxTreeCtrl *m_treectrl;
        wxString m_SelectedVar;
        wxString m_old;

        DECLARE_EVENT_TABLE()
};

class UsrGlblMgrEditDialog : public wxScrollingDialog
{
        wxString m_CurrentSetName;
        wxString m_CurrentVar;

        wxChoice *m_SelSet;
        wxListBox *m_SelVar;

        wxButton *m_DeleteSet;

        wxTextCtrl *m_Base;
        wxTextCtrl *m_Include;
        wxTextCtrl *m_Lib;
        wxTextCtrl *m_Obj;
        wxTextCtrl *m_Bin;

        std::vector<wxTextCtrl*> m_Name;
        std::vector<wxTextCtrl*> m_Value;

        VariableSetMap m_varMap;

        UserVariableManager *m_UserVarMgr;

        void Help(wxCommandEvent& event);
        void DoClose();
        void OnOK(cb_unused wxCommandEvent& event)
        {
            DoClose();
        };
        void OnCancel(cb_unused wxCommandEvent& event)
        {
            DoClose();
        };
        void CloseHandler(cb_unused wxCloseEvent& event)
        {
            DoClose();
        };

        void CloneVar(wxCommandEvent&  event);
        void CloneSet(wxCommandEvent&  event);
        void NewVar(wxCommandEvent&    event);
        void NewSet(wxCommandEvent&    event);
        void DeleteVar(wxCommandEvent& event);
        void DeleteSet(wxCommandEvent& event);
        void SaveSet(cb_unused wxCommandEvent& event);
        // handler for the folder selection button
        void OnFS(wxCommandEvent& event);

        void SelectSet(wxCommandEvent& event);
        void SelectVar(wxCommandEvent& event);

        void Load();
        void Save();
        void UpdateChoices();
        void AddVar(const wxString& var);
        void Sanitise(wxString& s);

        // Export, import sets
        wxString GetExportFileName(bool exportAllSets);
        void ExportXMLtoFile(TiXmlDocument * exportXmlDoc, bool exportAllSets);
        void ExportSetData(bool exportAllSets);
        void ExportAllSets(wxCommandEvent & event);
        void ExportSet(wxCommandEvent & event);
        void ImportSet(wxCommandEvent & event);
        // void SaveSet(wxCommandEvent & event);
        bool TiXmlSuccess(TiXmlDocument * xmlDoc, wxString & xmlFileName);

        DECLARE_EVENT_TABLE()

    public:
        UsrGlblMgrEditDialog(const wxString& var = wxEmptyString);
        friend class UserVarManagerGUI;
};

class AskUserVariableDialog : public wxScrollingDialog
{
public:
    AskUserVariableDialog(wxWindow *parent, const wxString &name, const wxString& desc, const wxString& def);

    wxString GetValue() { return m_txtValue->GetValue(); }
private:
    void OnBrowse(cb_unused wxCommandEvent& event);

private:
    wxTextCtrl *m_txtValue;
    wxString m_VarName;

    DECLARE_EVENT_TABLE()
};


class UserVarManagerGUI : public UserVarManagerUI
{
    public:
        void DisplayInfoWindow(const wxString &title, const wxString &msg) override;
        void OpenEditWindow(const std::set<wxString> &var) override;
        wxString GetVariable(wxWindow* parent, const wxString &old) override;
        bool AskUserForVariable(const wxString &name, const wxString& desc, const wxString& def, wxString& value) override;
};

#endif // USERVARDLGS_H
