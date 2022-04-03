//******************************************************************************
//* Name:      ConfigDialog.cpp
//* Purpose:   Configuration dialog
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************


#include "ConfigDialog.h"
#include "CBSvnPluginManager.h"
#include "TwoPaneSelectionDialog.h"
#include "menu.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>

using namespace CBTSVN;

//(*IdInit(ConfigDialog)
const long ConfigDialog::ID_STATICTEXT1 = wxNewId();
const long ConfigDialog::ID_TEXTCTRL1 = wxNewId();
const long ConfigDialog::ID_BUTTON2 = wxNewId();
const long ConfigDialog::ID_STATICTEXT2 = wxNewId();
const long ConfigDialog::ID_TEXTCTRL2 = wxNewId();
const long ConfigDialog::ID_BUTTON5 = wxNewId();
const long ConfigDialog::ID_PANEL2 = wxNewId();
const long ConfigDialog::ID_STATICBOX1 = wxNewId();
const long ConfigDialog::ID_STATICBOX2 = wxNewId();
const long ConfigDialog::ID_CHECKBOX1 = wxNewId();
const long ConfigDialog::ID_CHECKBOX3 = wxNewId();
const long ConfigDialog::ID_CHECKBOX2 = wxNewId();
const long ConfigDialog::ID_STATICTEXT3 = wxNewId();
const long ConfigDialog::ID_STATICBOX3 = wxNewId();
const long ConfigDialog::ID_CHECKBOX4 = wxNewId();
const long ConfigDialog::ID_STATICBOX4 = wxNewId();
const long ConfigDialog::ID_BUTTON6 = wxNewId();
const long ConfigDialog::ID_BUTTON7 = wxNewId();
const long ConfigDialog::ID_PANEL3 = wxNewId();
const long ConfigDialog::ID_BUTTON4 = wxNewId();
const long ConfigDialog::ID_STATICTEXT4 = wxNewId();
const long ConfigDialog::ID_PANEL1 = wxNewId();
const long ConfigDialog::ID_NOTEBOOK1 = wxNewId();
const long ConfigDialog::ID_BUTTON1 = wxNewId();
const long ConfigDialog::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
    //(*EventTable(ConfigDialog)
    //*)
END_EVENT_TABLE()

ConfigDialog::ConfigDialog(const std::vector<CBTSVN::MenuEntry> & all_menu_entries, wxWindow * parent, wxWindowID id) :
    m_all_menu_entries(all_menu_entries)
{
    //(*Initialize(ConfigDialog)
    Create(0, wxID_ANY, _("CBTortoiseSVN Settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(244, 114));
    FlexGridSizer1 = new wxFlexGridSizer(3, 0, 0, 0);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxSize(445, 231), 0, _T("ID_NOTEBOOK1"));
    PanelPath = new wxPanel(Notebook1, ID_PANEL2, wxPoint(93, 6), wxSize(433, 205), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    StaticText1 = new wxStaticText(PanelPath, ID_STATICTEXT1, _("Subversion.exe path"), wxPoint(12, 21), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    TextCtrlSVNPath = new wxTextCtrl(PanelPath, ID_TEXTCTRL1, wxEmptyString, wxPoint(128, 17), wxSize(264, 21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    ButtonGetSvnPath = new wxButton(PanelPath, ID_BUTTON2, _("..."), wxPoint(396, 16), wxSize(27, 23), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticText2 = new wxStaticText(PanelPath, ID_STATICTEXT2, _("Tortoiseproc.exe path"), wxPoint(12, 51), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    TextCtrlTortoiseSVNPath = new wxTextCtrl(PanelPath, ID_TEXTCTRL2, wxEmptyString, wxPoint(128, 48), wxSize(264, 21), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    ButtonGetTortoiseSVNPath = new wxButton(PanelPath, ID_BUTTON5, _("..."), wxPoint(396, 48), wxSize(27, 23), 0, wxDefaultValidator, _T("ID_BUTTON5"));
    Panel2 = new wxPanel(Notebook1, ID_PANEL3, wxPoint(85, 13), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    StaticBox1 = new wxStaticBox(Panel2, ID_STATICBOX1, _("Menu integration"), wxPoint(16, 8), wxSize(200, 40), 0, _T("ID_STATICBOX1"));
    StaticBox2 = new wxStaticBox(Panel2, ID_STATICBOX2, _("Popup integration"), wxPoint(16, 56), wxSize(200, 64), 0, _T("ID_STATICBOX2"));
    CheckBoxMainMenuIntegration = new wxCheckBox(Panel2, ID_CHECKBOX1, _("Integrate into main menu (*)"), wxPoint(32, 26), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxMainMenuIntegration->SetValue(false);
    CheckBoxProjectManagerIntegration = new wxCheckBox(Panel2, ID_CHECKBOX3, _("Project manager"), wxPoint(32, 74), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    CheckBoxProjectManagerIntegration->SetValue(false);
    CheckBoxEditorIntegration = new wxCheckBox(Panel2, ID_CHECKBOX2, _("Editor"), wxPoint(32, 96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxEditorIntegration->SetValue(false);
    StaticText3 = new wxStaticText(Panel2, ID_STATICTEXT3, _("(*) Requires a Codeblocks restart"), wxPoint(24, 182), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticBox3 = new wxStaticBox(Panel2, ID_STATICBOX3, _("Performance"), wxPoint(16, 128), wxSize(408, 44), 0, _T("ID_STATICBOX3"));
    CheckBoxMaxIntegration = new wxCheckBox(Panel2, ID_CHECKBOX4, _("Don\'t check if files are under version control (improves performance)"), wxPoint(32, 148), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    CheckBoxMaxIntegration->SetValue(false);
    StaticBox4 = new wxStaticBox(Panel2, ID_STATICBOX4, _("Menu"), wxPoint(232, 8), wxSize(192, 112), 0, _T("ID_STATICBOX4"));
    ButtonMainMenu = new wxButton(Panel2, ID_BUTTON6, _("Edit main menu..."), wxPoint(248, 32), wxSize(160, 32), 0, wxDefaultValidator, _T("ID_BUTTON6"));
    ButtonPopupMenu = new wxButton(Panel2, ID_BUTTON7, _("Edit popup menu..."), wxPoint(248, 72), wxSize(160, 32), 0, wxDefaultValidator, _T("ID_BUTTON7"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxPoint(124, 8), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    ButtonShowDebug = new wxButton(Panel1, ID_BUTTON4, _("Toggle debug on/off"), wxPoint(16, 16), wxSize(128, 23), 0, wxDefaultValidator, _T("ID_BUTTON4"));
    StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Note: The CBTortoiseSVN plugin logs to the Code::Blocks log tab. "), wxPoint(16, 48), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    Notebook1->AddPage(PanelPath, _("Paths"), false);
    Notebook1->AddPage(Panel2, _("Integration"), false);
    Notebook1->AddPage(Panel1, _("Misc."), false);
    BoxSizer2->Add(Notebook1, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer2, 1, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    CancelButton = new wxButton(this, ID_BUTTON1, _("cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(CancelButton, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    OkButton = new wxButton(this, ID_BUTTON3, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    OkButton->SetDefault();
    BoxSizer3->Add(OkButton, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer3, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->SetSizeHints(this);
    Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnButtonGetSvnPathClick);
    Connect(ID_BUTTON5, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnButtonGetTortoiseSVNPathClick);
    Connect(ID_BUTTON6, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnButtonMainMenuClick);
    Connect(ID_BUTTON7, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnButtonPopupMenuClick);
    Connect(ID_BUTTON4, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnButtonShowDebugClick);
    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnCancelButtonClick);
    Connect(ID_BUTTON3, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigDialog::OnOkButtonClick);
    //*)
    // Update Dialog
    TextCtrlSVNPath->SetValue(CBSvnPluginManager::GetInstance().GetSvn());
    TextCtrlTortoiseSVNPath->SetValue(CBSvnPluginManager::GetInstance().GetTortoiseproc());
    CheckBoxMainMenuIntegration->SetValue(CBSvnPluginManager::GetInstance().GetMainMenuIntegration());
    CheckBoxProjectManagerIntegration->SetValue(CBSvnPluginManager::GetInstance().GetProjectManagerIntegration());
    CheckBoxEditorIntegration->SetValue(CBSvnPluginManager::GetInstance().GetEditorIntegration());
    CheckBoxMaxIntegration->SetValue(CBSvnPluginManager::GetInstance().GetMaxIntegrationPerformance());
}

//******************************************************************************

ConfigDialog::~ConfigDialog()
{
    //(*Destroy(ConfigDialog)
    //*)
}

//******************************************************************************

void ConfigDialog::OnCancelButtonClick(wxCommandEvent & event)
{
    EndModal(wxID_CANCEL);
}

//******************************************************************************

void ConfigDialog::OnOkButtonClick(wxCommandEvent & event)
{
    bool no_integration =
        (!CheckBoxMainMenuIntegration->GetValue())          &&
        (!CheckBoxProjectManagerIntegration->GetValue())    &&
        (!CheckBoxEditorIntegration->GetValue());

    if (no_integration)
    {
        wxString msg;
        msg << _("All integration is now disabled. \r\n\r\n");
        msg << _("The settings dialog will be unavailable after a Codeblocks restart. \r\n\r\n");
        msg << _("Do you want to proceed?");

        if (wxMessageBox(msg, _("Integration disabled"), wxYES_NO | wxCANCEL | wxICON_WARNING) != wxYES)
        {
            return;
        }
    }

    CBSvnPluginManager::GetInstance().SetSvn(TextCtrlSVNPath->GetValue());
    CBSvnPluginManager::GetInstance().SetTortoiseproc(TextCtrlTortoiseSVNPath->GetValue());
    CBSvnPluginManager::GetInstance().SetMainMenuIntegration(CheckBoxMainMenuIntegration->GetValue());
    CBSvnPluginManager::GetInstance().SetProjectManagerIntegration(CheckBoxProjectManagerIntegration->GetValue());
    CBSvnPluginManager::GetInstance().SetEditorIntegration(CheckBoxEditorIntegration->GetValue());
    CBSvnPluginManager::GetInstance().SetMaxIntegrationPerformance(CheckBoxMaxIntegration->GetValue());
    EndModal(wxID_OK);
}

//******************************************************************************

void ConfigDialog::OnButtonGetSvnPathClick(wxCommandEvent & event)
{
    wxFileDialog dlg(this,               // parent,
                     _("Get svn.exe"),   // message
                     _(""),              // defaultDir
                     _("svn.exe"),       // defaultFile
                     _("*.exe"),         // wildcard
                     wxFD_FILE_MUST_EXIST,  // style
                     wxDefaultPosition   // pos = )
                    );

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    TextCtrlSVNPath->SetValue(dlg.GetPath());
}

//******************************************************************************

void ConfigDialog::OnButtonGetTortoiseSVNPathClick(wxCommandEvent & event)
{
    wxFileDialog dlg(this,                      // parent,
                     _("Get tortoiseproc.exe"), // message
                     _(""),                     // defaultDir
                     _("tortoiseproc.exe"),     // defaultFile
                     _("*.exe"),                // wildcard
                     wxFD_FILE_MUST_EXIST,         // style
                     wxDefaultPosition          // pos = )
                    );

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    TextCtrlTortoiseSVNPath->SetValue(dlg.GetPath());
}

//******************************************************************************

void ConfigDialog::OnButtonShowDebugClick(wxCommandEvent & event)
{
    CBSvnPluginManager::GetInstance().Togglelogging(true);
}

//******************************************************************************

void ConfigDialog::OnButtonPopupMenuClick(wxCommandEvent & event)
{
    std::vector<wxString> left = ConvertMenuItemsToStrings(CBSvnPluginManager::smPopupMenu, m_all_menu_entries);
    std::vector<wxString> right;
    ConvertIndicesToMenuItems(m_all_menu_entries, convert(CBSvnPluginManager::GetInstance().GetPopupMenu()), right);
    TwoPaneSelectionDialog dlg(left, right, NULL);

    if (dlg.ShowModal() == wxID_OK)
    {
        std::vector<int> indices;
        ConvertMenuItemsToIndices(m_all_menu_entries, right, indices);
        CBSvnPluginManager::GetInstance().SetPopupMenu(convert(indices));
    }
}

//******************************************************************************

void ConfigDialog::OnButtonMainMenuClick(wxCommandEvent & event)
{
    std::vector<wxString> left = ConvertMenuItemsToStrings(CBSvnPluginManager::smMainMenu, m_all_menu_entries);
    std::vector<wxString> right;
    ConvertIndicesToMenuItems(m_all_menu_entries, convert(CBSvnPluginManager::GetInstance().GetMainMenu()), right);
    TwoPaneSelectionDialog dlg(left, right, NULL);

    if (dlg.ShowModal() == wxID_OK)
    {
        std::vector<int> indices;
        ConvertMenuItemsToIndices(m_all_menu_entries, right, indices);
        CBSvnPluginManager::GetInstance().SetMainMenu(convert(indices));
    }
}

//******************************************************************************
// End of file
