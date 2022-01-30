#include "wx_pch.h"
#include "AddComp.h"

#include <globals.h>

#ifndef WX_PRECOMP
//(*InternalHeadersPCH(AddComp)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#endif
//(*InternalHeaders(AddComp)
//*)

//(*IdInit(AddComp)
const long AddComp::ID_STNAME = wxNewId();
const long AddComp::ID_TCNAME = wxNewId();
const long AddComp::ID_CHECKLISTBOX1 = wxNewId();
const long AddComp::ID_PANEL1 = wxNewId();
const long AddComp::ID_BOK = wxNewId();
const long AddComp::ID_BCANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(AddComp,wxDialog)
    //(*EventTable(AddComp)
    //*)
END_EVENT_TABLE()

AddComp::AddComp(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(AddComp)
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxStaticBoxSizer* SBSFiles;
    wxBoxSizer* BoxSizer3;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("id"));
    SetClientSize(wxDefaultSize);
    Move(wxDefaultPosition);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    STName = new wxStaticText(this, ID_STNAME, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STNAME"));
    BoxSizer2->Add(STName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TCName = new wxTextCtrl(this, ID_TCNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, wxDefaultValidator, _T("ID_TCNAME"));
    TCName->SetToolTip(_("Insert the Name of the new Component"));
    BoxSizer2->Add(TCName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SBSFiles = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Files"));
    CLBFiles = new wxCheckListBox(this, ID_CHECKLISTBOX1, wxDefaultPosition, wxSize(207,166), 0, 0, 0, wxDefaultValidator, _T("ID_CHECKLISTBOX1"));
    CLBFiles->SetToolTip(_("Select the Files you want to install with the Component\n(if the Component has Children, his files will be ignored \nand only his children\'s files will be installed)"));
    SBSFiles->Add(CLBFiles, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer1->Add(SBSFiles, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer3->Add(Panel1, 1, wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BOk = new wxButton(this, ID_BOK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BOK"));
    BoxSizer3->Add(BOk, 0, wxBOTTOM|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    BCancel = new wxButton(this, ID_BCANCEL, _("Abbrechen"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BCANCEL"));
    BoxSizer3->Add(BCancel, 0, wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BOK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddComp::OnOKClick);
    Connect(ID_BCANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddComp::OnCancelClick);
    //*)
}

AddComp::~AddComp()
{
    //(*Destroy(AddComp)
    //*)
}

void AddComp::SetFiles(wxArrayString files)
{
    CLBFiles->InsertItems(files,0);
}

CompItem *AddComp::GetCompItem()
{
    CompItem* item = new CompItem();
    item->m_Name = TCName->GetValue();
    for (unsigned int i=0; i<CLBFiles->GetCount(); i++)
    {
        if (CLBFiles->IsChecked(i))
            item->m_Files.Add(CLBFiles->GetString(i));
    }
    return item;
}

void AddComp::OnOKClick(wxCommandEvent& event)
{
    if (TCName->IsEmpty())
    {
        cbMessageBox(_T("Insert a Name!"));
        return;
    }
    if (CLBFiles->IsEmpty())
    {
        cbMessageBox(_T("No file(s) selected!"));
        return;
    }
    EndModal(wxID_OK);
}

void AddComp::OnCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_ABORT);
}
