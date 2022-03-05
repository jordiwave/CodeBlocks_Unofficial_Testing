/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */
#define SUPPORT_COLOR

#include "sdk_precomp.h"
#include "autodetectcompilers.h"

#ifndef CB_PRECOMP
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

#include "compiler.h"
#include "compilerfactory.h"
#include "configmanager.h"
#include "logmanager.h"
#include "manager.h"
#include "macrosmanager.h"
#endif
#include <wx/settings.h>
#include <wx/tooltip.h>

#include "infowindow.h"

BEGIN_EVENT_TABLE(AutoDetectCompilers, wxScrollingDialog)
    EVT_UPDATE_UI(-1, AutoDetectCompilers::OnUpdateUI)
    EVT_BUTTON(XRCID("btnSetDefaultCompiler"), AutoDetectCompilers::OnDefaultCompilerClick)
    EVT_RADIOBOX(XRCID("rbCompilerShowOptions"), AutoDetectCompilers::OnUdateCompilerListUI)
    EVT_CLOSE(AutoDetectCompilers::OnClose)
END_EVENT_TABLE()

AutoDetectCompilers::AutoDetectCompilers(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, "dlgAutoDetectCompilers", "wxScrollingDialog");
    XRCCTRL(*this, "wxID_OK", wxButton)->SetDefault();
    Bind(wxEVT_BUTTON, &AutoDetectCompilers::OnCloseClicked, this, wxID_OK);

    wxString defaultCompilerID = CompilerFactory::GetDefaultCompilerID();
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        LogManager *logMgr = Manager::Get()->GetLogManager();
        list->Connect(wxEVT_MOTION, wxMouseEventHandler(AutoDetectCompilers::OnMouseMotion));
        list->ClearAll();
        list->InsertColumn(ccnNameColumn, _("Compiler"), wxLIST_FORMAT_LEFT, 380);
        list->InsertColumn(ccnStatusColumn, _("Status"), wxLIST_FORMAT_LEFT, 100);
        list->InsertColumn(ccnDetectedPathColumn, _("Compiler Path"), wxLIST_FORMAT_LEFT, 200);

        logMgr->Log("AutoDetectCompilers info follows:");
        const size_t count = CompilerFactory::GetCompilersCount();
        for (size_t i = 0; i < count; ++i)
        {
            Compiler* compiler = CompilerFactory::GetCompiler(i);
            if (!compiler)
                continue;

            CompilerItem compilerListItem;
            compilerListItem.compilerName = compiler->GetName();
            compilerListItem.status = _("Unknown");       	// Default to Unknown
            compilerListItem.compilerPath = wxString();     // Default to empty string
            compilerListItem.detected = false;            	// Default to false

            wxString currentCompilerID = compiler->GetID();

            // Only check if an actual compiler, ignore "NO compiler"
            if (!currentCompilerID.IsSameAs("null"))
            {
                wxString masterPath = compiler->GetMasterPath();
                wxString masterPathNoMacros = masterPath;
                Manager::Get()->GetMacrosManager()->ReplaceMacros(masterPathNoMacros);

                const bool detectedDirectory = wxFileName::DirExists(masterPathNoMacros);
                const bool compilerIsDefault = defaultCompilerID.IsSameAs(currentCompilerID);
                // bool compilerSetConfigured = Manager::Get()->GetConfigManager("compiler")->Exists("/sets/" + compiler->GetID() + "/name");

                logMgr->Log(wxString::Format("CompilerName : '%s'   , masterPathNoMacros : '%s'", compilerListItem.compilerName, masterPathNoMacros));
                logMgr->Log(wxString::Format("Manager::Get()->GetConfigManager(\"compiler\")->Exists('%s') : %s", "/sets/" + compiler->GetID() + "/name",
                                             Manager::Get()->GetConfigManager("compiler")->Exists("/sets/" + compiler->GetID() + "/name")?"True":"False"));


                if (detectedDirectory)
                {
                    compilerListItem.status = _("Detected");
                    compilerListItem.detected = true;
                    compilerListItem.compilerPath = masterPathNoMacros;
                    compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
                }
                else
                {
                    // Path is invalid. Try auto-detection
                    const bool detectedAutoDetect = compiler->AutoDetectInstallationDir() == adrDetected;
                    wxString pathDetected = compiler->GetMasterPath();
                    wxString pathDetectedNoMacros = pathDetected;
                    Manager::Get()->GetMacrosManager()->ReplaceMacros(pathDetectedNoMacros);

                    logMgr->Log(wxString::Format("AutoDetectInstallationDir: pathDetected : '%s' , pathDetectedNoMacros : '%s'", pathDetected, pathDetectedNoMacros ));

                    //Revert the detected path back to the original path!!!!
                    if (!masterPath.empty())
                        compiler->SetMasterPath(masterPath);

                    // In case auto-detection was successful:
                    if (detectedAutoDetect)
                    {
                        if (wxFileName::DirExists(pathDetectedNoMacros))
                        {
                            if (masterPathNoMacros.empty())
                            {
                                compilerListItem.status = _("New compiler detected.");
                            }
                            else
                            {
                                compilerListItem.status = _("Compiler path changed.");
                            }
                            compilerListItem.detected = true;
                            compilerListItem.compilerPath = pathDetectedNoMacros;
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightYellow;
                        }
                        else
                        {
                            if (compiler->GetParentID().empty()) // built-in compiler
                                compilerListItem.status = _("Detected");
                            else
                                compilerListItem.status = _("User defined detected");

                            compilerListItem.detected = true;
                            compilerListItem.compilerPath = pathDetected;
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
                        }
                    }
                    else
                    {
                        compilerListItem.detected = false;
                        if (compilerIsDefault)
                        {
                            if (compiler->GetParentID().empty()) // built-in compiler
                                compilerListItem.status = _("Default compiler not detected");
                            else
                                compilerListItem.status = _("Default compiler user defined not detected");
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightRed;

                        }
                        else
                        {
                            if (compiler->GetParentID().empty()) // built-in compiler
                                compilerListItem.status = _("Not detected");
                            else
                                compilerListItem.status = _("User defined not detected");
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGrey;
                        }
                    }
                }
            }
            else
            {
                compilerListItem.status = _("No compiler");
                compilerListItem.compilerPath = _("N/A");
                compilerListItem.detected = true;
                compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
            }

            if (defaultCompilerID.IsSameAs(currentCompilerID))
            {
                wxStaticText* controlLblDefCompiler = XRCCTRL(*this, "lblDefCompiler", wxStaticText);
                if (compilerListItem.detected)
                {
                    controlLblDefCompiler->SetLabel(compilerListItem.compilerName);
#ifdef SUPPORT_COLOR
                    controlLblDefCompiler->SetForegroundColour(XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetForegroundColour());
                    controlLblDefCompiler->SetBackgroundColour(XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetBackgroundColour());
#else
                    if (controlLblDefCompiler->GetFont().GetStrikethrough ())
                    {
                        wxFont font = controlLblDefCompiler->GetFont();
                        font.SetStrikethrough(false);
                        controlLblDefCompiler->SetFont(font);
                    }
#endif
                }
                else
                {
                    controlLblDefCompiler->SetLabel(wxString::Format(_("INVALID: %s"),compilerListItem.compilerName));
#ifdef SUPPORT_COLOR
                    controlLblDefCompiler->SetForegroundColour(wxColour(*wxWHITE));
                    controlLblDefCompiler->SetBackgroundColour(wxColour(*wxRED));
#else
                    controlLblDefCompiler->SetFont(controlLblDefCompiler->GetFont().MakeStrikethrough());
#endif
                }
            }

            logMgr->Log(wxString::Format("Compiler '%s' , status: '%s' , compilerPath: '%s' , detected: %s",
                                         compilerListItem.compilerName,
                                         compilerListItem.status,
                                         compilerListItem.compilerPath,
                                         compilerListItem.detected?"True":"False"
                                        ));
            m_CompilerList.push_back(compilerListItem);
        }
        UpdateCompilerDisplayList();
    }
}

AutoDetectCompilers::~AutoDetectCompilers()
{
    //dtor
}

bool AutoDetectCompilers::closeCheckOkay()
{
    wxString defaultCompiler = CompilerFactory::GetDefaultCompiler()->GetName();

    for (std::vector<CompilerItem>::iterator it = m_CompilerList.begin(); it != m_CompilerList.end(); ++it)
    {
        // Find default compiler in the list and if it was not detected double check with the user that this is okay.
        if (it->compilerName.IsSameAs(defaultCompiler) && !it->detected)
        {
            if (wxMessageBox(_("Are you sure you want to configure CodeBlocks for an invalid compiler?"), "",wxYES_NO) == wxNO)
            {
                return false;
            }
        }
    }
    return true;
}

void AutoDetectCompilers::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto())
    {
        if (!closeCheckOkay())
        {
            event.Veto();
            return;
        }
    }
    event.Skip(); // the default event handler does call Destroy()
}

void AutoDetectCompilers::OnCloseClicked(wxCommandEvent& event)
{
    if (closeCheckOkay())
    {
        EndModal(0); // DO NOT call Destroy(); as it will cause an exception!
    }
    else
    {
        event.Skip();
    }
}

void AutoDetectCompilers::OnDefaultCompilerClick(cb_unused wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int idxList = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxList != -1)
    {
        wxString wsSelection = list->GetItemText(idxList);
        Compiler* compiler = CompilerFactory::GetCompilerByName(wsSelection);
        int idxComiler = CompilerFactory::GetCompilerIndex(compiler);

        CompilerFactory::SetDefaultCompiler(idxComiler);

        wxStaticText* controlLblDefCompiler = XRCCTRL(*this, "lblDefCompiler", wxStaticText);
        controlLblDefCompiler->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
#ifdef SUPPORT_COLOR
        controlLblDefCompiler->SetForegroundColour(XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetForegroundColour());
        controlLblDefCompiler->SetBackgroundColour(XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetBackgroundColour());
#endif
    }
}

void AutoDetectCompilers::OnUdateCompilerListUI(cb_unused wxCommandEvent& event)
{
    UpdateCompilerDisplayList();
}

void AutoDetectCompilers::UpdateCompilerDisplayList()
{
    bool bShowAllCompilerOptions = (XRCCTRL(*this, "rbCompilerShowOptions", wxRadioBox)->GetSelection() == 1);

    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->ClearAll();
        list->InsertColumn(ccnNameColumn, _("Compiler"), wxLIST_FORMAT_LEFT, 380);
        list->InsertColumn(ccnStatusColumn, _("Status"), wxLIST_FORMAT_LEFT, 100);
        list->InsertColumn(ccnDetectedPathColumn, _("Compiler Path"), wxLIST_FORMAT_LEFT, 200);

        for (std::vector<CompilerItem>::iterator it = m_CompilerList.begin(); it != m_CompilerList.end(); ++it)
        {
            if (bShowAllCompilerOptions && !it->detected)
                continue;

            int idx = list->GetItemCount();
            idx = list->InsertItem(idx, it->compilerName);
            list->SetItem(idx, ccnStatusColumn, it->status);
            list->SetItem(idx, ccnDetectedPathColumn, it->compilerPath);

            wxColour colourText = wxNullColour;
            wxColour colourBackground = wxNullColour;
            switch(it->colorHighlight)
            {
            case CompilerHighlightColor::colorHighlightGrey:
#ifndef SUPPORT_COLOR
                list->SetItemFont(idx, list->GetItemFont(idx).MakeItalic());
#endif
                colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                colourText  = wxColour(*wxLIGHT_GREY);
                break;
            case CompilerHighlightColor::colorHighlightGreen:
#ifndef SUPPORT_COLOR
                list->SetItemFont(idx, list->GetItemFont(idx).MakeBold());
#endif
                colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                colourBackground  = wxColour(*wxGREEN);
                break;
            case CompilerHighlightColor::colorHighlightRed:
#ifndef SUPPORT_COLOR
                list->SetItemFont(idx, list->GetItemFont(idx).MakeStrikethrough());
#endif
                colourText  = wxColour(*wxWHITE);
                colourBackground  = wxColour(*wxRED);
                break;
            case CompilerHighlightColor::colorHighlightYellow:
#ifndef SUPPORT_COLOR
                list->SetItemFont(idx, list->GetItemFont(idx).MakeUnderlined());
#endif
                colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                colourBackground  = wxColour(*wxYELLOW);
                break;
            case CompilerHighlightColor::colorHighlightNone:
            default:
                // colourText = wxNullColour;
                // colourBackground = wxNullColour;
                break;
            }
#ifdef SUPPORT_COLOR
            list->SetItemTextColour(idx, colourText);
            list->SetItemBackgroundColour(idx, colourBackground);

#endif
        }

        // Resize columns so one can read the whole stuff:
        list->SetColumnWidth(ccnNameColumn, wxLIST_AUTOSIZE);
        list->SetColumnWidth(ccnStatusColumn, wxLIST_AUTOSIZE);
        list->SetColumnWidth(ccnDetectedPathColumn, wxLIST_AUTOSIZE);
    }
}

void AutoDetectCompilers::OnMouseMotion(wxMouseEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int flags = 0;
    int idx = list->HitTest(event.GetPosition(), flags);
    wxString txt = wxEmptyString;
    if (idx != wxNOT_FOUND)
    {
        wxListItem itm;
        itm.SetId(idx);
        itm.SetColumn(ccnStatusColumn);
        itm.SetMask(wxLIST_MASK_TEXT);
        if (list->GetItem(itm))
            txt = itm.GetText();
    }

    if (txt == _("Detected") || txt == _("User-defined"))
    {
        wxListItem itm;
        itm.SetId(idx);
        itm.SetColumn(ccnNameColumn);
        itm.SetMask(wxLIST_MASK_TEXT);
        if (list->GetItem(itm))
        {
            Compiler* compiler = CompilerFactory::GetCompilerByName(itm.GetText());
            idx = CompilerFactory::GetCompilerIndex(compiler);
        }
        txt = CompilerFactory::GetCompiler(idx)->GetMasterPath();
    }
    else
        txt = wxEmptyString;

    if (list->GetToolTip())
    {
        if (txt.empty())
            list->UnsetToolTip();
        else if (txt != list->GetToolTip()->GetTip())
            list->SetToolTip(txt);
    }
    else if (!txt.empty())
        list->SetToolTip(txt);
}

void AutoDetectCompilers::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    const bool en = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED) != -1;
    XRCCTRL(*this, "btnSetDefaultCompiler", wxButton)->Enable(en);

    event.Skip();
}
