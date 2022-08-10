// System include files

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "manager.h"
#include <logmanager.h>

// ProjectExporter include files
#include "PremakeDlg.h"
#include "PremakeExporter.h"

//(*InternalHeaders(PremakeDlg)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(PremakeDlg)
//*)

BEGIN_EVENT_TABLE(PremakeDlg, wxScrollingDialog)
    //(*EventTable(PremakeDlg)
    //*)
END_EVENT_TABLE()

PremakeDlg::PremakeDlg(wxWindow * parent)
{
    //(*Initialize(PremakeDlg)
    wxXmlResource::Get()->LoadObject(this, parent, "PremakeDlg", "wxScrollingDialog");
    CheckBoxReplVars = (wxCheckBox *)FindWindow(XRCID("ID_CHECKBOX1"));
    CheckBoxUpgr = (wxCheckBox *)FindWindow(XRCID("ID_CHECKBOX2"));
    ButtonExport = (wxButton *)FindWindow(XRCID("ID_BUTTON1"));
    ButtonCancel = (wxButton *)FindWindow(XRCID("ID_BUTTON2"));
    Connect(XRCID("ID_BUTTON1"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&PremakeDlg::OnButtonExportClick);
    Connect(XRCID("ID_BUTTON2"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&PremakeDlg::OnButtonCancelClick);
    //*)
}

PremakeDlg::~PremakeDlg()
{
    //(*Destroy(PremakeDlg)
    //*)
}


void PremakeDlg::OnButtonExportClick(wxCommandEvent & event)
{
    PremakeExporter * ExportObject = new PremakeExporter();
    ExportObject->RunExport(CheckBoxReplVars->IsChecked(), CheckBoxUpgr->IsChecked());
    Manager::Get()->GetLogManager()->Log("Premake script exported");
    EndModal(wxID_OK);
}

void PremakeDlg::OnButtonCancelClick(wxCommandEvent & event)
{
    Manager::Get()->GetLogManager()->Log("Premake export canceled");
    EndModal(wxID_CANCEL);
}
