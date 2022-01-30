//******************************************************************************
//* Name:      TwoPaneSelectionDialog.h
//* Purpose:   2 pane dialog
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef TwoPaneSelectionDialog_H
#define TwoPaneSelectionDialog_H

#include "menu.h"
#include <vector>
#include <wx/string.h>

//(*Headers(TwoPaneSelectionDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class TwoPaneSelectionDialog: public wxDialog
{
public:

    TwoPaneSelectionDialog(
        const std::vector<wxString>& left,
        std::vector<wxString>& right,
        wxWindow* parent,
        wxWindowID id=wxID_ANY,
        const wxPoint& pos=wxDefaultPosition,
        const wxSize& size=wxDefaultSize);
    virtual ~TwoPaneSelectionDialog();

    //(*Declarations(TwoPaneSelectionDialog)
    wxBoxSizer* BoxSizer4;
    wxButton* ButtonRemove;
    wxListBox* ListBoxLeft;
    wxStaticText* StaticText2;
    wxButton* ButtonOK;
    wxFlexGridSizer* FlexGridSizer2;
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer2;
    wxListBox* ListBoxRight;
    wxButton* ButtonAdd;
    wxButton* ButtonUp;
    wxButton* ButtonCancel;
    wxStaticLine* StaticLine1;
    wxButton* ButtonDown;
    wxBoxSizer* BoxSizer1;
    wxButton* ButtonSeparator;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    //*)

protected:

    //(*Identifiers(TwoPaneSelectionDialog)
    static const long ID_STATICTEXT1;
    static const long ID_STATICTEXT2;
    static const long ID_LISTBOX1;
    static const long ID_BUTTON1;
    static const long ID_BUTTON2;
    static const long ID_BUTTON3;
    static const long ID_BUTTON6;
    static const long ID_BUTTON7;
    static const long ID_LISTBOX2;
    static const long ID_STATICLINE1;
    static const long ID_BUTTON5;
    static const long ID_BUTTON4;
    //*)

private:
    enum direction
    {
        dirLeft=0,
        dirRight
    };

    //(*Handlers(TwoPaneSelectionDialog)
    void OnButtonAddClick(wxCommandEvent& event);
    void OnButtonRemoveClick(wxCommandEvent& event);
    void OnButtonSeparatorClick(wxCommandEvent& event);
    void OnButtonUpClick(wxCommandEvent& event);
    void OnButtonDownClick(wxCommandEvent& event);
    void OnListBoxLeftDClick(wxCommandEvent& event);
    void OnListBoxRightDClick(wxCommandEvent& event);
    void OnButtonOKClick(wxCommandEvent& event);
    void OnButton5Click(wxCommandEvent& event);
    void OnButtonCancelClick(wxCommandEvent& event);
    //*)

    DECLARE_EVENT_TABLE()

private:
    const std::vector<wxString>& m_left;
    std::vector<wxString>&       m_right;
    void Fill(direction dir, const std::vector<wxString>& list);
    bool GetPosition(const std::vector<wxString>& vec, const wxString& s, int& pos);
    void AddString(direction dir, const wxString& s, int pos);
    void AddString(direction dir, const wxString& s);
    bool RemoveSelection(direction dir);
};

#endif
