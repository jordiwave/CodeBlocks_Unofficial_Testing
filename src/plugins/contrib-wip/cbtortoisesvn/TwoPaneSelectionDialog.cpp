//******************************************************************************
//* Name:      TwoPaneSelectionDialog.cpp
//* Purpose:   2 pane dialog
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************


#include "TwoPaneSelectionDialog.h"

//(*InternalHeaders(TwoPaneSelectionDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(TwoPaneSelectionDialog)
const long TwoPaneSelectionDialog::ID_STATICTEXT1 = wxNewId();
const long TwoPaneSelectionDialog::ID_STATICTEXT2 = wxNewId();
const long TwoPaneSelectionDialog::ID_LISTBOX1 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON1 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON2 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON3 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON6 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON7 = wxNewId();
const long TwoPaneSelectionDialog::ID_LISTBOX2 = wxNewId();
const long TwoPaneSelectionDialog::ID_STATICLINE1 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON5 = wxNewId();
const long TwoPaneSelectionDialog::ID_BUTTON4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TwoPaneSelectionDialog,wxDialog)
    //(*EventTable(TwoPaneSelectionDialog)
    //*)
END_EVENT_TABLE()

TwoPaneSelectionDialog::TwoPaneSelectionDialog(
    const std::vector<wxString>& left,
    std::vector<wxString>& right,
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size)
    : m_left(left), m_right(right)
{
    //(*Initialize(TwoPaneSelectionDialog)
    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Available commands"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
    BoxSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(163,20,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Current commands"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
    BoxSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
    ListBoxLeft = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(200,250), 0, 0, wxLB_SINGLE, wxDefaultValidator, _T("ID_LISTBOX1"));
    FlexGridSizer2->Add(ListBoxLeft, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    ButtonAdd = new wxButton(this, ID_BUTTON1, _("Add -->"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(ButtonAdd, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonRemove = new wxButton(this, ID_BUTTON2, _("<-- Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer3->Add(ButtonRemove, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonSeparator = new wxButton(this, ID_BUTTON3, _("Add separator"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer3->Add(ButtonSeparator, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonUp = new wxButton(this, ID_BUTTON6, _("Up"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    BoxSizer3->Add(ButtonUp, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonDown = new wxButton(this, ID_BUTTON7, _("Down"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    BoxSizer3->Add(ButtonDown, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ListBoxRight = new wxListBox(this, ID_LISTBOX2, wxDefaultPosition, wxSize(200,250), 0, 0, wxLB_SINGLE, wxDefaultValidator, _T("ID_LISTBOX2"));
    FlexGridSizer2->Add(ListBoxRight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(520,2), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    BoxSizer2->Add(StaticLine1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    ButtonCancel = new wxButton(this, ID_BUTTON5, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer4->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonOK = new wxButton(this, ID_BUTTON4, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer4->Add(ButtonOK, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(BoxSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnListBoxLeftDClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonAddClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonRemoveClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonSeparatorClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonUpClick);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonDownClick);
    Connect(ID_LISTBOX2,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnListBoxRightDClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonCancelClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TwoPaneSelectionDialog::OnButtonOKClick);
    //*)

    // Fill the lisboxes
    Fill(dirLeft,m_left);
    Fill(dirRight,m_right);

    // delete items from left already present on the right
    int index=0;
    for (unsigned int i=0; i<ListBoxRight->GetCount(); ++i)
        if ((index=ListBoxLeft->FindString(ListBoxRight->GetString(i)))!=wxNOT_FOUND)
            ListBoxLeft->Delete(index);

    ListBoxLeft->SetSelection(0);
}

//******************************************************************************

TwoPaneSelectionDialog::~TwoPaneSelectionDialog()
{
    //(*Destroy(TwoPaneSelectionDialog)
    //*)
}

//******************************************************************************

void TwoPaneSelectionDialog::Fill(direction dir, const std::vector<wxString>& list)
{
    wxListBox* listbox;
    dir==dirLeft?listbox=ListBoxLeft:listbox=ListBoxRight;

    wxArrayString array;
    for (std::vector<wxString>::const_iterator it=list.begin();
            it!=list.end(); ++it)
        array.Add(*it);

    listbox->InsertItems(array,0);
    listbox->SetSelection(0);
}

//******************************************************************************

bool TwoPaneSelectionDialog::GetPosition(const std::vector<wxString>& vec, const wxString& s, int& pos)
{
    pos=0;
    for (std::vector<wxString>::const_iterator it=vec.begin(); it!=vec.end(); ++it)
    {
        if ((*it)==s)
            return true;
        ++pos;
    }
    return false;
}

//******************************************************************************

void TwoPaneSelectionDialog::AddString(direction dir, const wxString& s, int pos)
{
    wxListBox* listbox;
    dir==dirLeft?listbox=ListBoxLeft:listbox=ListBoxRight;

    listbox->InsertItems(1,&s,pos);
    listbox->SetSelection(pos);
}

//******************************************************************************

void TwoPaneSelectionDialog::AddString(direction dir, const wxString& s)
{
    wxListBox* listbox;
    dir==dirLeft?listbox=ListBoxLeft:listbox=ListBoxRight;

    int index = listbox->GetSelection();
    if (index==wxNOT_FOUND)
        index=0;
    else
        ++index;

    listbox->InsertItems(1,&s, index);
    listbox->SetSelection(index);
}

//******************************************************************************

bool TwoPaneSelectionDialog::RemoveSelection(direction dir)
{
    wxListBox* listbox;
    dir==dirLeft?listbox=ListBoxLeft:listbox=ListBoxRight;

    unsigned int index = listbox->GetSelection();

    if (index==(unsigned int)wxNOT_FOUND)
        return false;

    listbox->Delete(index);
    if (listbox->GetCount()==index)
        --index;

    listbox->SetSelection(index);
    return true;
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonAddClick(wxCommandEvent& event)
{
    if (ListBoxLeft->GetStringSelection()!=_(""))
    {
        AddString(dirRight,ListBoxLeft->GetStringSelection());
        RemoveSelection(dirLeft);
    }
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonRemoveClick(wxCommandEvent& event)
{
    wxString s = ListBoxRight->GetStringSelection();
    if (RemoveSelection(dirRight))
    {
        int end_pos;
        if (!GetPosition(m_left,s,end_pos))
            return;

        size_t index=0;
        for (index=0; index<ListBoxLeft->GetCount(); ++index)
        {
            int pos;
            GetPosition(m_left,ListBoxLeft->GetString(index),pos);
            if (pos>end_pos)
                break;
        }
        AddString(dirLeft,s,index);
    }
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonSeparatorClick(wxCommandEvent& event)
{
    AddString(dirRight, _("-----------------------"));
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonUpClick(wxCommandEvent& event)
{
    int index = ListBoxRight->GetSelection();
    if ((index==wxNOT_FOUND) || (index ==0))
        return;

    wxString s = ListBoxRight->GetStringSelection();
    ListBoxRight->Delete(index);
    ListBoxRight->Insert(s,index-1);
    ListBoxRight->SetSelection(index-1);
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonDownClick(wxCommandEvent& event)
{
    int index = ListBoxRight->GetSelection();
    if ((index==wxNOT_FOUND) || (index==((int)ListBoxRight->GetCount())-1))
        return;

    wxString s = ListBoxRight->GetStringSelection();
    ListBoxRight->Delete(index);
    ListBoxRight->Insert(s,index+1);
    ListBoxRight->SetSelection(index+1);
}

//******************************************************************************

void TwoPaneSelectionDialog::OnListBoxLeftDClick(wxCommandEvent& event)
{
    OnButtonAddClick(event);
}

//******************************************************************************

void TwoPaneSelectionDialog::OnListBoxRightDClick(wxCommandEvent& event)
{
    OnButtonRemoveClick(event);
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonOKClick(wxCommandEvent& event)
{
    m_right.clear();
    for (unsigned int i=0; i<ListBoxRight->GetCount(); ++i)
        m_right.push_back(ListBoxRight->GetString(i));
    EndModal(wxID_OK);
}

//******************************************************************************

void TwoPaneSelectionDialog::OnButtonCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

//******************************************************************************
// End of file
