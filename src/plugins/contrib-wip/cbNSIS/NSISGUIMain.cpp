/***************************************************************
 * Name:      NSISGUIMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    GeO (GeO.GeO@live.de)
 * Created:   2009-06-28
 * Copyright: GeO ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "NSISGUIMain.h"

#include <manager.h>
#include <globals.h>
#include <cbproject.h>
#include <projectmanager.h>
#include <projectmanagerui.h>

//(*InternalHeaders(NSISGUIFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "AddComp.h"
#include "NSISGUIFile.h"

#include "create.xpm"
#include "load.xpm"
#include "save.xpm"

//(*IdInit(NSISGUIFrame)
const long NSISGUIFrame::ID_BBCREATE = wxNewId();
const long NSISGUIFrame::ID_BBLOAD = wxNewId();
const long NSISGUIFrame::ID_BBSAVE = wxNewId();
const long NSISGUIFrame::ID_STATICTEXT1 = wxNewId();
const long NSISGUIFrame::ID_TCNAME = wxNewId();
const long NSISGUIFrame::ID_STATICTEXT2 = wxNewId();
const long NSISGUIFrame::ID_TCOUTFILE = wxNewId();
const long NSISGUIFrame::ID_STATICTEXT3 = wxNewId();
const long NSISGUIFrame::ID_CHINSTPATH = wxNewId();
const long NSISGUIFrame::ID_STATICTEXT5 = wxNewId();
const long NSISGUIFrame::ID_CBSHORTCUT = wxNewId();
const long NSISGUIFrame::ID_STATICTEXT4 = wxNewId();
const long NSISGUIFrame::ID_CBLIC = wxNewId();
const long NSISGUIFrame::ID_CBCOM = wxNewId();
const long NSISGUIFrame::ID_CBDIR = wxNewId();
const long NSISGUIFrame::ID_CBINS = wxNewId();
const long NSISGUIFrame::ID_CBUCON = wxNewId();
const long NSISGUIFrame::ID_CBUUNI = wxNewId();
const long NSISGUIFrame::ID_TCLOG = wxNewId();
const long NSISGUIFrame::ID_PANEL2 = wxNewId();
const long NSISGUIFrame::ID_TCLICPATH = wxNewId();
const long NSISGUIFrame::ID_BLICPATH = wxNewId();
const long NSISGUIFrame::ID_TCLIC = wxNewId();
const long NSISGUIFrame::ID_PANEL1 = wxNewId();
const long NSISGUIFrame::ID_CHSECTION = wxNewId();
const long NSISGUIFrame::ID_BADDSEC = wxNewId();
const long NSISGUIFrame::ID_BREMSEC = wxNewId();
const long NSISGUIFrame::ID_TCCOMP = wxNewId();
const long NSISGUIFrame::ID_BADDCOMP = wxNewId();
const long NSISGUIFrame::ID_BREMCOMP = wxNewId();
const long NSISGUIFrame::ID_PANEL3 = wxNewId();
const long NSISGUIFrame::ID_LBFILES = wxNewId();
const long NSISGUIFrame::ID_BADDFILE = wxNewId();
const long NSISGUIFrame::ID_BREMFILE = wxNewId();
const long NSISGUIFrame::ID_PANEL4 = wxNewId();
const long NSISGUIFrame::ID_NOTEBOOK = wxNewId();
//*)

BEGIN_EVENT_TABLE(NSISGUIFrame,wxDialog)
    //(*EventTable(NSISGUIFrame)
    //*)
END_EVENT_TABLE()

NSISGUIFrame::NSISGUIFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(NSISGUIFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer15;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer7;
    wxStaticBoxSizer* SBSInstall;
    wxBoxSizer* BoxSizer8;
    wxBoxSizer* BoxSizer13;
    wxStaticBoxSizer* SBSUninstall;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer11;
    wxBoxSizer* BoxSizer12;
    wxBoxSizer* BoxSizer14;
    wxGridSizer* GridSizer1;
    wxGridSizer* GridSizer3;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer9;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxBoxSizer* BoxSizer3;
    wxGridSizer* GridSizer2;

    Create(parent, wxID_ANY, _("NSISGUI"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(300,282));
    GridSizer1 = new wxGridSizer(0, 0, 0, 0);
    BoxSizer14 = new wxBoxSizer(wxVERTICAL);
    BoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    BBCreate = new wxBitmapButton(this, ID_BBCREATE, wxBitmap(create_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BBCREATE"));
    BBCreate->SetDefault();
    BoxSizer15->Add(BBCreate, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BBLoad = new wxBitmapButton(this, ID_BBLOAD, wxBitmap(load_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BBLOAD"));
    BBLoad->SetDefault();
    BoxSizer15->Add(BBLoad, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BBSave = new wxBitmapButton(this, ID_BBSAVE, wxBitmap(save_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BBSAVE"));
    BBSave->SetDefault();
    BoxSizer15->Add(BBSave, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer14->Add(BoxSizer15, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    GridSizer3 = new wxGridSizer(0, 0, 0, 0);
    Install = new wxNotebook(this, ID_NOTEBOOK, wxDefaultPosition, wxSize(300,300), 0, _T("ID_NOTEBOOK"));
    Panel2 = new wxPanel(Install, ID_PANEL2, wxPoint(73,171), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    STName = new wxStaticText(Panel2, ID_STATICTEXT1, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer3->Add(STName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TCName = new wxTextCtrl(Panel2, ID_TCNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, wxDefaultValidator, _T("ID_TCNAME"));
    TCName->SetToolTip(_("Name of the Installer\nused for Caption..."));
    BoxSizer3->Add(TCName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    STOutFile = new wxStaticText(Panel2, ID_STATICTEXT2, _("Setupfile:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer3->Add(STOutFile, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TCOutFile = new wxTextCtrl(Panel2, ID_TCOUTFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, wxDefaultValidator, _T("ID_TCOUTFILE"));
    TCOutFile->SetToolTip(_("Name of the Outputfile (exe)"));
    BoxSizer3->Add(TCOutFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    STInstPath = new wxStaticText(Panel2, ID_STATICTEXT3, _("Default Installpath:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer5->Add(STInstPath, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CHInstPath = new wxChoice(Panel2, ID_CHINSTPATH, wxDefaultPosition, wxSize(165,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHINSTPATH"));
    CHInstPath->Append(_("Desktop"));
    CHInstPath->SetSelection( CHInstPath->Append(_("Programfiles")) );
    CHInstPath->SetToolTip(_("Default Installpath\n(check Directory if you want\n to be select from the user)"));
    BoxSizer5->Add(CHInstPath, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    STShortcut = new wxStaticText(Panel2, ID_STATICTEXT5, _("Shortcut:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    BoxSizer5->Add(STShortcut, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CBShortcut = new wxCheckBox(Panel2, ID_CBSHORTCUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBSHORTCUT"));
    CBShortcut->SetValue(false);
    CBShortcut->SetToolTip(_("Creates a Shortcut on the Desktop\n(only if a .exe is in the Installfiles)"));
    BoxSizer5->Add(CBShortcut, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    STPages = new wxStaticText(Panel2, ID_STATICTEXT4, _("Pages:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    BoxSizer6->Add(STPages, 0, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    SBSInstall = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Install"));
    CBLic = new wxCheckBox(Panel2, ID_CBLIC, _("License"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBLIC"));
    CBLic->SetValue(false);
    CBLic->SetToolTip(_("Check to show a Licensepage\n(for inserting a License look page \"License\")"));
    SBSInstall->Add(CBLic, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CBCom = new wxCheckBox(Panel2, ID_CBCOM, _("Components"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBCOM"));
    CBCom->SetValue(false);
    CBCom->SetToolTip(_("Check to show a Componentspage\n(for inserting Components look page \"Components\")"));
    SBSInstall->Add(CBCom, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CBDir = new wxCheckBox(Panel2, ID_CBDIR, _("Directory"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBDIR"));
    CBDir->SetValue(false);
    CBDir->SetToolTip(_("Check to show a Directorypage\nand let the user select the Installpath"));
    SBSInstall->Add(CBDir, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CBIns = new wxCheckBox(Panel2, ID_CBINS, _("Install Files"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBINS"));
    CBIns->SetValue(true);
    CBIns->Disable();
    CBIns->SetToolTip(_("Always checked\n(You must install something!)\n(for inserting Components look page \"Files\")"));
    SBSInstall->Add(CBIns, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer8->Add(SBSInstall, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SBSUninstall = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Uninstall"));
    CBUCon = new wxCheckBox(Panel2, ID_CBUCON, _("Confirm"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBUCON"));
    CBUCon->SetValue(false);
    CBUCon->SetToolTip(_("Check if you want a Confirmpage on Uninstall"));
    SBSUninstall->Add(CBUCon, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CBUUni = new wxCheckBox(Panel2, ID_CBUUNI, _("Uninstall Files"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CBUUNI"));
    CBUUni->SetValue(false);
    CBUUni->SetToolTip(_("Check if you want to create an Uninstaller"));
    SBSUninstall->Add(CBUUni, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer8->Add(SBSUninstall, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6->Add(BoxSizer8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer2->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("Log"));
    TCLog = new wxTextCtrl(Panel2, ID_TCLOG, wxEmptyString, wxDefaultPosition, wxSize(282,38), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TCLOG"));
    TCLog->SetToolTip(_("Displays the Output of NSISMAKE"));
    StaticBoxSizer1->Add(TCLog, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer2->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel2->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel2);
    BoxSizer2->SetSizeHints(Panel2);
    Panel1 = new wxPanel(Install, ID_PANEL1, wxPoint(121,7), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    TCLicPath = new wxTextCtrl(Panel1, ID_TCLICPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, wxDefaultValidator, _T("ID_TCLICPATH"));
    TCLicPath->SetToolTip(_("Select a Path to License\n(Should be a .txt Textfile)"));
    BoxSizer9->Add(TCLicPath, 1, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BLicPath = new wxButton(Panel1, ID_BLICPATH, _("..."), wxDefaultPosition, wxSize(24,23), 0, wxDefaultValidator, _T("ID_BLICPATH"));
    BLicPath->SetToolTip(_("Select a Path to License\n(Should be a .txt Textfile)"));
    BoxSizer9->Add(BLicPath, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer9, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    TCLic = new wxTextCtrl(Panel1, ID_TCLIC, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TCLIC"));
    TCLic->SetToolTip(_("or insert your License here"));
    BoxSizer4->Add(TCLic, 1, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer4);
    BoxSizer4->Fit(Panel1);
    BoxSizer4->SetSizeHints(Panel1);
    Panel3 = new wxPanel(Install, ID_PANEL3, wxPoint(105,7), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer10 = new wxBoxSizer(wxVERTICAL);
    BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    CHSection = new wxChoice(Panel3, ID_CHSECTION, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHSECTION"));
    CHSection->SetToolTip(_("Select your Installtyp"));
    BoxSizer13->Add(CHSection, 1, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BAddSec = new wxButton(Panel3, ID_BADDSEC, _("+"), wxDefaultPosition, wxSize(30,23), 0, wxDefaultValidator, _T("ID_BADDSEC"));
    BAddSec->SetToolTip(_("The current selection \n(the items checked in the Componentstree) \nwill be saved as a new Installtyp."));
    BoxSizer13->Add(BAddSec, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BRemSec = new wxButton(Panel3, ID_BREMSEC, _("-"), wxDefaultPosition, wxSize(30,23), 0, wxDefaultValidator, _T("ID_BREMSEC"));
    BRemSec->SetToolTip(_("The current Installtyp will be deleted"));
    BoxSizer13->Add(BRemSec, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer10->Add(BoxSizer13, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    Components = new CompTreeCtrl(Panel3, ID_TCCOMP, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HIDE_ROOT|wxTR_SINGLE|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TCCOMP"));
    Components->SetToolTip(_("Click + to add and - to remove Components\n(Rightclick to unselect)"));
    Components->AddRoot(_("root"));
    BoxSizer11->Add(Components, 4, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer12 = new wxBoxSizer(wxVERTICAL);
    GridSizer2 = new wxGridSizer(2, 0, 0, 0);
    BAddComp = new wxButton(Panel3, ID_BADDCOMP, _("+"), wxDefaultPosition, wxSize(26,23), 0, wxDefaultValidator, _T("ID_BADDCOMP"));
    BAddComp->SetToolTip(_("Click + to add and - to remove Components\n(Rightclick to unselect)"));
    GridSizer2->Add(BAddComp, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BRemComp = new wxButton(Panel3, ID_BREMCOMP, _("-"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BREMCOMP"));
    BRemComp->SetToolTip(_("Click + to add and - to remove Components\n(Rightclick to unselect)"));
    GridSizer2->Add(BRemComp, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer12->Add(GridSizer2, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer11->Add(BoxSizer12, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer10->Add(BoxSizer11, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel3->SetSizer(BoxSizer10);
    BoxSizer10->Fit(Panel3);
    BoxSizer10->SetSizeHints(Panel3);
    Panel4 = new wxPanel(Install, ID_PANEL4, wxPoint(158,10), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    LBFiles = new wxListBox(Panel4, ID_LBFILES, wxDefaultPosition, wxDefaultSize, 0, 0, wxLB_SINGLE, wxDefaultValidator, _T("ID_LBFILES"));
    LBFiles->SetToolTip(_("Click + to add and - to remove Files"));
    BoxSizer1->Add(LBFiles, 4, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer7 = new wxBoxSizer(wxVERTICAL);
    BAddFile = new wxButton(Panel4, ID_BADDFILE, _("+"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BADDFILE"));
    BAddFile->SetToolTip(_("Click + to add and - to remove Files"));
    BoxSizer7->Add(BAddFile, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BRemFile = new wxButton(Panel4, ID_BREMFILE, _("-"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BREMFILE"));
    BRemFile->SetToolTip(_("Click + to add and - to remove Files"));
    BoxSizer7->Add(BRemFile, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel4->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel4);
    BoxSizer1->SetSizeHints(Panel4);
    Install->AddPage(Panel2, _("Start"), true);
    Install->AddPage(Panel1, _("License"), false);
    Install->AddPage(Panel3, _("Components"), false);
    Install->AddPage(Panel4, _("Files"), false);
    GridSizer3->Add(Install, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer14->Add(GridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    GridSizer1->Add(BoxSizer14, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(GridSizer1);
    GridSizer1->SetSizeHints(this);

    Connect(ID_BBCREATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnCreate);
    Connect(ID_BBLOAD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnLoad);
    Connect(ID_BBSAVE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnSave);
    Connect(ID_TCNAME,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&NSISGUIFrame::OnTCNameText);
    Connect(ID_CBLIC,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnCBLicClick);
    Connect(ID_CBCOM,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnCBComClick);
    Connect(ID_CBUCON,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnCBUConClick);
    Connect(ID_CBUUNI,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnCBUUniClick);
    Connect(ID_TCLICPATH,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&NSISGUIFrame::OnTCLicText);
    Connect(ID_BLICPATH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnLicPath);
    Connect(ID_TCLIC,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&NSISGUIFrame::OnTCLicText);
    Connect(ID_CHSECTION,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&NSISGUIFrame::OnCHSectionSelect);
    Connect(ID_BADDSEC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnAddSection);
    Connect(ID_BREMSEC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnRemSection);
    Connect(ID_TCCOMP,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&NSISGUIFrame::OnComponentsItemRightClick);
    Connect(ID_BADDCOMP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnAddComp);
    Connect(ID_BREMCOMP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnRemComp);
    Connect(ID_BADDFILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnAddFiles);
    Connect(ID_BREMFILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NSISGUIFrame::OnRemFiles);
    //*)
    Install->GetPage(1)->Enable(false);
    Install->GetPage(2)->Enable(false);
}

NSISGUIFrame::~NSISGUIFrame()
{
    //(*Destroy(NSISGUIFrame)
    //*)
}

void NSISGUIFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void NSISGUIFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(_("About NSISGUI"),_(""));
}

void NSISGUIFrame::SetPaths(wxString execpath,wxString nsispath)
{
    m_nsispath = nsispath;
    m_execpath = execpath;
}

void NSISGUIFrame::AddToProject(bool yes)
{
    m_addtoproject = yes;
}

void NSISGUIFrame::OnCreate(wxCommandEvent& event)
{
    if (!CheckParameters())return;
    NSIFile* file = new NSIFile(TCName->GetValue(),m_execpath,m_nsispath);
    wxArrayInt selections;
    if (CBLic->IsChecked())selections.Add(0);
    if (CBCom->IsChecked())selections.Add(1);
    if (CBDir->IsChecked())selections.Add(2);
    if (CBIns->IsChecked())selections.Add(3);
    if (CBUCon->IsChecked())selections.Add(4);
    if (CBUUni->IsChecked())selections.Add(5);
    file->SetAll(TCOutFile->GetValue(),CHInstPath->GetStringSelection(),selections,LBFiles->GetStrings(),CBShortcut->IsChecked());
    if (CBLic->IsChecked())
    {
        if (!TCLicPath->IsEmpty())
            file->SetLicense(TCLicPath->GetValue());
        else
        {
            if (wxFileExists(_T("license_tiaulid.txt")))wxRemoveFile(_T("license_tiaulid.txt"));
            wxFile* licf = new wxFile();
            licf->Create(_T("license_tiaulid.txt"));
            licf->Write(TCLic->GetValue());
            licf->Close();
            file->SetLicense(_T("license_tiaulid.txt"));
        }
    }
    file->SetSections(CHSection->GetStrings());
    file->SetComponent(Components->GetComponents());
    file->Create();
    if(m_addtoproject)
    {
        wxString nsisfile = TCName->GetValue() + _T(".nsi");
        cbProject* actproj = Manager::Get()->GetProjectManager()->GetActiveProject();
        actproj->BeginAddFiles();
        actproj->AddFile(actproj->GetActiveBuildTarget(),nsisfile,false,false);
        actproj->EndAddFiles();

        Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
        // Manager::Get()->GetProjectManager()->RebuildTree();
        actproj->AddCommandsAfterBuild(_T("$(#nsis)/makensis ") + nsisfile);
        actproj->SetAlwaysRunPostBuildSteps(true);
    }
    wxArrayString ar = file->GetOutput();
    for (unsigned int i=0; i<ar.GetCount(); i++)
        TCLog->AppendText(_T("\n") + ar[i]);
    Install->SetSelection(0);
    TCLog->ShowPosition(TCLog->GetLastPosition());
    if(ar[ar.GetCount()-1].Find(_T("Total size:"))!=wxNOT_FOUND)
        cbMessageBox(_("Build ended successfully!"));
    else
        cbMessageBox(_("An error occurred!"));
}

void NSISGUIFrame::OnTCNameText(wxCommandEvent& event)
{
    TCOutFile->SetValue(TCName->GetValue() + _T(".exe"));
}

void NSISGUIFrame::OnCBUConClick(wxCommandEvent& event)
{
    if (CBUCon->IsChecked())
        CBUUni->SetValue(true);
}

void NSISGUIFrame::OnCBUUniClick(wxCommandEvent& event)
{
    if (!CBUUni->IsChecked())
        CBUCon->SetValue(false);
}

void NSISGUIFrame::OnCBLicClick(wxCommandEvent& event)
{
    Install->GetPage(1)->Enable(CBLic->IsChecked());
}

void NSISGUIFrame::OnCBComClick(wxCommandEvent& event)
{
    Install->GetPage(2)->Enable(CBCom->IsChecked());
}

void NSISGUIFrame::OnLicPath(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,                                  // wxWindow *  	parent,
                                         _("Open License File"),                 // const wxString &  	message = wxFileSelectorPromptStr,
                                         _T(""),                                // const wxString &  	defaultDir = wxEmptyString,
                                         _T(""),                                // const wxString &  	defaultFile = wxEmptyString,
                                         _T("Textfile (*.txt)|*.txt"),          // const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
                                         wxFD_OPEN | wxFD_FILE_MUST_EXIST );   // long  	style = wxFD_DEFAULT_STYLE,
    if (dlg->ShowModal()==wxID_OK)
        TCLicPath->SetValue(dlg->GetPath());
}


void NSISGUIFrame::OnAddFiles(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,                                  // wxWindow *  	parent,
                                         _("Add Installfile(s)"),               // const wxString &  	message = wxFileSelectorPromptStr,
                                         _T(""),                                // const wxString &  	defaultDir = wxEmptyString,
                                         _T(""),                                // const wxString &  	defaultFile = wxEmptyString,
                                         wxFileSelectorDefaultWildcardStr,      // const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
                                         wxFD_MULTIPLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST);  // long  	style = wxFD_DEFAULT_STYLE,

    if (dlg->ShowModal()==wxID_OK)
    {
        wxArrayString ar;
        dlg->GetPaths(ar);
        LBFiles->InsertItems(ar,0);
    }
}

void NSISGUIFrame::OnRemFiles(wxCommandEvent& event)
{
    LBFiles->Delete(LBFiles->GetSelection());
}

void NSISGUIFrame::OnAddComp(wxCommandEvent& event)
{
    if (!CBCom->IsChecked())
        cbMessageBox(_("Caution: If you want to display the Componentspage\ncheck the Componentsbox on the Startpage!"));
    if (LBFiles->GetStrings().GetCount() <= 0 )
    {
        cbMessageBox(_("No Installfiles specified!"));
        return;
    }
    AddComp* dlg = new AddComp(this);
    dlg->SetFiles(LBFiles->GetStrings());
    if (dlg->ShowModal()==wxID_OK)
        Components->AddComp(dlg->GetCompItem());
}

void NSISGUIFrame::OnRemComp(wxCommandEvent& event)
{
    Components->RemComp();
}

void NSISGUIFrame::OnComponentsItemRightClick(wxTreeEvent& event)
{
    Components->Unselect();
}

void NSISGUIFrame::OnAddSection(wxCommandEvent& event)
{
    if (!CBCom->IsChecked())
        cbMessageBox(_("Caution: If you want to display the Componentspage\ncheck the Componentsbox on the Startpage!"));
    if(Components->GetCount()<1)
    {
        cbMessageBox(_("Add Components to create a Installtype"));
        return;
    }
    wxString name = wxGetTextFromUser(_("Name of the Section:"));
    if (name != wxEmptyString)
    {
        CHSection->AppendString(name);
        Components->AddSection(CHSection->GetCount()-1);
        CHSection->SetSelection(CHSection->GetCount()-1);
    }
}

void NSISGUIFrame::OnRemSection(wxCommandEvent& event)
{
    int sel = CHSection->GetSelection();
    Components->RemoveSection(sel);
    CHSection->Delete(sel);
    CHSection->SetSelection(sel-1);
    Components->ChangeSection(CHSection->GetSelection());
}

void NSISGUIFrame::OnCHSectionSelect(wxCommandEvent& event)
{
    Components->ChangeSection(CHSection->GetSelection());
}

bool NSISGUIFrame::CheckParameters()
{
    if (TCName->IsEmpty())
    {
        cbMessageBox(_("No Name specified!"));
        return false;
    }
    if (TCOutFile->IsEmpty())
    {
        cbMessageBox(_("No Filename specified!"));
        return false;
    }
    if (CHInstPath->GetStringSelection() == wxEmptyString)
    {
        cbMessageBox(_("No Installpath specified!"));
        return false;
    }
    if (CBLic->IsChecked()&&TCLic->IsEmpty()&&TCLicPath->IsEmpty())
    {
        cbMessageBox(_("No License specified!"));
        return false;
    }
    if (CBCom->IsChecked()&&(Components->GetCount() <= 0))
    {
        cbMessageBox(_("No Components specified!"));
        return false;
    }
    if (LBFiles->GetStrings().GetCount() <= 0 )
    {
        cbMessageBox(_("No Installfiles specified!"));
        return false;
    }
    return true;
}

void NSISGUIFrame::OnTCLicText(wxCommandEvent& event)
{
    if (!CBLic->IsChecked())
        cbMessageBox(_("Caution: If you want to display the License\ncheck the Licensebox on the Startpage!"));
}

void NSISGUIFrame::OnSave(wxCommandEvent& event)
{
    if(TCName->GetValue()==wxEmptyString)
    {
        cbMessageBox(_("Please insert a Name"));
        return;
    }
    NGFOptions opt;
    opt.Name = TCName->GetValue();
    opt.OutputName = TCOutFile->GetValue();
    opt.InstallPath = CHInstPath->GetSelection();
    opt.Shortcut = CBShortcut->IsChecked();
    opt.License = CBLic->IsChecked();
    opt.Components = CBCom->IsChecked();
    opt.Directory = CBDir->IsChecked();
    opt.ConfirmUI = CBUCon->IsChecked();
    opt.Uninstall = CBUUni->IsChecked();

    opt.LicPath = TCLicPath->GetValue();
    opt.LicText = TCLic->GetValue();

    opt.InstallTypes = CHSection->GetStrings();
    opt.Items = Components->GetComponents();
    opt.Files = LBFiles->GetStrings();
    NSISGUIFile* file = new NSISGUIFile();
    file->Save(opt);
}

void NSISGUIFrame::OnLoad(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,                                  // wxWindow *  	parent,
                                         _("Open NSISGUI File"),                // const wxString &  	message = wxFileSelectorPromptStr,
                                         _T(""),                                // const wxString &  	defaultDir = wxEmptyString,
                                         _T(""),                                // const wxString &  	defaultFile = wxEmptyString,
                                         _T("NSISGUI (*.ngf)|*.ngf"),           // const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
                                         wxFD_OPEN  | wxFD_FILE_MUST_EXIST );   // long style = wxFD_DEFAULT_STYLE,
    if (dlg->ShowModal()==wxID_OK)
    {
        NSISGUIFile* file = new NSISGUIFile();
        NGFOptions opt = file->Load(dlg->GetPath());
        TCName->SetValue(opt.Name);
        TCOutFile->SetValue(opt.OutputName);
        CHInstPath->SetSelection(opt.InstallPath);
        CBShortcut->SetValue(opt.Shortcut);
        CBLic->SetValue(opt.License);
        CBCom->SetValue(opt.Components);
        CBDir->SetValue(opt.Directory);
        CBUCon->SetValue(opt.ConfirmUI);
        CBUUni->SetValue(opt.Uninstall);
        if(opt.LicPath!=wxEmptyString)
            TCLicPath->SetValue(opt.LicPath);
        if(opt.LicText!=wxEmptyString)
            TCLic->SetValue(opt.LicText);
        CHSection->Clear();
        for (unsigned int i = 0; i<opt.InstallTypes.GetCount(); i++)
            CHSection->Append(opt.InstallTypes[i]);
        LBFiles->Clear();
        for (unsigned int i = 0; i<opt.Files.GetCount(); i++)
            LBFiles->Append(opt.Files[i]);
        Components->SetComponents(opt.Items);
    }
}
