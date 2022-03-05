/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 12287 $
 * $Id: ccoptionsdlg.cpp 12287 2021-01-23 05:46:23Z mortenmacfly $
 * $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/codecompletion/ccoptionsdlg.cpp $
 */

#include <sdk.h>

#ifndef CB_PRECOMP
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>
#include <wx/regex.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/xrc/xmlres.h>

#include <cbstyledtextctrl.h>
#include <configmanager.h>
#include <globals.h>
#include <logmanager.h>
#include <manager.h>
#endif

#include <wx/filedlg.h>
#include <wx/listbook.h>

#include <editpairdlg.h>

#include "cbcolourmanager.h"
#include "ccoptionsdlg.h"
#include "codecompletion.h"
#include "doxygen_parser.h" // For DocumentationHelper
#include "../ClangLocator.h"

BEGIN_EVENT_TABLE(CCOptionsDlg, wxPanel)
    EVT_UPDATE_UI(-1,                       CCOptionsDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnColour"),          CCOptionsDlg::OnChooseColour)
    EVT_COMMAND_SCROLL(XRCID("sldCCDelay"), CCOptionsDlg::OnCCDelayScroll)
    EVT_BUTTON(XRCID("btnDocBgColor"),      CCOptionsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnDocTextColor"),    CCOptionsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnDocLinkColor"),    CCOptionsDlg::OnChooseColour)

    EVT_BUTTON(XRCID("btnClangSelectMasterPath"),       CCOptionsDlg::OnLLVM_Clang_SelectMasterPath_Dlg)
    EVT_BUTTON(XRCID("btnClangAutoDetectMasterPath"),   CCOptionsDlg::OnLLVM_Clang_AutoDetectMasterPath)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CCOptionsDlg::CCOptionsDlg(wxWindow* parent, ParseManager* np, CodeCompletion* cc, DocumentationHelper* dh)
// ----------------------------------------------------------------------------
    : m_ParseManager(np),
      m_CodeCompletion(cc),
      m_Parser(np->GetParser()),
      m_Documentation(dh)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager("clangd_client");

    wxXmlResource::Get()->LoadPanel(this, parent, "dlgCCSettings");

    // -----------------------------------------------------------------------
    // Handle all options that are being directly applied from config
    // -----------------------------------------------------------------------

    // Page "clangd_client"
    XRCCTRL(*this, "chkNoSemantic",         wxCheckBox)->SetValue(!cfg->ReadBool("/semantic_keywords",   false));
    XRCCTRL(*this, "chkAutoAddParentheses", wxCheckBox)->SetValue(cfg->ReadBool("/auto_add_parentheses", true));
    XRCCTRL(*this, "chkDetectImpl",         wxCheckBox)->SetValue(cfg->ReadBool("/detect_implementation",false));
    XRCCTRL(*this, "chkAddDoxgenComment",   wxCheckBox)->SetValue(cfg->ReadBool("/add_doxgen_comment",   false));
    XRCCTRL(*this, "chkEnableHeaders",      wxCheckBox)->SetValue(cfg->ReadBool("/enable_headers",       true));
    XRCCTRL(*this, "spnMaxMatches",         wxSpinCtrl)->SetValue(cfg->ReadInt("/max_matches",           16384));
    XRCCTRL(*this, "txtFillupChars",        wxTextCtrl)->SetValue(cfg->Read("/fillup_chars",             wxEmptyString));
    XRCCTRL(*this, "sldCCDelay",            wxSlider)->SetValue(cfg->ReadInt("/cc_delay",                300) / 100);
    UpdateCCDelayLabel();
    XRCCTRL(*this, "chkKL_1",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set1",  true));
    XRCCTRL(*this, "chkKL_2",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set2",  true));
    XRCCTRL(*this, "chkKL_3",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set3",  false));
    XRCCTRL(*this, "chkKL_4",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set4",  false));
    XRCCTRL(*this, "chkKL_5",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set5",  false));
    XRCCTRL(*this, "chkKL_6",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set6",  false));
    XRCCTRL(*this, "chkKL_7",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set7",  false));
    XRCCTRL(*this, "chkKL_8",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set8",  false));
    XRCCTRL(*this, "chkKL_9",               wxCheckBox)->SetValue(cfg->ReadBool("/lexer_keywords_set9",  false));

    // Page "C / C++ parser"
    // NOTE (Morten#1#): Keep this in sync with files in the XRC file (settings.xrc) and ParseManager.cpp
    XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->SetValue(cfg->ReadInt("/max_threads", 1));
    XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->Enable(true);   //(ph 2021/07/17)

    // Page "C / C++ parser (adv.)"
    // NOTE (Morten#1#): Keep this in sync with files in the XRC file (settings.xrc) and parser.cpp
    XRCCTRL(*this, "txtCCFileExtHeader",       wxTextCtrl)->SetValue(cfg->Read("/header_ext",    "h,hpp,hxx,hh,h++,tcc,xpm"));
    XRCCTRL(*this, "chkCCFileExtEmpty",        wxCheckBox)->SetValue(cfg->ReadBool("/empty_ext", true));
    XRCCTRL(*this, "txtCCFileExtSource",       wxTextCtrl)->SetValue(cfg->Read("/source_ext",    "c,cpp,cxx,cc,c++"));

    // Page "Symbol browser"
    XRCCTRL(*this, "chkNoSB",        wxCheckBox)->SetValue(!cfg->ReadBool("/use_symbols_browser", true));
    XRCCTRL(*this, "chkFloatCB",     wxCheckBox)->SetValue(cfg->ReadBool("/as_floating_window", false));

    // The toolbar section
    wxCheckBox *scopeFilter = XRCCTRL(*this, "chkScopeFilter", wxCheckBox);
    scopeFilter->SetValue(cfg->ReadBool("/scope_filter", true));
    wxSpinCtrl *spinScopeLength = XRCCTRL(*this, "spnChoiceScopeLength", wxSpinCtrl);
    spinScopeLength->Enable(scopeFilter->GetValue());
    spinScopeLength->SetValue(cfg->ReadInt("/toolbar_scope_length", 280));
    XRCCTRL(*this, "spnChoiceFunctionLength", wxSpinCtrl)->SetValue(cfg->ReadInt("/toolbar_function_length", 660));

    // -----------------------------------------------------------------------
    // Handle all options that are being handled by m_Parser
    // -----------------------------------------------------------------------

    // Page "clangd_client"
    XRCCTRL(*this, "chkUseSmartSense",      wxCheckBox)->SetValue(!m_Parser.Options().useSmartSense);
    XRCCTRL(*this, "chkWhileTyping",        wxCheckBox)->SetValue(m_Parser.Options().whileTyping);

    // Page "C / C++ parser"
    XRCCTRL(*this, "chkLocals",             wxCheckBox)->SetValue(m_Parser.Options().followLocalIncludes);
    XRCCTRL(*this, "chkGlobals",            wxCheckBox)->SetValue(m_Parser.Options().followGlobalIncludes);
    XRCCTRL(*this, "chkPreprocessor",       wxCheckBox)->SetValue(m_Parser.Options().wantPreprocessor);
    XRCCTRL(*this, "chkComplexMacros",      wxCheckBox)->SetValue(m_Parser.Options().parseComplexMacros);
    XRCCTRL(*this, "chkPlatformCheck",      wxCheckBox)->SetValue(m_Parser.Options().platformCheck);
    XRCCTRL(*this, "chkLogClangdClient",    wxCheckBox)->SetValue(m_Parser.Options().logClangdClientCheck);
    XRCCTRL(*this, "chkLogClangdServer",    wxCheckBox)->SetValue(m_Parser.Options().logClangdServerCheck);
    XRCCTRL(*this, "chkLSPMsgsFocusOnSave", wxCheckBox)->SetValue(m_Parser.Options().lspMsgsFocusOnSaveCheck);
    XRCCTRL(*this, "chkLSPMsgsClearOnSave", wxCheckBox)->SetValue(m_Parser.Options().lspMsgsClearOnSaveCheck);

    wxString LLVM_DetectedClangExeFileName = m_Parser.Options().LLVM_DetectedClangExeFileName;
    wxString LLVM_DetectedClangDaemonExeFileName = m_Parser.Options().LLVM_DetectedClangDaemonExeFileName;

    XRCCTRL(*this, "txtLLVM_MasterPath",                wxTextCtrl)->SetValue(m_Parser.Options().LLVM_MasterPath);
    XRCCTRL(*this, "txtDetectedClangExeFileName",       wxTextCtrl)->SetValue(LLVM_DetectedClangExeFileName);
    XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->SetValue(LLVM_DetectedClangDaemonExeFileName);
    XRCCTRL(*this, "txtDetectedIncludeClangDirectory",  wxTextCtrl)->SetValue(m_Parser.Options().LLVM_DetectedIncludeClangDirectory);

    if (!LLVM_DetectedClangDaemonExeFileName.empty())
    {
        // Verify clangd version is at least 13
        wxString ClangDaemonVersion =  ClangLocator::GetExeFileVersion(LLVM_DetectedClangDaemonExeFileName);
        wxTextCtrl* controlClangDaemonVersionNumber = XRCCTRL(*this, "txtDetectedClangDaemonExeFileVersion", wxTextCtrl);
        controlClangDaemonVersionNumber->SetValue(ClangDaemonVersion);

        if (ClangLocator::IsClangFileVersionValid(LLVM_DetectedClangDaemonExeFileName))
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->GetBackgroundColour());
        }
        else
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(wxColour(*wxRED));
        }
    }


    if (!LLVM_DetectedClangExeFileName.empty())
    {
        // Verify clangd version is at least 13
        wxString ClangDaemonVersion =  ClangLocator::GetExeFileVersion(LLVM_DetectedClangExeFileName);
        wxTextCtrl* controlClangDaemonVersionNumber = XRCCTRL(*this, "txtDetectedClangExeFileVersion", wxTextCtrl);
        controlClangDaemonVersionNumber->SetValue(ClangDaemonVersion);

        if (ClangLocator::IsClangFileVersionValid(LLVM_DetectedClangExeFileName))
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(XRCCTRL(*this, "txtDetectedClangExeFileName", wxTextCtrl)->GetBackgroundColour());
        }
        else
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(wxColour(*wxRED));
        }
    }

    m_Old_LLVM_MasterPath = m_Parser.Options().LLVM_MasterPath; //save for onApply() check

    // FIXME (ph#): implement these unused hidden check boxes ?
    XRCCTRL(*this, "chkLocals",        wxCheckBox)->Hide(); //(ph 2021/11/9)
    XRCCTRL(*this, "chkGlobals",       wxCheckBox)->Hide();
    XRCCTRL(*this, "chkPreprocessor",  wxCheckBox)->Hide();
    XRCCTRL(*this, "chkComplexMacros", wxCheckBox)->Hide();

    // Page "Symbol browser"
    XRCCTRL(*this, "chkInheritance",        wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().showInheritance);
    XRCCTRL(*this, "chkExpandNS",           wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().expandNS);
    XRCCTRL(*this, "chkTreeMembers",        wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().treeMembers);

    // Page Documentation
    XRCCTRL(*this, "chkDocumentation",      wxCheckBox)->SetValue(m_Documentation->IsEnabled());

    ColourManager *colours = Manager::Get()->GetColourManager();
    XRCCTRL(*this, "btnDocBgColor",         wxButton)->SetBackgroundColour(colours->GetColour(wxT("cc_docs_back")));
    XRCCTRL(*this, "btnDocTextColor",       wxButton)->SetBackgroundColour(colours->GetColour(wxT("cc_docs_fore")));
    XRCCTRL(*this, "btnDocLinkColor",       wxButton)->SetBackgroundColour(colours->GetColour(wxT("cc_docs_link")));
}

// ----------------------------------------------------------------------------
CCOptionsDlg::~CCOptionsDlg()
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::OnPageChanging()
// ----------------------------------------------------------------------------
{
    // plugin about to be shown
    // This event does NOT occur when others get the focus
    // so we never know if we've lost focused or not.
}
// ----------------------------------------------------------------------------
void CCOptionsDlg::OnApply()
// ----------------------------------------------------------------------------
{
    wxString activePageTitle;
    // Get the title of the currently active/focused configuration page
    wxWindow* pTopWindow = wxFindWindowByName(_("Configure editor"));
    if (not pTopWindow)
        pTopWindow = m_CodeCompletion->GetTopWxWindow();
    if (pTopWindow)
    {
        wxListbook* lb = XRCCTRL(*pTopWindow, "nbMain", wxListbook);
        wxWindow* page = lb ? lb->GetCurrentPage() : nullptr;
        int pageID = page ? lb->FindPage(page) : 0;
        activePageTitle = lb ? lb->GetPageText(pageID) : wxString();
    }

    ConfigManager* cfg = Manager::Get()->GetConfigManager("clangd_client");

    // -----------------------------------------------------------------------
    // Handle all options that are being directly applied / written from UI:
    // -----------------------------------------------------------------------

    // Page "clangd_client"
    cfg->Write("/semantic_keywords",    (bool)!XRCCTRL(*this, "chkNoSemantic",         wxCheckBox)->GetValue());
    cfg->Write("/use_SmartSense",       (bool) XRCCTRL(*this, "chkUseSmartSense",      wxCheckBox)->GetValue());
    cfg->Write("/while_typing",         (bool) XRCCTRL(*this, "chkWhileTyping",        wxCheckBox)->GetValue());
    cfg->Write("/auto_add_parentheses", (bool) XRCCTRL(*this, "chkAutoAddParentheses", wxCheckBox)->GetValue());
    cfg->Write("/detect_implementation",(bool) XRCCTRL(*this, "chkDetectImpl",         wxCheckBox)->GetValue());
    cfg->Write("/add_doxgen_comment",   (bool) XRCCTRL(*this, "chkAddDoxgenComment",   wxCheckBox)->GetValue());
    cfg->Write("/enable_headers",       (bool) XRCCTRL(*this, "chkEnableHeaders",      wxCheckBox)->GetValue());
    cfg->Write("/max_matches",          (int)  XRCCTRL(*this, "spnMaxMatches",         wxSpinCtrl)->GetValue());
    cfg->Write("/fillup_chars",                XRCCTRL(*this, "txtFillupChars",        wxTextCtrl)->GetValue());
    cfg->Write("/cc_delay",             (int)  XRCCTRL(*this, "sldCCDelay",            wxSlider)->GetValue() * 100);
    cfg->Write("/lexer_keywords_set1",  (bool) XRCCTRL(*this, "chkKL_1",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set2",  (bool) XRCCTRL(*this, "chkKL_2",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set3",  (bool) XRCCTRL(*this, "chkKL_3",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set4",  (bool) XRCCTRL(*this, "chkKL_4",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set5",  (bool) XRCCTRL(*this, "chkKL_5",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set6",  (bool) XRCCTRL(*this, "chkKL_6",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set7",  (bool) XRCCTRL(*this, "chkKL_7",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set8",  (bool) XRCCTRL(*this, "chkKL_8",               wxCheckBox)->GetValue());
    cfg->Write("/lexer_keywords_set9",  (bool) XRCCTRL(*this, "chkKL_9",               wxCheckBox)->GetValue());

    // Page "C / C++ parser"
    cfg->Write("/parser_follow_local_includes",  (bool) XRCCTRL(*this, "chkLocals",                wxCheckBox)->GetValue());
    cfg->Write("/parser_follow_global_includes", (bool) XRCCTRL(*this, "chkGlobals",               wxCheckBox)->GetValue());
    cfg->Write("/want_preprocessor",             (bool) XRCCTRL(*this, "chkPreprocessor",          wxCheckBox)->GetValue());
    cfg->Write("/parse_complex_macros",          (bool) XRCCTRL(*this, "chkComplexMacros",         wxCheckBox)->GetValue());
    cfg->Write("/platform_check",                (bool) XRCCTRL(*this, "chkPlatformCheck",         wxCheckBox)->GetValue());
    cfg->Write("/logClangdClient_check",         (bool) XRCCTRL(*this, "chkLogClangdClient",       wxCheckBox)->GetValue());
    cfg->Write("/logClangdServer_check",         (bool) XRCCTRL(*this, "chkLogClangdServer",       wxCheckBox)->GetValue());
    cfg->Write("/lspMsgsFocusOnSave_check",      (bool) XRCCTRL(*this, "chkLSPMsgsFocusOnSave",    wxCheckBox)->GetValue());
    cfg->Write("/lspMsgsClearOnSave_check",      (bool) XRCCTRL(*this, "chkLSPMsgsClearOnSave",    wxCheckBox)->GetValue());
    cfg->Write("/LLVM_MasterPath",                      XRCCTRL(*this, "txtLLVM_MasterPath",                wxTextCtrl)->GetValue());
    cfg->Write("/LLVM_DetectedClangExeFileName",        XRCCTRL(*this, "txtDetectedClangExeFileName",       wxTextCtrl)->GetValue());
    cfg->Write("/LLVM_DetectedClangDaemonExeFileName",  XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->GetValue());
    cfg->Write("/LLVM_DetectedIncludeClangDirectory",   XRCCTRL(*this, "txtDetectedIncludeClangDirectory",  wxTextCtrl)->GetValue());

    cfg->Write("/max_threads",                   (int)  XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->GetValue());

    // Page "C / C++ parser (adv.)"
    cfg->Write("/header_ext",        XRCCTRL(*this, "txtCCFileExtHeader", wxTextCtrl)->GetValue());
    cfg->Write("/empty_ext",  (bool) XRCCTRL(*this, "chkCCFileExtEmpty",  wxCheckBox)->GetValue());
    cfg->Write("/source_ext",        XRCCTRL(*this, "txtCCFileExtSource", wxTextCtrl)->GetValue());

    // Page "Symbol browser"
    cfg->Write("/use_symbols_browser",      (bool)!XRCCTRL(*this, "chkNoSB",        wxCheckBox)->GetValue());
    cfg->Write("/browser_show_inheritance", (bool) XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue());
    cfg->Write("/browser_expand_ns",        (bool) XRCCTRL(*this, "chkExpandNS",    wxCheckBox)->GetValue());
    cfg->Write("/as_floating_window",       (bool) XRCCTRL(*this, "chkFloatCB",     wxCheckBox)->GetValue());
    cfg->Write("/browser_tree_members",     (bool) XRCCTRL(*this, "chkTreeMembers", wxCheckBox)->GetValue());

    // The toolbar section
    cfg->Write("/scope_filter", (bool) XRCCTRL(*this, "chkScopeFilter", wxCheckBox)->GetValue());
    cfg->Write("/toolbar_scope_length", (int)XRCCTRL(*this, "spnChoiceScopeLength", wxSpinCtrl)->GetValue());
    cfg->Write("/toolbar_function_length", (int)XRCCTRL(*this, "spnChoiceFunctionLength", wxSpinCtrl)->GetValue());

    // Page "Documentation"
    cfg->Write("/use_documentation_helper", (bool) XRCCTRL(*this, "chkDocumentation", wxCheckBox)->GetValue());
    cfg->Write("/documentation_helper_background_color", (wxColour) XRCCTRL(*this, "btnDocBgColor",   wxButton)->GetBackgroundColour());
    cfg->Write("/documentation_helper_text_color",       (wxColour) XRCCTRL(*this, "btnDocTextColor", wxButton)->GetBackgroundColour());
    cfg->Write("/documentation_helper_link_color",       (wxColour) XRCCTRL(*this, "btnDocLinkColor", wxButton)->GetBackgroundColour());
    // -----------------------------------------------------------------------
    // Handle all options that are being read by m_Parser.ReadOptions():
    // -----------------------------------------------------------------------

    // Force parser to read its options that we write in the config
    // Also don't forget to update the Parser option according UI!
    m_Parser.ReadOptions();

    // Page "clangd_client"
    m_Parser.Options().useSmartSense = !XRCCTRL(*this, "chkUseSmartSense",    wxCheckBox)->GetValue();
    m_Parser.Options().whileTyping   =  XRCCTRL(*this, "chkWhileTyping",      wxCheckBox)->GetValue();

    // Page "C / C++ parser"
    m_Parser.Options().followLocalIncludes  = XRCCTRL(*this, "chkLocals",             wxCheckBox)->GetValue();
    m_Parser.Options().followGlobalIncludes = XRCCTRL(*this, "chkGlobals",            wxCheckBox)->GetValue();
    m_Parser.Options().wantPreprocessor     = XRCCTRL(*this, "chkPreprocessor",       wxCheckBox)->GetValue();
    m_Parser.Options().parseComplexMacros   = XRCCTRL(*this, "chkComplexMacros",      wxCheckBox)->GetValue();
    m_Parser.Options().platformCheck        = XRCCTRL(*this, "chkPlatformCheck",      wxCheckBox)->GetValue();
    m_Parser.Options().logClangdClientCheck = XRCCTRL(*this, "chkLogClangdClient",    wxCheckBox)->GetValue();
    m_Parser.Options().logClangdServerCheck = XRCCTRL(*this, "chkLogClangdServer",    wxCheckBox)->GetValue();
    m_Parser.Options().lspMsgsFocusOnSaveCheck = XRCCTRL(*this, "chkLSPMsgsFocusOnSave",  wxCheckBox)->GetValue();
    m_Parser.Options().lspMsgsClearOnSaveCheck = XRCCTRL(*this, "chkLSPMsgsClearOnSave",  wxCheckBox)->GetValue();

    if (wxDirExists(m_Parser.Options().LLVM_MasterPath))
    {
        m_Parser.Options().LLVM_MasterPath                      = XRCCTRL(*this, "txtLLVM_MasterPath",                 wxTextCtrl)->GetValue();
        m_Parser.Options().LLVM_DetectedClangExeFileName        = XRCCTRL(*this, "txtDetectedClangExeFileName",        wxTextCtrl)->GetValue();
        m_Parser.Options().LLVM_DetectedClangDaemonExeFileName  = XRCCTRL(*this, "txtDetectedClangDaemonExeFileName",  wxTextCtrl)->GetValue();
        m_Parser.Options().LLVM_DetectedIncludeClangDirectory   = XRCCTRL(*this, "txtDetectedIncludeClangDirectory",   wxTextCtrl)->GetValue();
    }
    else
    {
        wxString msg;
        msg << _("The clangd path:\n") << m_Parser.Options().LLVM_MasterPath << _(" does not exist.");
        msg << _("\nCode completion will be inoperable.");
        if ((activePageTitle == "clangd_client") or (activePageTitle == _("clangd_client")) )
            cbMessageBox(msg, _("ERROR: Clangd client") );
        m_Parser.Options().LLVM_MasterPath = m_Old_LLVM_MasterPath;
    }

    // Page "Symbol browser"
    m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();
    m_Parser.ClassBrowserOptions().expandNS        = XRCCTRL(*this, "chkExpandNS",    wxCheckBox)->GetValue();
    m_Parser.ClassBrowserOptions().treeMembers     = XRCCTRL(*this, "chkTreeMembers", wxCheckBox)->GetValue();

    // Page "Documentation"
    m_Documentation->RereadOptions(cfg);

    m_Parser.Options().storeDocumentation    = XRCCTRL(*this, "chkDocumentation",  wxCheckBox)->GetValue();
    m_Documentation->SetEnabled(               XRCCTRL(*this, "chkDocumentation",  wxCheckBox)->GetValue() );

    ColourManager *colours = Manager::Get()->GetColourManager();
    wxColor colour = XRCCTRL(*this, "btnDocBgColor",   wxButton)->GetBackgroundColour();
    colours->SetColour("cc_docs_back", colour);
    colour = XRCCTRL(*this, "btnDocTextColor",   wxButton)->GetBackgroundColour();
    colours->SetColour("cc_docs_text", colour);
    colour = XRCCTRL(*this, "btnDocLinkColor",   wxButton)->GetBackgroundColour();
    colours->SetColour("cc_docs_link", colour);

    // Now write the parser options and re-read them again to make sure they are up-to-date
    m_Parser.WriteOptions();
    m_ParseManager->RereadParserOptions();
    m_Documentation->WriteOptions(cfg);
    m_CodeCompletion->RereadOptions();

    // If a project is loaded and the clangd location changed, say something.
    if (
        Manager::Get()->GetProjectManager()->GetActiveProject() &&
        (m_Old_LLVM_MasterPath != m_Parser.Options().LLVM_MasterPath)
    )
    {
        wxString msg = _("Currently loaded projects need reloading or reparsing to accomodate the clangd change.");
        if ((activePageTitle == "clangd_client") or (activePageTitle == _("clangd_client")) )
            cbMessageBox(msg, _("Settings changed"));
    }

}
// ----------------------------------------------------------------------------
void CCOptionsDlg::OnChooseColour(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxColourData data;
    wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

    wxColourDialog dlg(this, &data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour colour = dlg.GetColourData().GetColour();
        sender->SetBackgroundColour(colour);
    }
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::OnCCDelayScroll(cb_unused wxScrollEvent& event)
// ----------------------------------------------------------------------------
{
    UpdateCCDelayLabel();
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    // ccmanager's config Settings/Editor/Code completion
    ConfigManager* ccmcfg = Manager::Get()->GetConfigManager("ccmanager");
    bool en = ccmcfg->ReadBool("/code_completion", false); //<==  CCManagers  main setting, NOT clangd_client's
    bool aap = XRCCTRL(*this, "chkAutoAddParentheses", wxCheckBox)->GetValue();

    // Page "clangd_client"
    XRCCTRL(*this, "chkUseSmartSense",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkWhileTyping",                wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoAddParentheses",         wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkDetectImpl",                 wxCheckBox)->Enable(en && aap);
    XRCCTRL(*this, "chkAddDoxgenComment",           wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkEnableHeaders",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkNoSemantic",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "lblMaxMatches",                 wxStaticText)->Enable(en);
    XRCCTRL(*this, "spnMaxMatches",                 wxSpinCtrl)->Enable(en);
    XRCCTRL(*this, "lblFillupChars",                wxStaticText)->Enable(en);
    XRCCTRL(*this, "txtFillupChars",                wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "sldCCDelay",                    wxSlider)->Enable(en);

    // keyword sets
    XRCCTRL(*this, "chkKL_1",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_2",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_3",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_4",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_5",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_6",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_7",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_8",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_9",                 wxCheckBox)->Enable(en);

    // Page "C / C++ parser"
    XRCCTRL(*this, "chkLocals",                     wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkGlobals",                    wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkPreprocessor",               wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkComplexMacros",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkPlatformCheck",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkLogClangdClient",            wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkLogClangdServer",            wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkLSPMsgsFocusOnSave",         wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkLSPMsgsClearOnSave",         wxCheckBox)->Enable(en);
    XRCCTRL(*this, "txtLLVM_MasterPath",                wxTextCtrl)->Enable(en);    //(ph 2021/11/7)
    XRCCTRL(*this, "txtDetectedClangExeFileName",       wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->Enable(en);

    // Page "C / C++ parser (adv.)"
    // FIXME (ollydbg#1#01/07/15): should code_completion option affect our parser's behaviour?
    en = ccmcfg->ReadBool("/clangd_client", true);
    XRCCTRL(*this, "txtCCFileExtHeader",      wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "chkCCFileExtEmpty",       wxCheckBox)->Enable(en);
    XRCCTRL(*this, "txtCCFileExtSource",      wxTextCtrl)->Enable(en);

    // Page "Symbol browser"
    en = !XRCCTRL(*this, "chkNoSB",           wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkInheritance",          wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkExpandNS",             wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkFloatCB",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkTreeMembers",          wxCheckBox)->Enable(en);

    // Toolbar section
    wxCheckBox *scopeFilter = XRCCTRL(*this, "chkScopeFilter", wxCheckBox);
    XRCCTRL(*this, "spnChoiceScopeLength", wxSpinCtrl)->Enable(scopeFilter->GetValue());

    // Page "Documentation"
    en = XRCCTRL(*this, "chkDocumentation",   wxCheckBox)->GetValue();
    XRCCTRL(*this, "btnDocBgColor",           wxButton)->Enable(en);
    XRCCTRL(*this, "btnDocTextColor",         wxButton)->Enable(en);
    XRCCTRL(*this, "btnDocLinkColor",         wxButton)->Enable(en);
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::UpdateCCDelayLabel()
// ----------------------------------------------------------------------------
{
    int position = XRCCTRL(*this, "sldCCDelay", wxSlider)->GetValue();
    wxString lbl;
    if (position >= 10)
        lbl.Printf(_("%d.%d sec"), position / 10, position % 10);
    else
        lbl.Printf(_("%d ms"), position * 100);
    XRCCTRL(*this, "lblDelay", wxStaticText)->SetLabel(lbl);
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::UpdateClangDetectedDetails(const wxString& LLVMMasterPath, const wxString& clangDaemonFilename, const wxString& clangExeFilename, const wxString& clangIncDir)
{
    if (clangDaemonFilename.empty())
    {
        XRCCTRL(*this, "txtLLVM_MasterPath", wxTextCtrl)->SetValue("");
        wxString msg;
        msg << __PRETTY_FUNCTION__ << "() Could not find clangd installation.";
        cbMessageBox( msg, "Error");
        return;
    }

    if (clangDaemonFilename.empty())
    {
        XRCCTRL(*this, "txtDetectedClangDaemonExeFileVersion", wxTextCtrl)->SetValue(wxEmptyString);
    }
    else
    {
        // Verify clangd version is at least 13
        wxString ClangDaemonVersion =  ClangLocator::GetExeFileVersion(clangDaemonFilename);
        wxTextCtrl* controlClangDaemonVersionNumber = XRCCTRL(*this, "txtDetectedClangDaemonExeFileVersion", wxTextCtrl);
        controlClangDaemonVersionNumber->SetValue(ClangDaemonVersion);

        if (ClangLocator::IsClangFileVersionValid(clangDaemonFilename))
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->GetBackgroundColour());
        }
        else
        {
            controlClangDaemonVersionNumber->SetBackgroundColour(wxColour(*wxRED));
        }
    }

    if (clangExeFilename.empty())
    {
        XRCCTRL(*this, "txtDetectedClangExeFileVersion", wxTextCtrl)->SetValue(wxEmptyString);
    }
    else
    {
        // Verify clang version is at least 13
        wxString ClangVersion =  ClangLocator::GetExeFileVersion(clangExeFilename);
        wxTextCtrl* controlClangVersionNumber = XRCCTRL(*this, "txtDetectedClangExeFileVersion", wxTextCtrl);
        controlClangVersionNumber->SetValue(ClangVersion);

        if (ClangLocator::IsClangFileVersionValid(clangExeFilename))
        {
            controlClangVersionNumber->SetBackgroundColour(XRCCTRL(*this, "txtDetectedClangExeFileVersion", wxTextCtrl)->GetBackgroundColour());
        }
        else
        {
            controlClangVersionNumber->SetBackgroundColour(wxColour(*wxRED));
        }
    }

    XRCCTRL(*this, "txtLLVM_MasterPath", wxTextCtrl)->SetValue(LLVMMasterPath);
    XRCCTRL(*this, "txtDetectedClangExeFileName", wxTextCtrl)->SetValue(clangExeFilename);
    XRCCTRL(*this, "txtDetectedClangDaemonExeFileName", wxTextCtrl)->SetValue(clangDaemonFilename);
    XRCCTRL(*this, "txtDetectedIncludeClangDirectory", wxTextCtrl)->SetValue(clangIncDir);

    m_Parser.Options().LLVM_MasterPath = LLVMMasterPath;
    m_Parser.Options().LLVM_DetectedClangExeFileName = clangExeFilename;
    m_Parser.Options().LLVM_DetectedClangDaemonExeFileName = clangDaemonFilename;
    m_Parser.Options().LLVM_DetectedIncludeClangDirectory = clangIncDir;
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::OnLLVM_Clang_SelectMasterPath_Dlg(wxCommandEvent& event)
{
    // common part follows
    wxString file_selection = _("All files (*)|*");
    if (platform::windows)
    {
        file_selection = _("Executable files (*.exe)|*.exe");
    }

    wxFileDialog dlg(this,                                  // wxWindow *  	parent,
                     _("Select clangd executable file"),    // const wxString &  	message = wxFileSelectorPromptStr,
#if defined(__WXGTK__)
                     "/",                                // const wxString &  	defaultDir = wxEmptyString,
                     CLANG_DAEMON_FILENAME,              // const wxString &  	defaultFile = wxEmptyString,
                     "*",                                // const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
#else
                     "",                                 // const wxString &  	defaultDir = wxEmptyString,
                     CLANG_DAEMON_FILENAME,              // const wxString &  	defaultFile = wxEmptyString,
                     "*.*",                              // const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
#endif
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | compatibility::wxHideReadonly ); // long  	style = wxFD_DEFAULT_STYLE,
    dlg.SetFilterIndex(0);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    wxString clangDaemonFilename = dlg.GetPath();  // get path and filename
    if (clangDaemonFilename.Contains(CLANG_DAEMON_FILENAME))
    {
        wxString clangExeFilename = wxEmptyString;
        wxString detectedClangVersion = wxEmptyString;
        wxString clangIncDir = wxEmptyString;

        // Get master path
        wxFileName fnLLVMMasterPath(clangDaemonFilename);
        wxString LLVMMasterPath = fnLLVMMasterPath.GetPath();

        // Try to find clang exe
        const wxString clangFilenameConst(CLANG_FILENAME);
        if (wxFileExists(fnLLVMMasterPath.GetPath() + wxFILE_SEP_PATH + clangFilenameConst))
        {
            clangExeFilename = fnLLVMMasterPath.GetPath() + wxFILE_SEP_PATH + clangFilenameConst;
        }

        // Try to find include clang directory
        ClangLocator::LocateIncludeClangDir(LLVMMasterPath, detectedClangVersion, clangIncDir);

        UpdateClangDetectedDetails(LLVMMasterPath, clangDaemonFilename, clangExeFilename, clangIncDir);
    }
    else
    {
        wxString msg = "Failed to select the clangd executable.";
        cbMessageBox(msg,"ERROR");
    }
}

// ----------------------------------------------------------------------------
void CCOptionsDlg::OnLLVM_Clang_AutoDetectMasterPath(cb_unused wxCommandEvent& event)
{
    wxString LLVMMasterPath, clangDaemonFilename, clangExeFilename, clangIncDir;

    // Locate folders for LLVM resources
    ClangLocator clangLocator;
    clangLocator.LocateLLVMResources(LLVMMasterPath, clangDaemonFilename, clangExeFilename, clangIncDir);

    UpdateClangDetectedDetails(LLVMMasterPath, clangDaemonFilename, clangExeFilename, clangIncDir);
}
