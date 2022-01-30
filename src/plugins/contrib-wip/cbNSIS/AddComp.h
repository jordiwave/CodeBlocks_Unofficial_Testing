#ifndef ADDCOMP_H
#define ADDCOMP_H

#ifndef WX_PRECOMP
//(*HeadersPCH(AddComp)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)
#endif
//(*Headers(AddComp)
//*)

#include "CompMap.h"

class AddComp: public wxDialog
{
public:

    AddComp(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~AddComp();

    //(*Declarations(AddComp)
    wxCheckListBox* CLBFiles;
    wxTextCtrl* TCName;
    wxPanel* Panel1;
    wxButton* BCancel;
    wxStaticText* STName;
    wxButton* BOk;
    //*)
    void SetFiles(wxArrayString files);
    CompItem* GetCompItem();
protected:

    //(*Identifiers(AddComp)
    static const long ID_STNAME;
    static const long ID_TCNAME;
    static const long ID_CHECKLISTBOX1;
    static const long ID_PANEL1;
    static const long ID_BOK;
    static const long ID_BCANCEL;
    //*)

private:

    //(*Handlers(AddComp)
    void OnOKClick(wxCommandEvent& event);
    void OnCancelClick(wxCommandEvent& event);
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
