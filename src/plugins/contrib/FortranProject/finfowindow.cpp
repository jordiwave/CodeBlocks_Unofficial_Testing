/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Author: Darius Markauskas
 *
 */
#include "finfowindow.h"

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editorcolourset.h>
    #include <manager.h>

    #include <wx/sizer.h>
#endif

#include <cbstyledtextctrl.h>

FInfoWindow::FInfoWindow()
    : wxPanel(Manager::Get()->GetAppWindow())
{
    //ctor
    m_pView = new cbStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(1, 1));
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_pView, 1, wxEXPAND, 0);
    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
    m_pView->SetReadOnly(true);
    // Colorize
    cbEditor::ApplyStyles(m_pView);
    EditorColourSet edColSet;
    edColSet.Apply(edColSet.GetLanguageForFilename("name.f90"), m_pView, false, true);
    SetFoldingIndicator();
    // Creates log image
    wxString prefix(ConfigManager::GetDataFolder() + "/FortranProject.zip#zip:/images/");
#if wxCHECK_VERSION(3, 1, 6)
    wxBitmapBundle * bmp = new wxBitmapBundle(cbLoadBitmapBundleFromSVG(prefix + "svg/info_f.svg", wxSize(16, 16)));
#else
    const int uiSize = Manager::Get()->GetImageSize(Manager::UIComponent::InfoPaneNotebooks);
    prefix << wxString::Format("%dx%d/", uiSize, uiSize);
    wxBitmap * bmp = new wxBitmap(cbLoadBitmap(prefix + "info_f.png", wxBITMAP_TYPE_PNG));
#endif
    CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, this, _("Fortran info"), bmp);
    Manager::Get()->ProcessEvent(evtAdd);
}

FInfoWindow::~FInfoWindow()
{
    //dtor
}

void FInfoWindow::RemoveFromNotebook()
{
    if (Manager::Get()->GetLogManager())
    {
        CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, this);
        Manager::Get()->ProcessEvent(evt);
    }
}

void FInfoWindow::WriteToInfoWindow(const wxString & text)
{
    m_pView->Enable(false);
    m_pView->SetReadOnly(false);
    m_pView->SetText(text);
    m_pView->SetReadOnly(true);
    m_pView->Enable(true);
}

void FInfoWindow::SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back)
{
    m_pView->MarkerDefine(marker, markerType);
    m_pView->MarkerSetForeground(marker, fore);
    m_pView->MarkerSetBackground(marker, back);
}

void FInfoWindow::SetFoldingIndicator()
{
    //simple style
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
}

