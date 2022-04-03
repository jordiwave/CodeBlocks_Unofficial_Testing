/***************************************************************
 * Name:      NSISGUIMain.h
 * Purpose:   Defines Application Frame
 * Author:    GeO (GeO.GeO@live.de)
 * Created:   2009-06-28
 * Copyright: GeO ()
 * License:
 **************************************************************/

#ifndef NSISGUIMAIN_H
#define NSISGUIMAIN_H

//(*Headers(NSISGUIFrame)
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include "NSIFile.h"
#include "CompTreeCtrl.h"

class NSISGUIFrame: public wxDialog
{
    public:

        NSISGUIFrame(wxWindow * parent, wxWindowID id = -1);
        virtual ~NSISGUIFrame();

        void SetPaths(wxString execpath, wxString nsispath);
        void AddToProject(bool yes);

    private:

        //(*Handlers(NSISGUIFrame)
        void OnQuit(wxCommandEvent & event);
        void OnAbout(wxCommandEvent & event);
        void OnCheckListBox1Toggled(wxCommandEvent & event);
        void OnAddFiles(wxCommandEvent & event);
        void OnRemFiles(wxCommandEvent & event);
        void OnCreate(wxCommandEvent & event);
        void OnTCNameText(wxCommandEvent & event);
        void OnCBUConClick(wxCommandEvent & event);
        void OnCBLicClick(wxCommandEvent & event);
        void OnLicPath(wxCommandEvent & event);
        void OnAddComp(wxCommandEvent & event);
        void OnComponentsItemRightClick(wxTreeEvent & event);
        void OnRemComp(wxCommandEvent & event);
        void OnAddSection(wxCommandEvent & event);
        void OnRemSection(wxCommandEvent & event);
        void OnCHSectionSelect(wxCommandEvent & event);
        void OnTCLicText(wxCommandEvent & event);
        void OnSave(wxCommandEvent & event);
        void OnLoad(wxCommandEvent & event);
        void OnCBUUniClick(wxCommandEvent & event);
        void OnCBComClick(wxCommandEvent & event);
        //*)

        //(*Identifiers(NSISGUIFrame)
        static const long ID_BBCREATE;
        static const long ID_BBLOAD;
        static const long ID_BBSAVE;
        static const long ID_STATICTEXT1;
        static const long ID_TCNAME;
        static const long ID_STATICTEXT2;
        static const long ID_TCOUTFILE;
        static const long ID_STATICTEXT3;
        static const long ID_CHINSTPATH;
        static const long ID_STATICTEXT5;
        static const long ID_CBSHORTCUT;
        static const long ID_STATICTEXT4;
        static const long ID_CBLIC;
        static const long ID_CBCOM;
        static const long ID_CBDIR;
        static const long ID_CBINS;
        static const long ID_CBUCON;
        static const long ID_CBUUNI;
        static const long ID_TCLOG;
        static const long ID_PANEL2;
        static const long ID_TCLICPATH;
        static const long ID_BLICPATH;
        static const long ID_TCLIC;
        static const long ID_PANEL1;
        static const long ID_CHSECTION;
        static const long ID_BADDSEC;
        static const long ID_BREMSEC;
        static const long ID_TCCOMP;
        static const long ID_BADDCOMP;
        static const long ID_BREMCOMP;
        static const long ID_PANEL3;
        static const long ID_LBFILES;
        static const long ID_BADDFILE;
        static const long ID_BREMFILE;
        static const long ID_PANEL4;
        static const long ID_NOTEBOOK;
        //*)

        //(*Declarations(NSISGUIFrame)
        wxTextCtrl * TCLic;
        wxTextCtrl * TCLicPath;
        wxBitmapButton * BBCreate;
        wxPanel * Panel4;
        wxCheckBox * CBIns;
        wxButton * BRemFile;
        wxTextCtrl * TCName;
        wxCheckBox * CBUCon;
        wxCheckBox * CBShortcut;
        wxPanel * Panel1;
        wxStaticText * STShortcut;
        wxCheckBox * CBUUni;
        wxChoice * CHSection;
        wxButton * BAddComp;
        wxButton * BAddFile;
        wxPanel * Panel3;
        wxCheckBox * CBDir;
        wxCheckBox * CBCom;
        wxButton * BAddSec;
        wxTextCtrl * TCOutFile;
        wxChoice * CHInstPath;
        wxStaticText * STName;
        wxButton * BRemComp;
        wxStaticText * STOutFile;
        wxBitmapButton * BBLoad;
        wxNotebook * Install;
        wxBitmapButton * BBSave;
        wxListBox * LBFiles;
        wxPanel * Panel2;
        wxTextCtrl * TCLog;
        wxStaticText * STInstPath;
        wxButton * BLicPath;
        wxStaticText * STPages;
        CompTreeCtrl * Components;
        wxCheckBox * CBLic;
        wxButton * BRemSec;
        //*)
        bool CheckParameters();
        wxString m_nsispath;
        wxString m_execpath;
        bool m_addtoproject;
        DECLARE_EVENT_TABLE()
};

#endif // NSISGUIMAIN_H
