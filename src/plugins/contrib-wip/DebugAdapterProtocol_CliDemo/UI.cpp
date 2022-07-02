//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: UI.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "UI.hpp"

// Declare the bitmap loading function
extern void wxC10A1InitBitmapResources();

static bool bBitmapLoaded = false;

MainFrameBase::MainFrameBase(wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos,
                             const wxSize & size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if (!bBitmapLoaded)
    {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC10A1InitBitmapResources();
        bBitmapLoaded = true;
    }

    m_toolbar12 = this->CreateToolBar(wxTB_HORZ_TEXT | wxTB_NOICONS | wxTB_FLAT, wxID_ANY);
    m_toolbar12->SetToolBitmapSize(wxSize(16, 16));
    m_toolbar12->AddTool(wxID_NETWORK, _("Connect..."), wxXmlResource::Get()->LoadBitmap(wxT("placeholder16")),
                         wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    m_toolbar12->AddSeparator();
    m_toolbar12->AddTool(wxID_FORWARD, _("Next"),
                         wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR, wxSize(24, 24)), wxNullBitmap,
                         wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    m_toolbar12->AddTool(wxID_DOWN, _("Step In"),
                         wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR, wxSize(24, 24)), wxNullBitmap,
                         wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    m_toolbar12->AddTool(wxID_UP, _("Step Out"), wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_TOOLBAR, wxSize(24, 24)),
                         wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    m_toolbar12->AddTool(wxID_EXECUTE, _("Continue"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""),
                         NULL);
    m_toolbar12->AddSeparator();
    m_toolbar12->AddTool(wxID_ABORT, _("Pause"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    m_toolbar12->AddSeparator();
    m_toolbar12->AddTool(wxID_STOP, _("Set Breakpoint..."), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""),
                         NULL);
    m_toolbar12->Realize();
    wxBoxSizer * boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);
    m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    boxSizer1->Add(m_panel2, 1, wxEXPAND, WXC_FROM_DIP(5));
    wxBoxSizer * boxSizer48 = new wxBoxSizer(wxVERTICAL);
    m_panel2->SetSizer(boxSizer48);
    wxFlexGridSizer * flexGridSizer51 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer51->SetFlexibleDirection(wxBOTH);
    flexGridSizer51->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer51->AddGrowableCol(1);
    boxSizer48->Add(flexGridSizer51, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_staticTextSelectDebugFileName = new wxStaticText(m_panel2, wxID_ANY, _("Debug File Name"), wxDefaultPosition,
                                                       wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);
    flexGridSizer51->Add(m_staticTextSelectDebugFileName, 0, wxLEFT | wxTOP | wxBOTTOM | wxALIGN_CENTER_VERTICAL,
                         WXC_FROM_DIP(5));
    m_filePickerSelectDebugFileName =
        new wxFilePickerCtrl(m_panel2, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition,
                             wxDLG_UNIT(m_panel2, wxSize(600, -1)), wxFLP_DEFAULT_STYLE | wxFLP_SMALL);
    flexGridSizer51->Add(m_filePickerSelectDebugFileName, 1, wxEXPAND, WXC_FROM_DIP(5));
    m_splitter4 = new wxSplitterWindow(m_panel2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)),
                                       wxSP_LIVE_UPDATE | wxSP_NO_XP_THEME | wxSP_3DSASH);
    m_splitter4->SetSashGravity(0.5);
    m_splitter4->SetMinimumPaneSize(10);
    boxSizer48->Add(m_splitter4, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_splitterPageSourceFile =
        new wxPanel(m_splitter4, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter4, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    wxBoxSizer * boxSizerSourceFile = new wxBoxSizer(wxVERTICAL);
    m_splitterPageSourceFile->SetSizer(boxSizerSourceFile);
    m_stcTextSourceFile = new wxStyledTextCtrl(m_splitterPageSourceFile, wxID_ANY, wxDefaultPosition,
                                               wxDLG_UNIT(m_splitterPageSourceFile, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcTextSourceFile->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcTextSourceFile->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcTextSourceFile->SetMarginSensitive(4, true);
    m_stcTextSourceFile->SetMarginWidth(4, 0);
    // Configure the tracker margin
    m_stcTextSourceFile->SetMarginWidth(1, 0);
    // Configure the symbol margin
    m_stcTextSourceFile->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcTextSourceFile->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcTextSourceFile->SetMarginWidth(2, 0);
    m_stcTextSourceFile->SetMarginSensitive(2, true);
    // Configure the line numbers margin
    int m_stcTextSourceFile_PixelWidth = 4 + 5 * m_stcTextSourceFile->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_stcTextSourceFile->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcTextSourceFile->SetMarginWidth(0, m_stcTextSourceFile_PixelWidth);
    // Configure the line symbol margin
    m_stcTextSourceFile->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcTextSourceFile->SetMarginMask(3, 0);
    m_stcTextSourceFile->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcTextSourceFile->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcTextSourceFile->StyleClearAll();
    m_stcTextSourceFile->SetWrapMode(0);
    m_stcTextSourceFile->SetIndentationGuides(0);
    m_stcTextSourceFile->SetKeyWords(0, wxT(""));
    m_stcTextSourceFile->SetKeyWords(1, wxT(""));
    m_stcTextSourceFile->SetKeyWords(2, wxT(""));
    m_stcTextSourceFile->SetKeyWords(3, wxT(""));
    m_stcTextSourceFile->SetKeyWords(4, wxT(""));
    boxSizerSourceFile->Add(m_stcTextSourceFile, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_splitterPageDAPDebugInfo =
        new wxPanel(m_splitter4, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter4, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_splitter4->SplitHorizontally(m_splitterPageSourceFile, m_splitterPageDAPDebugInfo, 0);
    wxBoxSizer * boxSizerDAPDebugInfo = new wxBoxSizer(wxVERTICAL);
    m_splitterPageDAPDebugInfo->SetSizer(boxSizerDAPDebugInfo);
    m_notebookDAPDebugInfo = new wxNotebook(m_splitterPageDAPDebugInfo, wxID_ANY, wxDefaultPosition,
                                            wxDLG_UNIT(m_splitterPageDAPDebugInfo, wxSize(-1, -1)), wxBK_DEFAULT);
    m_notebookDAPDebugInfo->SetName(wxT("m_notebookDAPDebugInfo"));
    boxSizerDAPDebugInfo->Add(m_notebookDAPDebugInfo, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_panel18 = new wxPanel(m_notebookDAPDebugInfo, wxID_ANY, wxDefaultPosition,
                            wxDLG_UNIT(m_notebookDAPDebugInfo, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebookDAPDebugInfo->AddPage(m_panel18, _("Stack"), false);
    wxBoxSizer * boxSizer20 = new wxBoxSizer(wxVERTICAL);
    m_panel18->SetSizer(boxSizer20);
    m_stcStack = new wxStyledTextCtrl(m_panel18, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel18, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcStack->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcStack->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcStack->SetMarginSensitive(4, true);
    m_stcStack->SetMarginWidth(4, 0);
    // Configure the tracker margin
    m_stcStack->SetMarginWidth(1, 0);
    // Configure the symbol margin
    m_stcStack->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcStack->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcStack->SetMarginWidth(2, 0);
    m_stcStack->SetMarginSensitive(2, true);
    // Configure the line numbers margin
    int m_stcStack_PixelWidth = 4 + 5 * m_stcStack->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_stcStack->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcStack->SetMarginWidth(0, m_stcStack_PixelWidth);
    // Configure the line symbol margin
    m_stcStack->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcStack->SetMarginMask(3, 0);
    m_stcStack->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcStack->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcStack->StyleClearAll();
    m_stcStack->SetWrapMode(0);
    m_stcStack->SetIndentationGuides(0);
    m_stcStack->SetKeyWords(0, wxT(""));
    m_stcStack->SetKeyWords(1, wxT(""));
    m_stcStack->SetKeyWords(2, wxT(""));
    m_stcStack->SetKeyWords(3, wxT(""));
    m_stcStack->SetKeyWords(4, wxT(""));
    boxSizer20->Add(m_stcStack, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_panel19 = new wxPanel(m_notebookDAPDebugInfo, wxID_ANY, wxDefaultPosition,
                            wxDLG_UNIT(m_notebookDAPDebugInfo, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebookDAPDebugInfo->AddPage(m_panel19, _("Threads"), false);
    wxBoxSizer * boxSizer21 = new wxBoxSizer(wxVERTICAL);
    m_panel19->SetSizer(boxSizer21);
    m_stcThreads =
        new wxStyledTextCtrl(m_panel19, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel19, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcThreads->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcThreads->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcThreads->SetMarginSensitive(4, true);
    m_stcThreads->SetMarginWidth(4, 0);
    // Configure the tracker margin
    m_stcThreads->SetMarginWidth(1, 0);
    // Configure the symbol margin
    m_stcThreads->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcThreads->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcThreads->SetMarginWidth(2, 0);
    m_stcThreads->SetMarginSensitive(2, true);
    // Configure the line numbers margin
    int m_stcThreads_PixelWidth = 4 + 5 * m_stcThreads->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_stcThreads->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcThreads->SetMarginWidth(0, m_stcThreads_PixelWidth);
    // Configure the line symbol margin
    m_stcThreads->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcThreads->SetMarginMask(3, 0);
    m_stcThreads->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcThreads->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcThreads->StyleClearAll();
    m_stcThreads->SetWrapMode(0);
    m_stcThreads->SetIndentationGuides(0);
    m_stcThreads->SetKeyWords(0, wxT(""));
    m_stcThreads->SetKeyWords(1, wxT(""));
    m_stcThreads->SetKeyWords(2, wxT(""));
    m_stcThreads->SetKeyWords(3, wxT(""));
    m_stcThreads->SetKeyWords(4, wxT(""));
    boxSizer21->Add(m_stcThreads, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_panel27 = new wxPanel(m_notebookDAPDebugInfo, wxID_ANY, wxDefaultPosition,
                            wxDLG_UNIT(m_notebookDAPDebugInfo, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebookDAPDebugInfo->AddPage(m_panel27, _("Log"), false);
    wxBoxSizer * boxSizer28 = new wxBoxSizer(wxVERTICAL);
    m_panel27->SetSizer(boxSizer28);
    m_stcLog = new wxStyledTextCtrl(m_panel27, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel27, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcLog->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcLog->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcLog->SetMarginSensitive(4, true);
    m_stcLog->SetMarginWidth(4, 0);
    // Configure the tracker margin
    m_stcLog->SetMarginWidth(1, 0);
    // Configure the symbol margin
    m_stcLog->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcLog->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcLog->SetMarginWidth(2, 0);
    m_stcLog->SetMarginSensitive(2, true);
    // Configure the line numbers margin
    int m_stcLog_PixelWidth = 4 + 5 * m_stcLog->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_stcLog->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcLog->SetMarginWidth(0, m_stcLog_PixelWidth);
    // Configure the line symbol margin
    m_stcLog->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcLog->SetMarginMask(3, 0);
    m_stcLog->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcLog->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcLog->StyleClearAll();
    m_stcLog->SetWrapMode(0);
    m_stcLog->SetIndentationGuides(0);
    m_stcLog->SetKeyWords(0, wxT(""));
    m_stcLog->SetKeyWords(1, wxT(""));
    m_stcLog->SetKeyWords(2, wxT(""));
    m_stcLog->SetKeyWords(3, wxT(""));
    m_stcLog->SetKeyWords(4, wxT(""));
    boxSizer28->Add(m_stcLog, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_panel30 = new wxPanel(m_notebookDAPDebugInfo, wxID_ANY, wxDefaultPosition,
                            wxDLG_UNIT(m_notebookDAPDebugInfo, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebookDAPDebugInfo->AddPage(m_panel30, _("Scope Variables"), false);
    wxBoxSizer * boxSizer31 = new wxBoxSizer(wxVERTICAL);
    m_panel30->SetSizer(boxSizer31);
    m_stcScopes =
        new wxStyledTextCtrl(m_panel30, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel30, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcScopes->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcScopes->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcScopes->SetMarginSensitive(4, true);
    m_stcScopes->SetMarginWidth(4, 0);
    // Configure the tracker margin
    m_stcScopes->SetMarginWidth(1, 0);
    // Configure the symbol margin
    m_stcScopes->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcScopes->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcScopes->SetMarginWidth(2, 0);
    m_stcScopes->SetMarginSensitive(2, true);
    // Configure the line numbers margin
    int m_stcScopes_PixelWidth = 4 + 5 * m_stcScopes->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_stcScopes->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcScopes->SetMarginWidth(0, m_stcScopes_PixelWidth);
    // Configure the line symbol margin
    m_stcScopes->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcScopes->SetMarginMask(3, 0);
    m_stcScopes->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcScopes->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcScopes->StyleClearAll();
    m_stcScopes->SetWrapMode(0);
    m_stcScopes->SetIndentationGuides(0);
    m_stcScopes->SetKeyWords(0, wxT(""));
    m_stcScopes->SetKeyWords(1, wxT(""));
    m_stcScopes->SetKeyWords(2, wxT(""));
    m_stcScopes->SetKeyWords(3, wxT(""));
    m_stcScopes->SetKeyWords(4, wxT(""));
    boxSizer31->Add(m_stcScopes, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
#if wxVERSION_NUMBER >= 2900

    if (!wxPersistenceManager::Get().Find(m_notebookDAPDebugInfo))
    {
        wxPersistenceManager::Get().RegisterAndRestore(m_notebookDAPDebugInfo);
    }
    else
    {
        wxPersistenceManager::Get().Restore(m_notebookDAPDebugInfo);
    }

#endif
    SetName(wxT("MainFrameBase"));
    SetSize(wxDLG_UNIT(this, wxSize(800, 600)));

    if (GetSizer())
    {
        GetSizer()->Fit(this);
    }

    if (GetParent())
    {
        CentreOnParent(wxBOTH);
    }
    else
    {
        CentreOnScreen(wxBOTH);
    }

    if (!wxPersistenceManager::Get().Find(this))
    {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    }
    else
    {
        wxPersistenceManager::Get().Restore(this);
    }

    // Connect events
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnConnect, this, wxID_NETWORK);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnConnectUI, this, wxID_NETWORK);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnNext, this, wxID_FORWARD);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnNextUI, this, wxID_FORWARD);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnStepIn, this, wxID_DOWN);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnStepInUI, this, wxID_DOWN);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnStepOut, this, wxID_UP);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnStepOutUI, this, wxID_UP);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnContinue, this, wxID_EXECUTE);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnContinueUI, this, wxID_EXECUTE);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnPause, this, wxID_ABORT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnPauseUI, this, wxID_ABORT);
    this->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnSetBreakpoint, this, wxID_STOP);
    this->Bind(wxEVT_UPDATE_UI, &MainFrameBase::OnSetBreakpointUI, this, wxID_STOP);
}

MainFrameBase::~MainFrameBase()
{
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnConnect, this, wxID_NETWORK);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnConnectUI, this, wxID_NETWORK);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnNext, this, wxID_FORWARD);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnNextUI, this, wxID_FORWARD);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnStepIn, this, wxID_DOWN);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnStepInUI, this, wxID_DOWN);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnStepOut, this, wxID_UP);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnStepOutUI, this, wxID_UP);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnContinue, this, wxID_EXECUTE);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnContinueUI, this, wxID_EXECUTE);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnPause, this, wxID_ABORT);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnPauseUI, this, wxID_ABORT);
    this->Unbind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrameBase::OnSetBreakpoint, this, wxID_STOP);
    this->Unbind(wxEVT_UPDATE_UI, &MainFrameBase::OnSetBreakpointUI, this, wxID_STOP);
}
