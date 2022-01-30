//******************************************************************************
//* Name:      ConfigDialog.h
//* Purpose:   Configuration dialog
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <vector>
#include "menu.h"


//(*Headers(ConfigDialog)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class ConfigDialog: public wxDialog
{
public:

    ConfigDialog(const std::vector<CBTSVN::MenuEntry>& all_menu_entries, wxWindow* parent,wxWindowID id=wxID_ANY);
    virtual ~ConfigDialog();

    //(*Identifiers(ConfigDialog)
    static const long ID_STATICTEXT1;
    static const long ID_TEXTCTRL1;
    static const long ID_BUTTON2;
    static const long ID_STATICTEXT2;
    static const long ID_TEXTCTRL2;
    static const long ID_BUTTON5;
    static const long ID_PANEL2;
    static const long ID_STATICBOX1;
    static const long ID_STATICBOX2;
    static const long ID_CHECKBOX1;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX2;
    static const long ID_STATICTEXT3;
    static const long ID_STATICBOX3;
    static const long ID_CHECKBOX4;
    static const long ID_STATICBOX4;
    static const long ID_BUTTON6;
    static const long ID_BUTTON7;
    static const long ID_PANEL3;
    static const long ID_BUTTON4;
    static const long ID_STATICTEXT4;
    static const long ID_PANEL1;
    static const long ID_NOTEBOOK1;
    static const long ID_BUTTON1;
    static const long ID_BUTTON3;
    //*)

protected:

    //(*Handlers(ConfigDialog)
    void OncancelButtonClick(wxCommandEvent& event);
    void OnCancelButtonClick(wxCommandEvent& event);
    void OnOkButtonClick(wxCommandEvent& event);
    void OnButtonGetSvnPathClick(wxCommandEvent& event);
    void OnButtonGetTortoiseSVNPathClick(wxCommandEvent& event);
    void OnButtonShowDebugClick(wxCommandEvent& event);
    void OnButtonPopupMenuClick(wxCommandEvent& event);
    void OnButtonMainMenuClick(wxCommandEvent& event);
    //*)

    //(*Declarations(ConfigDialog)
    wxButton* ButtonPopupMenu;
    wxNotebook* Notebook1;
    wxButton* ButtonGetTortoiseSVNPath;
    wxStaticText* StaticText2;
    wxTextCtrl* TextCtrlTortoiseSVNPath;
    wxButton* ButtonGetSvnPath;
    wxCheckBox* CheckBoxEditorIntegration;
    wxCheckBox* CheckBoxMaxIntegration;
    wxPanel* Panel1;
    wxCheckBox* CheckBoxMainMenuIntegration;
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer2;
    wxButton* OkButton;
    wxStaticText* StaticText3;
    wxStaticBox* StaticBox1;
    wxButton* ButtonMainMenu;
    wxStaticBox* StaticBox2;
    wxStaticBox* StaticBox3;
    wxButton* ButtonShowDebug;
    wxPanel* Panel2;
    wxPanel* PanelPath;
    wxCheckBox* CheckBoxProjectManagerIntegration;
    wxTextCtrl* TextCtrlSVNPath;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxStaticText* StaticText4;
    wxButton* CancelButton;
    wxStaticBox* StaticBox4;
    //*)

private:
    const std::vector<CBTSVN::MenuEntry>& m_all_menu_entries;

    DECLARE_EVENT_TABLE()
};

#endif
