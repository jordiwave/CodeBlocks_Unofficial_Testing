#ifndef CUSTOMCONFIGDIALOG_H
#define CUSTOMCONFIGDIALOG_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(CustomConfigDialog)
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    #include <wx/checkbox.h>
    #include <wx/panel.h>
    #include <wx/button.h>
    #include <wx/dialog.h>
    //*)
#endif
//(*Headers(CustomConfigDialog)
#include <wx/notebook.h>
//*)

class CustomConfigDialog: public wxDialog
{
    public:

        CustomConfigDialog(wxWindow * parent, wxWindowID id = wxID_ANY);
        virtual ~CustomConfigDialog();

        //(*Declarations(CustomConfigDialog)
        wxStaticText * StaticTextCustomProject;
        wxStaticText * StaticTextCustomProjectAbsolute;
        wxButton * ButtonCustomWorkspace;
        wxNotebook * Notebook1;
        wxCheckBox * CheckBoxCustomWorkspaceRelative;
        wxButton * ButtonCustomProject;
        wxTextCtrl * TextCtrlCustomWorkspace;
        wxButton * ButtonOK;
        wxTextCtrl * TextCtrlCustomWorkspaceAbsolute;
        wxCheckBox * CheckBoxCustomProjectRelative;
        wxPanel * Panel1;
        wxTextCtrl * TextCtrlCustomProjectAbsolute;
        wxStaticText * StaticTextCustomWorkspace;
        wxButton * ButtonCancel;
        wxPanel * Panel2;
        wxTextCtrl * TextCtrlCustomProject;
        wxStaticText * StaticTextCustomWorkspaceAbsolute;
        //*)

    protected:

        //(*Identifiers(CustomConfigDialog)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON3;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL3;
        static const long ID_CHECKBOX1;
        static const long ID_PANEL1;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT2;
        static const long ID_BUTTON5;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL4;
        static const long ID_CHECKBOX2;
        static const long ID_PANEL2;
        static const long ID_NOTEBOOK1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        //*)

    private:

        //(*Handlers(CustomConfigDialog)
        void OnButtonCancelClick(wxCommandEvent & event);
        void OnButtonOKClick(wxCommandEvent & event);
        void OnButtonCustomProjectClick(wxCommandEvent & event);
        void OnButtonCustomWorkspaceClick(wxCommandEvent & event);
        void OnCheckBoxCustomProjectRelativeClick(wxCommandEvent & event);
        void OnCheckBoxCustomWorkspaceRelativeClick(wxCommandEvent & event);
        //*)

        DECLARE_EVENT_TABLE()

        void UpdateAbsoluteRelative(wxTextCtrl & ctrl, const wxString & basedir, bool relative);
        bool GetDir(wxString & newdir, const wxString & olddir, const wxString & basedir);
};

#endif
