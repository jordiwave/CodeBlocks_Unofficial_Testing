
#include "CustomConfigDialog.h"
#include "CBSvnPluginManager.h"
#include "utils.h"

#include <wx/dirdlg.h>
#include <wx/filename.h>

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(CustomConfigDialog)
    #include <wx/intl.h>
    #include <wx/string.h>
    //*)
#endif
//(*InternalHeaders(CustomConfigDialog)
#include <wx/settings.h>
//*)

using namespace CBTSVN;


//(*IdInit(CustomConfigDialog)
const long CustomConfigDialog::ID_TEXTCTRL1 = wxNewId();
const long CustomConfigDialog::ID_BUTTON3 = wxNewId();
const long CustomConfigDialog::ID_STATICTEXT1 = wxNewId();
const long CustomConfigDialog::ID_STATICTEXT3 = wxNewId();
const long CustomConfigDialog::ID_TEXTCTRL3 = wxNewId();
const long CustomConfigDialog::ID_CHECKBOX1 = wxNewId();
const long CustomConfigDialog::ID_PANEL1 = wxNewId();
const long CustomConfigDialog::ID_TEXTCTRL2 = wxNewId();
const long CustomConfigDialog::ID_STATICTEXT2 = wxNewId();
const long CustomConfigDialog::ID_BUTTON5 = wxNewId();
const long CustomConfigDialog::ID_STATICTEXT4 = wxNewId();
const long CustomConfigDialog::ID_TEXTCTRL4 = wxNewId();
const long CustomConfigDialog::ID_CHECKBOX2 = wxNewId();
const long CustomConfigDialog::ID_PANEL2 = wxNewId();
const long CustomConfigDialog::ID_NOTEBOOK1 = wxNewId();
const long CustomConfigDialog::ID_BUTTON1 = wxNewId();
const long CustomConfigDialog::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CustomConfigDialog, wxDialog)
    //(*EventTable(CustomConfigDialog)
    //*)
END_EVENT_TABLE()

CustomConfigDialog::CustomConfigDialog(wxWindow * parent, wxWindowID id)
{
    //(*Initialize(CustomConfigDialog)
    wxBoxSizer * BoxSizer2;
    wxBoxSizer * BoxSizer1;
    wxFlexGridSizer * FlexGridSizer1;
    Create(parent, wxID_ANY, _("Custom configuration"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(523, 196));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxSize(492, 143), 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxPoint(177, 122), wxSize(408, 174), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    TextCtrlCustomProject = new wxTextCtrl(Panel1, ID_TEXTCTRL1, wxEmptyString, wxPoint(160, 16), wxSize(272, 21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    TextCtrlCustomProject->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    ButtonCustomProject = new wxButton(Panel1, ID_BUTTON3, _("..."), wxPoint(440, 16), wxSize(24, 23), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticTextCustomProject = new wxStaticText(Panel1, ID_STATICTEXT1, _("Project custom location"), wxPoint(16, 18), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    StaticTextCustomProjectAbsolute = new wxStaticText(Panel1, ID_STATICTEXT3, _("Absolute location"), wxPoint(16, 42), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    TextCtrlCustomProjectAbsolute = new wxTextCtrl(Panel1, ID_TEXTCTRL3, wxEmptyString, wxPoint(160, 40), wxSize(272, 21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    TextCtrlCustomProjectAbsolute->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    CheckBoxCustomProjectRelative = new wxCheckBox(Panel1, ID_CHECKBOX1, _("Use relative path"), wxPoint(160, 72), wxSize(200, 13), 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxCustomProjectRelative->SetValue(true);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxPoint(140, 9), wxSize(386, 110), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    TextCtrlCustomWorkspace = new wxTextCtrl(Panel2, ID_TEXTCTRL2, wxEmptyString, wxPoint(160, 16), wxSize(272, 21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    TextCtrlCustomWorkspace->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    StaticTextCustomWorkspace = new wxStaticText(Panel2, ID_STATICTEXT2, _("Workspace custom location"), wxPoint(16, 18), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    ButtonCustomWorkspace = new wxButton(Panel2, ID_BUTTON5, _("..."), wxPoint(440, 16), wxSize(24, 23), 0, wxDefaultValidator, _T("ID_BUTTON5"));
    StaticTextCustomWorkspaceAbsolute = new wxStaticText(Panel2, ID_STATICTEXT4, _("Absolute location"), wxPoint(16, 42), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    TextCtrlCustomWorkspaceAbsolute = new wxTextCtrl(Panel2, ID_TEXTCTRL4, wxEmptyString, wxPoint(160, 40), wxSize(272, 21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    TextCtrlCustomWorkspaceAbsolute->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    CheckBoxCustomWorkspaceRelative = new wxCheckBox(Panel2, ID_CHECKBOX2, _("Use relative path"), wxPoint(160, 72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxCustomWorkspaceRelative->SetValue(true);
    Notebook1->AddPage(Panel1, _("Project"), false);
    Notebook1->AddPage(Panel2, _("Workspace"), false);
    BoxSizer2->Add(Notebook1, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer2, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    ButtonCancel = new wxButton(this, ID_BUTTON1, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer1->Add(ButtonCancel, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    ButtonOK = new wxButton(this, ID_BUTTON2, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer1->Add(ButtonOK, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer1, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->SetSizeHints(this);
    Connect(ID_BUTTON3, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnButtonCustomProjectClick);
    Connect(ID_CHECKBOX1, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnCheckBoxCustomProjectRelativeClick);
    Connect(ID_BUTTON5, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnButtonCustomWorkspaceClick);
    Connect(ID_CHECKBOX2, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnCheckBoxCustomWorkspaceRelativeClick);
    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnButtonCancelClick);
    Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&CustomConfigDialog::OnButtonOKClick);
    //*)

    if (GetProjectFilename() == _("") || !MAIN_MENU.GetProjectBased())
    {
        Notebook1->GetPage(0)->Enable(false);
    }
    else
    {
        wxString str;
        ReadStringFromProjectInifile(plugin_name, project_custom_location, str);
        TextCtrlCustomProject->SetValue(str);
        TextCtrlCustomProjectAbsolute->SetValue(str);
        ReadStringFromProjectInifile(plugin_name, project_custom_relative, str);
        CheckBoxCustomProjectRelative->SetValue(str == _("1") || str == _(""));
        UpdateAbsoluteRelative(*TextCtrlCustomProjectAbsolute, GetBaseDir(GetProjectFilename()), false);
    }

    if (GetWorkspaceFilename() == _("") || !MAIN_MENU.GetWorkspaceBased())
    {
        Notebook1->GetPage(1)->Enable(false);
    }
    else
    {
        wxString str;
        ReadStringFromWorkspaceInifile(plugin_name, workspace_custom_location, str);
        TextCtrlCustomWorkspace->SetValue(str);
        TextCtrlCustomWorkspaceAbsolute->SetValue(str);
        ReadStringFromWorkspaceInifile(plugin_name, workspace_custom_relative, str);
        CheckBoxCustomWorkspaceRelative->SetValue(str == _("1") || str == _(""));
        UpdateAbsoluteRelative(*TextCtrlCustomWorkspaceAbsolute, GetBaseDir(GetWorkspaceFilename()), false);
    }
}

//******************************************************************************

CustomConfigDialog::~CustomConfigDialog()
{
    //(*Destroy(CustomConfigDialog)
    //*)
}

//******************************************************************************

void CustomConfigDialog::OnButtonCancelClick(wxCommandEvent & event)
{
    EndModal(wxID_CANCEL);
}

//******************************************************************************

void CustomConfigDialog::OnButtonOKClick(wxCommandEvent & event)
{
    if (GetProjectFilename() != _("") && MAIN_MENU.GetProjectBased())
    {
        CBTSVN::Logger::GetInstance().log(_("Saving: ") + GetProjectIniFile(plugin_name));
        WriteStringToProjectInifile(plugin_name, project_custom_location, TextCtrlCustomProject->GetValue());
        wxString val;
        CheckBoxCustomProjectRelative->GetValue() ?  val = _("1") : val = _("0");
        WriteStringToProjectInifile(plugin_name, project_custom_relative, val);
    }

    if (GetWorkspaceFilename() != _("") && !MAIN_MENU.GetWorkspaceBased())
    {
        CBTSVN::Logger::GetInstance().log(_("Saving: ") + GetWorkspaceIniFile(plugin_name));
        WriteStringToWorkspaceInifile(plugin_name, workspace_custom_location, TextCtrlCustomWorkspace->GetValue());
        wxString val;
        CheckBoxCustomWorkspaceRelative->GetValue() ?  val = _("1") : val = _("0");
        WriteStringToWorkspaceInifile(plugin_name, workspace_custom_relative, val);
    }

    EndModal(wxID_OK);
}

//******************************************************************************

bool CustomConfigDialog::GetDir(wxString & newdir, const wxString & olddir, const wxString & basedir)
{
    wxFileName dir;
    dir.AssignDir(olddir);
    dir.MakeAbsolute(basedir);
    wxDirDialog dlg(this,                    // parent,
                    _("Select directory"), // message
                    dir.GetFullPath(),     // defaultPath
                    wxDD_DEFAULT_STYLE,    // style
                    wxDefaultPosition,     // position,
                    wxDefaultSize          // size,
                    // name
                   );

    if (dlg.ShowModal() != wxID_OK)
    {
        return false;
    }

    newdir = dlg.GetPath();
    return true;
}

//******************************************************************************

void CustomConfigDialog::OnButtonCustomProjectClick(wxCommandEvent & event)
{
    wxString newdir;

    if (GetDir(newdir, TextCtrlCustomProject->GetValue(), GetBaseDir(GetProjectFilename())))
    {
        CBTSVN::Logger::GetInstance().log(_("new dir: ") + newdir);
        TextCtrlCustomProject->SetValue(newdir);
        TextCtrlCustomProjectAbsolute->SetValue(newdir);
        bool relative = CheckBoxCustomProjectRelative->GetValue();
        UpdateAbsoluteRelative(*TextCtrlCustomProject, GetBaseDir(GetProjectFilename()), relative);
        UpdateAbsoluteRelative(*TextCtrlCustomProjectAbsolute, GetBaseDir(GetProjectFilename()), false);
    }
}

//******************************************************************************

void CustomConfigDialog::OnButtonCustomWorkspaceClick(wxCommandEvent & event)
{
    wxString newdir;

    if (GetDir(newdir, TextCtrlCustomWorkspace->GetValue(), GetBaseDir(GetWorkspaceFilename())))
    {
        CBTSVN::Logger::GetInstance().log(_("new dir: ") + newdir);
        TextCtrlCustomWorkspace->SetValue(newdir);
        TextCtrlCustomWorkspaceAbsolute->SetValue(newdir);
        bool relative = CheckBoxCustomWorkspaceRelative->GetValue();
        UpdateAbsoluteRelative(*TextCtrlCustomWorkspace, GetBaseDir(GetWorkspaceFilename()), relative);
        UpdateAbsoluteRelative(*TextCtrlCustomWorkspaceAbsolute, GetBaseDir(GetWorkspaceFilename()), false);
    }
}

//******************************************************************************

void CustomConfigDialog::UpdateAbsoluteRelative(wxTextCtrl & ctrl, const wxString & basedir, bool relative)
{
    wxFileName dir;
    dir.AssignDir(ctrl.GetValue());

    if (relative)
    {
        dir.MakeRelativeTo(basedir);
    }
    else
    {
        dir.MakeAbsolute(basedir);
    }

    ctrl.SetValue(dir.GetFullPath());
}

//******************************************************************************

void CustomConfigDialog::OnCheckBoxCustomProjectRelativeClick(wxCommandEvent & event)
{
    bool relative = CheckBoxCustomProjectRelative->GetValue();
    UpdateAbsoluteRelative(*TextCtrlCustomProject, GetBaseDir(GetProjectFilename()), relative);
    UpdateAbsoluteRelative(*TextCtrlCustomProjectAbsolute, GetBaseDir(GetProjectFilename()), false);
}

//******************************************************************************

void CustomConfigDialog::OnCheckBoxCustomWorkspaceRelativeClick(wxCommandEvent & event)
{
    bool relative = CheckBoxCustomWorkspaceRelative->GetValue();
    UpdateAbsoluteRelative(*TextCtrlCustomWorkspace, GetBaseDir(GetWorkspaceFilename()), relative);
    UpdateAbsoluteRelative(*TextCtrlCustomWorkspaceAbsolute, GetBaseDir(GetWorkspaceFilename()), false);
}
