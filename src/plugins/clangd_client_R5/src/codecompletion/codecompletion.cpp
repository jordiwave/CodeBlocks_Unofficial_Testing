/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
+ * $Revision: 12304 $
+ * $Id: codecompletion.cpp 12304 2021-03-16 23:28:31Z fuscated $
+ * $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/codecompletion/codecompletion.cpp $
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <algorithm>
    #include <iterator>
    #include <set> // for handling unique items in some places
    #include "assert.h"

    #include <wx/choicdlg.h>
    #include <wx/choice.h>
    #include <wx/dir.h>
    #include <wx/filename.h>
    #include <wx/fs_zip.h>
    #include <wx/menu.h>
    #include <wx/mimetype.h>
    #include <wx/msgdlg.h>
    #include <wx/regex.h>
    #include <wx/tipwin.h>
    #include <wx/toolbar.h>
    #include <wx/utils.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/wxscintilla.h>
    #include <wx/uri.h>         //(ph 2020/12/4)
    #include <wx/textfile.h>    //(ph 2021/02/3)

    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editorcolourset.h>
    #include <editormanager.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <macrosmanager.h>
    #include <manager.h>
    #include <projectmanager.h>
    #include <sdk_events.h>
#endif

#include <wx/tokenzr.h>
#include <wx/html/htmlwin.h>
#include <wx/app.h>             //(ph 2021/02/1) wxWakeUpIdle()
#include <globals.h>

#include <cbstyledtextctrl.h>
#include <editor_hooks.h>
#include <filegroupsandmasks.h>
#include <multiselectdlg.h>

#include "codecompletion.h"
#include <annoyingdialog.h>

#include "Version.h" //(ph 2021/01/5)

#include "cbexception.h"
#include "ccoptionsdlg.h"
#include "ccoptionsprjdlg.h"
#include "insertclassmethoddlg.h"
#include "selectincludefile.h"
#include "parser/ccdebuginfo.h"
#include "parser/cclogger.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"
#include "doxygen_parser.h" // for DocumentationPopup and DoxygenParser
#include "gotofunctiondlg.h"
#include <searchresultslog.h>       //(ph 2020/10/25) LSP references event
#include <encodingdetector.h>       //(ph 2020/10/26)
#include "infowindow.h"             //(ph 2020/11/22)
#include "lspdiagresultslog.h"      //(ph 2020/10/25) LSP
#include "asyncprocess\procutils.h" //(ph 2020/10/25) LSP
#include "LSPEventCallbackHandler.h" //(ph 2021/10/22)

#define CC_CODECOMPLETION_DEBUG_OUTPUT 0
//#define CC_CODECOMPLETION_DEBUG_OUTPUT 1        //(ph 2021/05/1)

// let the global debug macro overwrite the local debug macro value
#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #undef CC_CODECOMPLETION_DEBUG_OUTPUT
    #define CC_CODECOMPLETION_DEBUG_OUTPUT CC_GLOBAL_DEBUG_OUTPUT
#endif

#if CC_CODECOMPLETION_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(wxString::Format(format, ##args))
    #define TRACE2(format, args...)
#elif CC_CODECOMPLETION_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(wxString::Format(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(wxString::Format(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

/// Scopes choice name for global functions in CC's toolbar.
static wxString g_GlobalScope(_T("<global>"));

// this auto-registers the plugin
// ----------------------------------------------------------------------------
namespace
// ----------------------------------------------------------------------------
{
    // this auto-registers the plugin
    PluginRegistrant<CodeCompletion> reg(_T("Clangd_Client"));

    const char STX = '\u0002';

    // LSP_Symbol identifiers
    #include "..\LSP_SymbolKind.h"

    bool wxFound(int result){return result != wxNOT_FOUND;}
    bool shutTFU = wxFound(0); //shutup 'not used' compiler err msg when wxFound not used
}
// ----------------------------------------------------------------------------
namespace CodeCompletionHelper
// ----------------------------------------------------------------------------
{
    // compare method for the sort algorithm for our FunctionScope struct
    inline bool LessFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
    {
        int result = wxStricmp(fs1.Scope, fs2.Scope);
        if (result == 0)
        {
            result = wxStricmp(fs1.Name, fs2.Name);
            if (result == 0)
                result = fs1.StartLine - fs2.StartLine;
        }

        return result < 0;
    }

    inline bool EqualFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
    {
        int result = wxStricmp(fs1.Scope, fs2.Scope);
        if (result == 0)
            result = wxStricmp(fs1.Name, fs2.Name);

        return result == 0;
    }

    inline bool LessNameSpace(const NameSpace& ns1, const NameSpace& ns2)
    {
        return ns1.Name < ns2.Name;
    }

    inline bool EqualNameSpace(const NameSpace& ns1, const NameSpace& ns2)
    {
        return ns1.Name == ns2.Name;
    }

    /// for OnGotoFunction(), search backward
    /// @code
    /// xxxxx  /* yyy */
    ///     ^             ^
    ///     result        begin
    /// @endcode
    inline wxChar GetLastNonWhitespaceChar(cbStyledTextCtrl* control, int position)
    {
        if (!control)
            return 0;

        while (--position > 0)
        {
            const int style = control->GetStyleAt(position);
            if (control->IsComment(style))
                continue;

            const wxChar ch = control->GetCharAt(position);
            if (ch <= _T(' '))
                continue;

            return ch;
        }

        return 0;
    }

    /// for OnGotoFunction(), search forward
    ///        /* yyy */  xxxxx
    ///     ^             ^
    ///     begin         result
    inline wxChar GetNextNonWhitespaceChar(cbStyledTextCtrl* control, int position)
    {
        if (!control)
            return 0;

        const int totalLength = control->GetLength();
        --position;
        while (++position < totalLength)
        {
            const int style = control->GetStyleAt(position);
            if (control->IsComment(style))
                continue;

            const wxChar ch = control->GetCharAt(position);
            if (ch <= _T(' '))
                continue;

            return ch;
        }

        return 0;
    }

    /**  Sorting in GetLocalIncludeDirs() */
    inline int CompareStringLen(const wxString& first, const wxString& second)
    {
        return second.Len() - first.Len();
    }

    /**  for CodeCompleteIncludes()
     * a line has some pattern like below
     @code
        # [space or tab] include
     @endcode
     */
    inline bool TestIncludeLine(wxString const &line)
    {
        size_t index = line.find(_T('#'));
        if (index == wxString::npos)
            return false;
        ++index;

        for (; index < line.length(); ++index)
        {
            if (line[index] != _T(' ') && line[index] != _T('\t'))
            {
                if (line.Mid(index, 7) == _T("include"))
                    return true;
                break;
            }
        }
        return false;
    }

    /** return identifier like token string under the current cursor pointer
     * @param[out] NameUnderCursor the identifier like token string
     * @param[out] IsInclude true if it is a #include command
     * @return true if the underlining text is a #include command, or a normal identifier
     */
    inline bool EditorHasNameUnderCursor(wxString& NameUnderCursor, bool& IsInclude)
    {
        bool ReturnValue = false;
        if (cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
        {
            cbStyledTextCtrl* control = ed->GetControl();
            const int pos = control->GetCurrentPos();
            const wxString line = control->GetLine(control->LineFromPosition(pos));
            const wxRegEx reg(_T("^[ \t]*#[ \t]*include[ \t]+[\"<]([^\">]+)[\">]"));
            wxString inc;
            if (reg.Matches(line))
                inc = reg.GetMatch(line, 1);

            if (!inc.IsEmpty())
            {
                NameUnderCursor = inc;
                ReturnValue = true;
                IsInclude = true;
            }
            else
            {
                const int start = control->WordStartPosition(pos, true);
                const int end = control->WordEndPosition(pos, true);
                const wxString word = control->GetTextRange(start, end);
                if (!word.IsEmpty())
                {
                    NameUnderCursor.Clear();
                    NameUnderCursor << word;
                    ReturnValue = true;
                    IsInclude = false;
                }
            }
        }
        return ReturnValue;
    }
    /** used to record the position of a token when user click find declaration or implementation */
    struct GotoDeclarationItem
    {
        wxString filename;
        unsigned line;
    };

    /** when user select one item in the suggestion list, the selected contains the full display
     * name, for example, "function_name():function_return_type", and we only need to insert the
     * "function_name" to the editor, so this function just get the actual inserted text.
     * @param selected a full display name of the selected token in the suggestion list
     * @return the stripped text which are used to insert to the editor
     */
    static wxString AutocompGetName(const wxString& selected)
    {
        size_t nameEnd = selected.find_first_of(_T("(: "));
        return selected.substr(0,nameEnd);
    }

}//end namespace CodeCompletionHelper

// ----------------------------------------------------------------------------
// menu IDs
// ----------------------------------------------------------------------------
// just because we don't know other plugins' used identifiers,
// we use wxNewId() to generate a guaranteed unique ID ;), instead of enum
// (don't forget that, especially in a plugin)
// used in the wxFrame's main menu
int idMenuGotoFunction          = wxNewId();
int idMenuGotoPrevFunction      = wxNewId();
int idMenuGotoNextFunction      = wxNewId();
int idMenuGotoDeclaration       = wxNewId();
int idMenuGotoImplementation    = wxNewId();
int idMenuOpenIncludeFile       = wxNewId();
int idMenuFindReferences        = wxNewId();
int idMenuRenameSymbols         = wxNewId();
int idViewClassBrowser          = wxNewId();
// used in context menu
int idCurrentProjectReparse     = wxNewId();
int idSelectedProjectReparse    = wxNewId();
int idSelectedFileReparse       = wxNewId();
int idEditorFileReparse         = wxNewId();    //(ph 2021/11/16)
int idEditorSubMenu             = wxNewId();
int idClassMethod               = wxNewId();
int idUnimplementedClassMethods = wxNewId();
int idGotoDeclaration           = wxNewId();
int idGotoImplementation        = wxNewId();
int idOpenIncludeFile           = wxNewId();

int idRealtimeParsingTimer      = wxNewId();
int idToolbarTimer              = XRCID("idToolbarTimer");
int idProjectSavedTimer         = wxNewId();
int idReparsingTimer            = wxNewId();
int idEditorActivatedTimer      = wxNewId();
int LSPeventID                  = wxNewId(); //(ph 2020/11/4)
int idPauseParsing              = wxNewId(); //(ph 2021/07/28)

// all the below delay time is in milliseconds units
// when the user enables the parsing while typing option, this is the time delay when parsing
// would happen after the editor has changed.
#define REALTIME_PARSING_DELAY    500

// there are many reasons to trigger the refreshing of CC toolbar. But to avoid refreshing
// the toolbar too often, we add a timer to delay the refresh, this is just like a mouse dwell
// event, which means we do the real job when the editor is stable for a while (no event
// happens in the delay time period).
#define TOOLBAR_REFRESH_DELAY     150

// the time delay between an editor activated event and the updating of the CC toolbar.
// Note that we are only interest in a stable activated editor, so if another editor is activated
// during the time delay, the timer will be restarted.
#define EDITOR_ACTIVATED_DELAY    300

// ----------------------------------------------------------------------------
// Event table
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CodeCompletion, cbCodeCompletionPlugin)
    EVT_UPDATE_UI_RANGE(idMenuGotoFunction, idCurrentProjectReparse, CodeCompletion::OnUpdateUI)

    EVT_MENU(idMenuGotoFunction,                   CodeCompletion::OnGotoFunction             )
    EVT_MENU(idMenuGotoPrevFunction,               CodeCompletion::OnGotoPrevFunction         )
    EVT_MENU(idMenuGotoNextFunction,               CodeCompletion::OnGotoNextFunction         )
    EVT_MENU(idMenuGotoDeclaration,                CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuGotoImplementation,             CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuFindReferences,                 CodeCompletion::OnFindReferences           )
    EVT_MENU(idMenuRenameSymbols,                  CodeCompletion::OnRenameSymbols            )
    EVT_MENU(idClassMethod,                        CodeCompletion::OnClassMethod              )
    EVT_MENU(idUnimplementedClassMethods,          CodeCompletion::OnUnimplementedClassMethods)
    EVT_MENU(idGotoDeclaration,                    CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idGotoImplementation,                 CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idOpenIncludeFile,                    CodeCompletion::OnOpenIncludeFile          )
    EVT_MENU(idMenuOpenIncludeFile,                CodeCompletion::OnOpenIncludeFile          )

    EVT_MENU(idViewClassBrowser,                   CodeCompletion::OnViewClassBrowser      )
    EVT_MENU(idCurrentProjectReparse,              CodeCompletion::OnCurrentProjectReparse )
    EVT_MENU(idSelectedProjectReparse,             CodeCompletion::OnReparseSelectedProject)
    EVT_MENU(idSelectedFileReparse,                CodeCompletion::OnSelectedFileReparse   )
    EVT_MENU(idEditorFileReparse,                  CodeCompletion::OnEditorFileReparse   )
    EVT_MENU(idPauseParsing,                       CodeCompletion::OnSelectedPauseParsing ) //(ph 2021/07/28)

    // CC's toolbar
    EVT_CHOICE(XRCID("chcCodeCompletionScope"),    CodeCompletion::OnScope   )
    EVT_CHOICE(XRCID("chcCodeCompletionFunction"), CodeCompletion::OnFunction)

    EVT_IDLE(                                      CodeCompletion::OnIdle)                   //(ph 2021/03/8)
    //-EVT_MENU(XRCID("idLSP_Process_Terminated"),    CodeCompletion::OnLSP_ProcessTerminated )     //(ph 2021/06/28)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeCompletion::CodeCompletion() :
// ----------------------------------------------------------------------------
    m_InitDone(false),
    m_pCodeRefactoring(nullptr), //re-initialized in dtor
    m_EditorHookId(0),
    m_TimerRealtimeParsing(this, idRealtimeParsingTimer),
    m_TimerToolbar(this, idToolbarTimer),
    m_TimerEditorActivated(this, idEditorActivatedTimer),
    m_LastEditor(0),
    m_ToolBar(0),
    m_Function(0),
    m_Scope(0),
    m_ToolbarNeedRefresh(true),
    m_ToolbarNeedReparse(false),
    m_CurrentLine(0),
    m_NeedReparse(false),
    m_CurrentLength(-1),
    m_NeedsBatchColour(true),
    m_CCMaxMatches(16384),
    m_CCAutoAddParentheses(true),
    m_CCDetectImplementation(false),
    m_CCDelay(300),
    m_CCEnableHeaders(false),
    m_CCEnablePlatformCheck(true),
    m_DocHelper(this)
{
    // ccmanager's config
    ConfigManager* ccmcfg = Manager::Get()->GetConfigManager(_T("ccmanager"));
    m_CodeCompletionEnabled = ccmcfg->ReadBool(_T("/code_completion"), false);

    if (not m_CodeCompletionEnabled) return;

    // create Idle time CallbackHandler     //(ph 2021/09/27)
    LSPEventCallbackHandler* pNewLSPEventSinkHandler = new LSPEventCallbackHandler();
    pLSPEventSinkHandler.reset( pNewLSPEventSinkHandler);

    // ParseManager / CodeRefactoring creation
    m_pParseManager.reset( new ParseManager(pNewLSPEventSinkHandler) );
    m_pCodeRefactoring = new CodeRefactoring(m_pParseManager.get());

    // CCLogger are the log event bridges, those events were finally handled by its parent, here
    // it is the CodeCompletion plugin ifself.
    CCLogger::Get()->Init(this, g_idCCLogger, g_idCCDebugLogger, g_idCCDebugErrorLogger);

    if (!Manager::LoadResource(_T("Clangd_Client.zip")))
        NotifyMissingFile(_T("Clangd_Client.zip"));

    // handling events send from CCLogger
    Connect(g_idCCLogger,                wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCLogger)     );
    Connect(g_idCCDebugLogger,           wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCDebugLogger));
    Connect(g_idCCDebugErrorLogger,      wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCDebugLogger));

    Connect(idToolbarTimer,         wxEVT_TIMER, wxTimerEventHandler(CodeCompletion::OnToolbarTimer)        );          //(ph 2021/07/27)
    Connect(idToolbarTimer,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnToolbarTimer));//(ph 2021/09/11)
    Connect(idEditorActivatedTimer, wxEVT_TIMER, wxTimerEventHandler(CodeCompletion::OnEditorActivatedTimer));

    Connect(XRCID("idLSP_Process_Terminated"), wxEVT_COMMAND_MENU_SELECTED, //(ph 2021/06/28)
            wxCommandEventHandler(CodeCompletion::OnLSP_ProcessTerminated));
}
// ----------------------------------------------------------------------------
CodeCompletion::~CodeCompletion()
// ----------------------------------------------------------------------------
{
    Disconnect(g_idCCLogger,                wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCLogger));
    Disconnect(g_idCCDebugLogger,           wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCDebugLogger));
    Disconnect(g_idCCDebugErrorLogger,      wxEVT_COMMAND_MENU_SELECTED, CodeBlocksThreadEventHandler(CodeCompletion::OnCCDebugLogger));

    Disconnect(idToolbarTimer,         wxEVT_TIMER, wxTimerEventHandler(CodeCompletion::OnToolbarTimer)        );
    Disconnect(idToolbarTimer,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnToolbarTimer)); //(ph 2021/09/11)
    Disconnect(idEditorActivatedTimer, wxEVT_TIMER, wxTimerEventHandler(CodeCompletion::OnEditorActivatedTimer));

    Disconnect(XRCID("idLSP_Process_Terminated"), wxEVT_COMMAND_MENU_SELECTED, //(ph 2021/06/28)
            wxCommandEventHandler(CodeCompletion::OnLSP_ProcessTerminated));
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnAttach()
// ----------------------------------------------------------------------------
{
    AppVersion appVersion;
    appVersion.m_AppName = "Clangd_Client";
    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = appVersion.GetVersion();

    // ccmanager's config obtained from Menu=>Settings=>Editor=>Code Completion (sub panel)
    // Get the CB config item that enables CodeCompletion
    ConfigManager* ccmcfg = Manager::Get()->GetConfigManager(_T("ccmanager"));
    m_CodeCompletionEnabled = ccmcfg->ReadBool(_T("/code_completion"), false);
    if (not m_CodeCompletionEnabled)
    {
        pInfo->version = pInfo->version.BeforeFirst(' ') + " Inactive";
        return;
    }

    // get top window to use as cbMessage parent, else they hide behind dialog
    wxWindow* topWindow = wxFindWindowByName("Manage plugins");
    if (not topWindow) topWindow = Manager::Get()->GetAppWindow();

    // For LSP client
    // If old "CodeCompletion" plugin is attached, exit with message
    PluginManager* pPlgnMgr = Manager::Get()->GetPluginManager();
    const PluginInfo* pCCinfo = pPlgnMgr->GetPluginInfo("CodeCompletion");
    if (pCCinfo)
    {
        // Old CodeCompletion is loaded?
        // Is it disabled...
        wxString baseKey;
        baseKey << _T("/") << "CodeCompletion";
        bool loadIt = Manager::Get()->GetConfigManager(_T("plugins"))->ReadBool(baseKey,true);
        if (loadIt)
        {
            wxString msg = "The Clangd client plugin cannot run while the \"Code completion\" plugin is enabled.";
            msg += "\nClangd client plugin will now disable itself. :-(";
            cbMessageBox(msg, "Clangd client", wxOK, topWindow);
            m_IsAttached = false;
            wxWindow* window = Manager::Get()->GetAppWindow();
            if (window)
            {
                // remove ourself from the application's event handling chain...
                // which cbPlugin.cpp just placed before calling OnAttach()
                // Else an attempt to re-enable this plugin will cause a hang.
                if (GetParseManager()->FindEventHandler(this))
                    window->RemoveEventHandler(this);
            }
            m_PluginNeedsAppRestart = true;
            return;
        }
    }
    // For LSP client
    if (m_PluginNeedsAppRestart)
    {
        cbMessageBox("Clang_Client plugin needs CodeBlocks to be restarted before it can function properly.",
                        "CB restart needed", wxOK, topWindow);
        wxWindow* window = Manager::Get()->GetAppWindow();
        if (window)
        {
            // remove ourself from the application's event handling chain...
            // which cbPlugin.cpp just placed before this call
            // Else an attempt to re-enable this plugin will cause a hang.
            if (GetParseManager()->FindEventHandler(this))
                window->RemoveEventHandler(this);
        }
        return;
    }

    m_EditMenu    = 0;
    m_SearchMenu  = 0;
    m_ViewMenu    = 0;
    m_ProjectMenu = 0;
    // toolbar related variables
    m_ToolBar     = 0;
    m_Function    = 0;
    m_Scope       = 0;
    m_FunctionsScope.clear();
    m_NameSpaces.clear();
    m_AllFunctionsScopes.clear();
    m_ToolbarNeedRefresh = true; // by default

    m_LastFile.clear();

    // read options from configure file
    RereadOptions();

    // Events which m_ParseManager does not handle will go to the the next event
    // handler which is the instance of a CodeCompletion.
    GetParseManager()->SetNextHandler(this);

    GetParseManager()->CreateClassBrowser();

    // hook to editors
    // both ccmanager and cc have hooks, but they don't conflict. ccmanager are mainly
    // hooking to the event such as key stroke or mouse dwell events, so the code completion, call tip
    // and tool tip will be handled in ccmanager. The other cases such as caret movement triggers
    // updating the CC's toolbar, modifying the editor causing the real time content reparse will be
    // handled inside cc's own editor hook.
    EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<CodeCompletion>(this, &CodeCompletion::EditorEventHook);
    m_EditorHookId = EditorHooks::RegisterHook(myhook);

    // register event sinks
    Manager* pm = Manager::Get();

    pm->RegisterEventSink(cbEVT_APP_STARTUP_DONE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnAppDoneStartup));

    pm->RegisterEventSink(cbEVT_WORKSPACE_CHANGED,    new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnWorkspaceChanged));

    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectActivated));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE,        new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectClosed));
    pm->RegisterEventSink(cbEVT_PROJECT_OPEN,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectOpened)); //(ph 2021/03/8)
    pm->RegisterEventSink(cbEVT_PROJECT_SAVE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectSaved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_ADDED,   new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileAdded));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileRemoved));

    pm->RegisterEventSink(cbEVT_EDITOR_SAVE,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorSave));
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorOpen));
    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorActivated));
    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorClosed));

    pm->RegisterEventSink(cbEVT_DEBUGGER_STARTED,      new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnDebuggerStarting));
    pm->RegisterEventSink(cbEVT_DEBUGGER_FINISHED,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnDebuggerFinished));

    m_DocHelper.OnAttach();
}
// ----------------------------------------------------------------------------
bool CodeCompletion::CanDetach() const
// ----------------------------------------------------------------------------
{
    if (not m_CodeCompletionEnabled) return true;

    int prjCount = Manager::Get()->GetProjectManager()->GetProjects()->GetCount();
    if (prjCount)
    {
        wxWindow* pDlg = wxWindow::FindWindowByLabel("Manage plugins");
        wxString msg = "Please close the workspace before disabling or uninstalling clangd_client plugin.";
        cbMessageBox(msg, "Uninstall" , wxOK, pDlg ? pDlg : wxTheApp->GetTopWindow());
        return false;
    }
    return true;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // FYI:
    // A crash occurs when user uninstalls this plugin while a project is loaded.
    // It happends in pluginmanager.cpp at "PluginManager::Configure()" exit.
    // If a project is NOT loaded, the crash does not occur.
    // Closing the workspace in this function does not help the issue.
    // Reinstalling the missing support code for "virtual bool CanDetach() const { return true; }"
    // into the sdk allows us to ask the user to close the project before uninstalling
    // this plugin.

    if (not m_CodeCompletionEnabled) return;

    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();

    // Check for disable or uninstall plugin
    if (pProject and (not appShutDown))
    {
        bool attachedState = m_IsAttached;
        m_IsAttached =  true;
        CodeBlocksEvent cbEvt(cbEVT_PROJECT_CLOSE);
        cbEvt.SetProject(pProject);
        OnProjectClosed(cbEvt);
        m_IsAttached = attachedState;
    }

    GetParseManager()->RemoveClassBrowser(appShutDown);
    GetParseManager()->ClearParsers();

    // remove chained handler
    GetParseManager()->SetNextHandler(nullptr);

    // unregister hook
    // 'true' will delete the functor too
    EditorHooks::UnregisterHook(m_EditorHookId, true);

    // remove registered event sinks
    Manager::Get()->RemoveAllEventSinksFor(this);

    m_FunctionsScope.clear();
    m_NameSpaces.clear();
    m_AllFunctionsScopes.clear();
    m_ToolbarNeedRefresh = false;

/* TODO (mandrav#1#): Delete separator line too... */
    if (m_EditMenu)
        m_EditMenu->Delete(idMenuRenameSymbols);

    if (m_SearchMenu)
    {
        m_SearchMenu->Delete(idMenuGotoFunction);
        m_SearchMenu->Delete(idMenuGotoPrevFunction);
        m_SearchMenu->Delete(idMenuGotoNextFunction);
        m_SearchMenu->Delete(idMenuGotoDeclaration);
        m_SearchMenu->Delete(idMenuGotoImplementation);
        m_SearchMenu->Delete(idMenuFindReferences);
        m_SearchMenu->Delete(idMenuOpenIncludeFile);
    }

    m_DocHelper.OnRelease();

    // ----------------------------------------------------------------
    // LSP OnRelease() processing
    // ----------------------------------------------------------------
    if (m_LSP_Clients.size())    // shutdown LSP servers
    {
        for (auto const& client : m_LSP_Clients)
        {
            //key: pProject; value: pProcessLanguageClient;
            if (client.second)
            {
                client.second->LSP_Shutdown();
                if (client.second)
                    delete client.second;
                m_LSP_Clients[client.first] = nullptr;
            }
        }
    }
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* CodeCompletion::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    if (not m_CodeCompletionEnabled) return nullptr;

    return new CCOptionsDlg(parent, GetParseManager(), this, &m_DocHelper);
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* CodeCompletion::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
// ----------------------------------------------------------------------------
{
    if (not m_CodeCompletionEnabled) return nullptr;

    return new CCOptionsProjectDlg(parent, project, GetParseManager());
}
// ----------------------------------------------------------------------------
void CodeCompletion::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
    // if not attached, exit
    if (!IsAttached())
    {
        // After OnAttach() the event handler was enabled by main().
        // Disable hard coded events when not attached.
        SetEvtHandlerEnabled(false);
        return;
    }

    if (not m_CodeCompletionEnabled) return;

    int pos = menuBar->FindMenu(_("&Edit"));
    if (pos != wxNOT_FOUND)
    {
        m_EditMenu = menuBar->GetMenu(pos);
        m_EditMenu->AppendSeparator();
        m_EditMenu->Append(idMenuRenameSymbols, _("Rename symbols\tAlt-N"));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Edit menu!"));

    pos = menuBar->FindMenu(_("Sea&rch"));
    if (pos != wxNOT_FOUND)
    {
        m_SearchMenu = menuBar->GetMenu(pos);
        m_SearchMenu->Append(idMenuGotoFunction,       _("Goto function...\tCtrl-Shift-G"));
        m_SearchMenu->Append(idMenuGotoPrevFunction,   _("Goto previous function\tCtrl-PgUp"));
        m_SearchMenu->Append(idMenuGotoNextFunction,   _("Goto next function\tCtrl-PgDn"));
        m_SearchMenu->Append(idMenuGotoDeclaration,    _("Goto declaration\tCtrl-Shift-."));
        m_SearchMenu->Append(idMenuGotoImplementation, _("Goto implementation\tCtrl-."));
        m_SearchMenu->Append(idMenuFindReferences,     _("Find references\tAlt-."));
        m_SearchMenu->Append(idMenuOpenIncludeFile,    _("Open include file"));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Search menu!"));

    // add the classbrowser window in the "View" menu
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        m_ViewMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ViewMenu->GetMenuItems();
        bool inserted = false;

        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                m_ViewMenu->InsertCheckItem(i, idViewClassBrowser, _("Symbols browser"), _("Toggle displaying the symbols browser"));
                inserted = true;
                break;
            }
        }

        // not found, just append
        if (!inserted)
            m_ViewMenu->AppendCheckItem(idViewClassBrowser, _("Symbols browser"), _("Toggle displaying the symbols browser"));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find View menu!"));

    // add Reparse item in the "Project" menu
    idx = menuBar->FindMenu(_("&Project"));
    if (idx != wxNOT_FOUND)
    {
        m_ProjectMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ProjectMenu->GetMenuItems();
        bool inserted = false;

        // find the first separator and insert before it
        for (size_t i = items.GetCount() - 1; i > 0; --i)
        {
            if (items[i]->IsSeparator())
            {
                m_ProjectMenu->InsertSeparator(i);
                m_ProjectMenu->Insert(i + 1, idCurrentProjectReparse, _("Reparse active project"), _("Reparse of the final switched project"));
                inserted = true;
                break;
            }
        }

        // not found, just append
        if (!inserted)
        {
            m_ProjectMenu->AppendSeparator();
            m_ProjectMenu->Append(idCurrentProjectReparse, _("Reparse active project"), _("Reparse of the final switched project"));
        }
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Project menu!"));
}
// ----------------------------------------------------------------------------
void CodeCompletion::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
// ----------------------------------------------------------------------------
{
    // if not attached, exit
    if (!menu || !IsAttached() || !m_InitDone)
        return;

    // User must specifically enable Code completion
    if (not m_CodeCompletionEnabled) return;

    if (type == mtEditorManager)
    {
        if (cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
        {
            if ( !IsProviderFor(ed) )
                return;
        }

        wxString NameUnderCursor;
        bool IsInclude = false;
        const bool nameUnderCursor = CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude);
        if (nameUnderCursor)
        {
            PluginManager *pluginManager = Manager::Get()->GetPluginManager();

            if (IsInclude)
            {
                wxString msg;
                msg.Printf(_("Open #include file: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(0, idOpenIncludeFile, msg);
                menu->Insert(1, wxID_SEPARATOR, wxEmptyString);
                pluginManager->RegisterFindMenuItems(true, 2);
            }
            else
            {
                int initialPos = pluginManager->GetFindMenuItemFirst();
                int pos = initialPos;
                wxString msg;
                msg.Printf(_("Find declaration of: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(pos++, idGotoDeclaration, msg);

                msg.Printf(_("Find implementation of: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(pos++, idGotoImplementation, msg);

                cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor(); //(ph 2021/01/18)
                if (GetParseManager()->GetParser().Done())
                {
                    msg.Printf(_("Find references of: '%s'"), NameUnderCursor.wx_str());
                    menu->Insert(pos++, idMenuFindReferences, msg);
                }
                else if ( pEditor and GetLSPclient(pEditor) //(ph 2021/01/18)
                        and GetLSP_Initialized(pEditor) and GetLSPclient(pEditor)->GetLSP_IsEditorParsed(pEditor) )
                 {
                     msg.Printf(_("Find references of: '%s'"), NameUnderCursor.wx_str());
                     menu->Insert(pos++, idMenuFindReferences, msg);
                 }

                pluginManager->RegisterFindMenuItems(false, pos - initialPos);
            }
        }

        const int insertId = menu->FindItem(_("Insert/Refactor"));
        if (insertId != wxNOT_FOUND)
        {
            if (wxMenuItem* insertMenu = menu->FindItem(insertId, 0))
            {
                if (wxMenu* subMenu = insertMenu->GetSubMenu())
                {
                    subMenu->Append(idClassMethod, _("Class method declaration/implementation..."));
                    subMenu->Append(idUnimplementedClassMethods, _("All class methods without implementation..."));

                    subMenu->AppendSeparator();

                    const bool enableRename = (GetParseManager()->GetParser().Done() && nameUnderCursor && !IsInclude);
                    subMenu->Append(idMenuRenameSymbols, _("Rename symbols"), _("Rename symbols under cursor"));
                    subMenu->Enable(idMenuRenameSymbols, enableRename);
                }
                else
                    CCLogger::Get()->DebugLog(_T("Could not find Insert menu 3!"));

                if (wxFound(insertId)) //(ph 2021/11/16)
                {
                    // insert "Reparse this file" under "Insert/Refactor"
                    size_t posn = 0;
                    wxMenuItem* insertMenuItem = menu->FindChildItem(insertId, &posn);
                    if (insertMenuItem)
                        menu->Insert(posn+1, idEditorFileReparse, _("Reparse this file"), _("Reparse current editors file"));
                }
            }
            else
                CCLogger::Get()->DebugLog(_T("Could not find Insert menu 2!"));
        }
        else
            CCLogger::Get()->DebugLog(_T("Could not find Insert menu!"));
    }
    else if (type == mtProjectManager)
    {
        if (data)
        {
            if (data->GetKind() == FileTreeData::ftdkProject)
            {
                size_t position = menu->GetMenuItemCount();
                int id = menu->FindItem(_("Build"));
                if (id != wxNOT_FOUND)
                    menu->FindChildItem(id, &position);
                menu->Insert(position, idSelectedProjectReparse, _("Reparse this project"), _("Reparse current actived project"));
                cbProject* pProject = data->GetProject();
                if (pProject)
                {
                    Parser* pParser = dynamic_cast<Parser*>(GetParseManager()->GetParserByProject(pProject));
                    if (pParser)
                    {
                        menu->InsertCheckItem(position + 1, idPauseParsing, _("Pause parsing (toggle)"), _("Toggle Resume/Pause LSP parsing"));
                        menu->Check(idPauseParsing, pParser and pParser->GetUserParsingPaused());
                    }
                }
                else
                    menu->Check(idPauseParsing, false);
            }
            else if (data->GetKind() == FileTreeData::ftdkFile)
                menu->Append(idSelectedFileReparse, _("Reparse this file"), _("Reparse current selected file"));
        }
    }
}
// ----------------------------------------------------------------------------
bool CodeCompletion::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{
    // User must specifically enable Code completion
    if (not m_CodeCompletionEnabled) return false;

    // load the toolbar resource
    Manager::Get()->AddonToolBar(toolBar,_T("codecompletion_toolbar"));
    // get the wxChoice control pointers
    m_Function = XRCCTRL(*toolBar, "chcCodeCompletionFunction", wxChoice);
    m_Scope    = XRCCTRL(*toolBar, "chcCodeCompletionScope",    wxChoice);

    m_ToolBar = toolBar;

    // set the wxChoice and best toolbar size
    UpdateToolBar();

    // disable the wxChoices
    EnableToolbarTools(false);

    return true;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnIdle(wxIdleEvent& event) //(ph 2020/10/24)
// ----------------------------------------------------------------------------
{
    event.Skip(); //always event.Skip() to allow others use of idle events

    // User must specifically enable Code completion
    if (not m_CodeCompletionEnabled) return;

    if (ProjectManager::IsBusy() or (not IsAttached()) or (not m_InitDone) )
        return;

    // Honor a pending code completion request when user stops typing
    if (m_PendingCompletionRequest) //(ph 2021/01/31)
    {
        m_PendingCompletionRequest = false;
        CodeBlocksEvent evt(cbEVT_COMPLETE_CODE);
        Manager::Get()->ProcessEvent(evt);
    }

}
// ----------------------------------------------------------------------------
CodeCompletion::CCProviderStatus CodeCompletion::GetProviderStatusFor(cbEditor* ed)
// ----------------------------------------------------------------------------
{
    // User must specifically enable Code completion
    if (not m_CodeCompletionEnabled) return ccpsInactive;

    EditorColourSet *colour_set = ed->GetColourSet();
    if (colour_set && ed->GetLanguage() == colour_set->GetHighlightLanguage(wxT("C/C++")))
        return ccpsActive;

    switch (ParserCommon::FileType(ed->GetFilename()))
    {
        case ParserCommon::ftHeader:
        case ParserCommon::ftSource:
            return ccpsActive;

        case ParserCommon::ftOther:
            return ccpsInactive;
        default:
            break;
    }
    return ccpsUniversal;
}
// ----------------------------------------------------------------------------
std::vector<CodeCompletion::CCToken> CodeCompletion::GetAutocompList(bool isAuto, cbEditor* ed, int& tknStart, int& tknEnd)
// ----------------------------------------------------------------------------
{
    // Called directly from ccmanager.cpp to get list of completions

    // On the first call from ccmanager we send clangd a completion request, then return enpty tokens as empty.
    // This event will be reissued after OnLSP_completionResponse() receives the completion items.
    // This routine will be entered the second time after clangd returns the completions and
    // function OnLSP_CompletionResponse() has filled in m_completionsTokens vector.
    // OnLSP_CompletionsResponse() will reissues the cbEVT_COMPLETE_CODE event to re-enter here the second time.
    // This routine will then return a filled-in tokens vector with the completions from m_completionsTokens.

    std::vector<CCToken> tokens;

    if (!IsAttached() || !m_InitDone)
        return tokens;

    cbStyledTextCtrl* stc = ed->GetControl();
    const int style = stc->GetStyleAt(tknEnd);
    const wxChar curChar = stc->GetCharAt(tknEnd - 1);

    if (isAuto) // filter illogical cases of auto-launch
    {
        // AutocompList can be prompt after user typed "::" or "->"
        // or if in preprocessor directive, after user typed "<" or "\"" or "/"
        if (   (   curChar == wxT(':') // scope operator
                && stc->GetCharAt(tknEnd - 2) != wxT(':') )
            || (   curChar == wxT('>') // '->'
                && stc->GetCharAt(tknEnd - 2) != wxT('-') )
            || (   wxString(wxT("<\"/")).Find(curChar) != wxNOT_FOUND // #include directive
                && !stc->IsPreprocessor(style) ) )
        {
            return tokens;
        }
    }
    // ----------------------------------------------------------------------------
    // LSP Code Completion              //(ph 2020/11/22)
    // ----------------------------------------------------------------------------
    // On second call from ccmanager, we should have some tokens to return
    // else the second call is never initiated by OnLSP_Completion().
    if (m_CompletionTokens.size() )
    {
        // We have some completions, hand them back to ccmanager

        // **debugging** LogManager* pLogMgr = CCLogger::Get()->;
        //for (size_t tknNdx; tknNdx<m_CompletionTokens.size(); ++tknNdx)
        //for (cbCodeCompletionPlugin::CCToken tknNdx : m_CompletionTokens)

        // **debugging** pLogMgr->DebugLog("-------------------Completions-------------------------");

        for(size_t ii=0; ii<m_CompletionTokens.size(); ++ii)
        {
            // **debugging** CCToken look = m_CompletionTokens[ii];
            tokens.push_back(m_CompletionTokens[ii]);
            // **debugging** info
            //wxString cmpltnStr = wxString::Format(
            //        "Completion:id[%d],category[%d],weight[%d],displayName[%s],name[%s]",
            //                        tokens[ii].id,
            //                        tokens[ii].category,
            //                        tokens[ii].weight,
            //                        tokens[ii].displayName,
            //                        tokens[ii].name
            //                        );
            //pLogMgr->DebugLog(cmpltnStr);
        }
        m_CompletionTokens.clear();
        return tokens;
    }

    // We have no completion data, issue a LSP_Completion() call, and return.
    // When the OnLSP_Completionresponse() event occurs, it will re-enter this function
    // with m_CompletionTokens full of clangd completion items.
    if (GetLSP_Initialized(ed) )
    {
        if (   stc->IsString(style)
            || stc->IsComment(style)
            || stc->IsCharacter(style)
            || stc->IsPreprocessor(style) )
        {
            return tokens; //For styles above ignore this request
        }

        //For users who type faster, say at 75 WPM, the gap that would indicate the end of typing would be only 0.3 seconds (300 milliseconds.)
        int mSecsSinceLastModify = GetLSPclient(ed)->GetDurationMilliSeconds(m_LastModificationMilliTime);
        if (mSecsSinceLastModify > m_CCDelay)
        {
            // FYI: LSP_Completion() will send LSP_DidChange() notification to LSP server for the current line.
            // else LSP may crash for out-of-range conditions trying to complete text it's never seen.

            // Ignore completing tokens ending in blank, CR, or LF
            m_PendingCompletionRequest = false;
            if ( (curChar == ' ') or (curChar == '\n') or (curChar == '\r') )
                return tokens;  //return empty tokens
            GetLSPclient(ed)->LSP_Completion(ed);
        }
        else {
            // time between typed keys too short. Wait awhile.
            m_PendingCompletionRequest = true;
            wxWakeUpIdle();
        }
    }

    return tokens; //return empty tokens on first call from ccmanager.

    //(ph 2021/01/27) useful debugging output
    //for(size_t ii=0; ii< tokens.size(); ++ii)
    //{
    //    CCToken look = tokens[ii];
    //    int id = look.id;                        //!< CCManager will pass this back unmodified. Use it as an internal identifier for the token.
    //    int category = look.category;;           //!< The category corresponding to the index of the registered image (during autocomplete).
    //    int weight = look.weight;                //!< Lower numbers are placed earlier in listing, 5 is default; try to keep 0-10.
    //    wxString displayName = look.displayName; //!< Verbose string representing the token.
    //    wxString name = look.name;               //!< Minimal name of the token that CCManager may choose to display in restricted circumstances.
    //    wxString msg = wxString::Format("CCToken(%d) id:%d category:%d weight:%d dispName:%s name:%s",
    //                ii, id, category, weight, displayName, name);
    //    CCLogger::Get()->DebugLog(msg);
    //}

    return tokens;
}
// ----------------------------------------------------------------------------
static int CalcStcFontSize(cbStyledTextCtrl *stc)
// ----------------------------------------------------------------------------
{
    wxFont defaultFont = stc->StyleGetFont(wxSCI_STYLE_DEFAULT);
    defaultFont.SetPointSize(defaultFont.GetPointSize() + stc->GetZoom());
    int fontSize;
    stc->GetTextExtent(wxT("A"), nullptr, &fontSize, nullptr, nullptr, &defaultFont);
    return fontSize;
}
// unused for clangd at present (2021/10/14) but may be useful in the future
void CodeCompletion::DoCodeCompletePreprocessor(int tknStart, int tknEnd, cbEditor* ed, std::vector<CCToken>& tokens)
{
    cbStyledTextCtrl* stc = ed->GetControl();
    if (stc->GetLexer() != wxSCI_LEX_CPP)
    {
        const FileType fTp = FileTypeOf(ed->GetShortName());
        if (   fTp != ftSource
            && fTp != ftHeader
            && fTp != ftTemplateSource
            && fTp != ftResource )
        {
            return; // not C/C++
        }
    }
    const wxString text = stc->GetTextRange(tknStart, tknEnd);

    wxStringVec macros;
    macros.push_back(wxT("define"));
    macros.push_back(wxT("elif"));
    macros.push_back(wxT("elifdef"));
    macros.push_back(wxT("elifndef"));
    macros.push_back(wxT("else"));
    macros.push_back(wxT("endif"));
    macros.push_back(wxT("error"));
    macros.push_back(wxT("if"));
    macros.push_back(wxT("ifdef"));
    macros.push_back(wxT("ifndef"));
    macros.push_back(wxT("include"));
    macros.push_back(wxT("line"));
    macros.push_back(wxT("pragma"));
    macros.push_back(wxT("undef"));
    const wxString idxStr = F(wxT("\n%d"), PARSER_IMG_MACRO_DEF);
    for (size_t i = 0; i < macros.size(); ++i)
    {
        if (text.IsEmpty() || macros[i][0] == text[0]) // ignore tokens that start with a different letter
            tokens.push_back(CCToken(wxNOT_FOUND, macros[i], PARSER_IMG_MACRO_DEF));
    }
    stc->ClearRegisteredImages();
    const int fontSize = CalcStcFontSize(stc);
    stc->RegisterImage(PARSER_IMG_MACRO_DEF,
                       GetParseManager()->GetImageList(fontSize)->GetBitmap(PARSER_IMG_MACRO_DEF));
}
// ----------------------------------------------------------------------------
std::vector<CodeCompletion::CCCallTip> CodeCompletion::GetCallTips(int pos, int style, cbEditor* ed, int& argsPos)
// ----------------------------------------------------------------------------
{
    std::vector<CCCallTip> tips;
    if (!IsAttached() || !m_InitDone || style == wxSCI_C_WXSMITH || !GetParseManager()->GetParser().Done())
        return tips;

    // If waiting for clangd LSP_HoverResponse() return empty tips
    if (m_HoverIsActive)
        return tips;    //empty tips

    // If not waiting for Hover response, and the signature help tokens are empty,
    // issue a LSP_SignatureHelp request.
    // This routine will be re-invoked after OnLSP_SignatureHelpResponse() fills in
    // the m_SignatureTokens.
    if (0 == m_SignatureTokens.size())
    {
        GetLSPclient(ed)->LSP_SignatureHelp(ed, pos);
        return tips; //empty tips
    }
    for(unsigned ii=0; ii < m_SignatureTokens.size(); ++ii)
    {
        tips.push_back(m_SignatureTokens[ii]);
    }
    m_SignatureTokens.clear(); //so we can ask for Signatures again
    return tips; //signature Help entries from clangd

}
// ----------------------------------------------------------------------------
wxString CodeCompletion::GetDocumentation(const CCToken& token)
// ----------------------------------------------------------------------------
{
    return m_DocHelper.GenerateHTML(token.id, GetParseManager()->GetParser().GetTokenTree());
}
// ----------------------------------------------------------------------------
std::vector<CodeCompletion::CCToken> CodeCompletion::GetTokenAt(int pos, cbEditor* ed, bool& WXUNUSED(allowCallTip))
// ----------------------------------------------------------------------------
{
    std::vector<CCToken> tokens;
    if (!IsAttached() || !m_InitDone)
        return tokens; //It's empty

    m_HoverIsActive = false;

    // ignore comments, strings, preprocessors, etc
    cbStyledTextCtrl* stc = ed->GetControl();
    const int style = stc->GetStyleAt(pos);
    if (   stc->IsString(style)
        || stc->IsComment(style)
        || stc->IsCharacter(style)
        || stc->IsPreprocessor(style) )
    {
        return tokens; //It's empty
    }

    // ----------------------------------------------------
    // LSP Hover
    // ----------------------------------------------------
    // On second call from ccmanager, we should have some tokens to return
    // else the second call is never initiated by OnLSP_HoverResponse().
    if (m_HoverTokens.size() )
    {
        tokens.clear();
        wxString hoverMsg = wxString::Format("GetTokenAt() sees %d tokens.\n", m_HoverTokens.size());
        CCLogger::Get()->DebugLog(hoverMsg);
        for(size_t ii=0; ii<m_HoverTokens.size(); ++ii)
        {
            CCToken look = m_HoverTokens[ii]; //debugging
            tokens.push_back(m_HoverTokens[ii]);
        }
        m_HoverTokens.clear();
        return tokens;
    }
    // On the first call from ccmanager, issue LSP_Hover() to clangd and return empty tokens
    // while waiting for clangd to respond. Once we get response data, OnLSP_HoverResponse()
    // will re-issue this event (cbEVT_EDITOR_TOOLTIP) to display the results.
    if (GetLSP_Initialized(ed) )
    {
        m_HoverIsActive = true;
        m_HoverLastPosition = pos;
        GetLSPclient(ed)->LSP_Hover(ed, pos);
    }
    tokens.clear();
    return tokens; //return empty tokens on first call from ccmanager.

}
// ----------------------------------------------------------------------------
wxString CodeCompletion::OnDocumentationLink(wxHtmlLinkEvent& event, bool& dismissPopup)
// ----------------------------------------------------------------------------
{
    // user has clicked link in HTML documentation popup window
    return m_DocHelper.OnDocumentationLink(event, dismissPopup);
}
// ----------------------------------------------------------------------------
void CodeCompletion::DoAutocomplete(const CCToken& token, cbEditor* ed)
// ----------------------------------------------------------------------------
{
    // wxScintilla Callback after code completion selection

    // Finish code completion for LSP
    return LSP_DoAutocomplete(token, ed); // Finish code completion for LSP
}
// ----------------------------------------------------------------------------
void CodeCompletion::LSP_DoAutocomplete(const CCToken& token, cbEditor* ed)     //(ph 2021/03/8)
// ----------------------------------------------------------------------------
{
    // wxScintilla Callback after code completion selection

    wxString itemText = CodeCompletionHelper::AutocompGetName(token.displayName);
    cbStyledTextCtrl* stc = ed->GetControl();

    int curPos = stc->GetCurrentPos();
    int startPos = stc->WordStartPosition(curPos, true);
    if (   itemText.GetChar(0) == _T('~') // special handle for dtor
        && startPos > 0
        && stc->GetCharAt(startPos - 1) == _T('~'))
    {
        --startPos;
    }
    bool needReparse = false;

    if (stc->IsPreprocessor(stc->GetStyleAt(curPos)))
    {
        curPos = stc->GetLineEndPosition(stc->GetCurrentLine()); // delete rest of line
        bool addComment = (itemText == wxT("endif"));
        for (int i = stc->GetCurrentPos(); i < curPos; ++i)
        {
            if (stc->IsComment(stc->GetStyleAt(i)))
            {
                curPos = i; // preserve line comment
                if (wxIsspace(stc->GetCharAt(i - 1)))
                    --curPos; // preserve a space before the comment
                addComment = false;
                break;
            }
        }
        if (addComment) // search backwards for the #if*
        {
            wxRegEx ppIf(wxT("^[ \t]*#[ \t]*if"));
            wxRegEx ppEnd(wxT("^[ \t]*#[ \t]*endif"));
            int depth = -1;
            for (int ppLine = stc->GetCurrentLine() - 1; ppLine >= 0; --ppLine)
            {
                if (stc->GetLine(ppLine).Find(wxT('#')) != wxNOT_FOUND) // limit testing due to performance cost
                {
                    if (ppIf.Matches(stc->GetLine(ppLine))) // ignore else's, elif's, ...
                        ++depth;
                    else if (ppEnd.Matches(stc->GetLine(ppLine)))
                        --depth;
                }
                if (depth == 0)
                {
                    wxRegEx pp(wxT("^[ \t]*#[ \t]*[a-z]*([ \t]+([a-zA-Z0-9_]+)|())"));
                    pp.Matches(stc->GetLine(ppLine));
                    if (!pp.GetMatch(stc->GetLine(ppLine), 2).IsEmpty())
                        itemText.Append(wxT(" // ") + pp.GetMatch(stc->GetLine(ppLine), 2));
                    break;
                }
            }
        }
        needReparse = true;

        int   pos = startPos - 1;
        wxChar ch = stc->GetCharAt(pos);
        while (ch != _T('<') && ch != _T('"') && ch != _T('#') && (pos>0))
            ch = stc->GetCharAt(--pos);
        if (ch == _T('<') || ch == _T('"'))
            startPos = pos + 1;

        if (ch == _T('"'))
            itemText << _T('"');
        else if (ch == _T('<'))
            itemText << _T('>');
    }
    else
    {
        const int endPos = stc->WordEndPosition(curPos, true);
        const wxString& alreadyText = stc->GetTextRange(curPos, endPos);
        if (!alreadyText.IsEmpty() && itemText.EndsWith(alreadyText))
            curPos = endPos;
    }

    int positionModificator = 0;
    bool insideParentheses = false;
    if (token.id != -1 && m_CCAutoAddParentheses)
    {
        //CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        //TokenTree* tree = GetParseManager()->GetParser().GetTokenTree();
        //const Token* tkn = tree->at(token.id);

        //if (!tkn)
        //{   CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex) }
        //else
        {
            //bool addParentheses = tkn->m_TokenKind & tkAnyFunctio
            bool addParentheses = ( (token.id == LSP_SymbolKind::Function)
                                   or token.displayName.BeforeFirst(' ').Contains("(") );

            // LSP does not yet provide arguments
            //if (!addParentheses && (tkn->m_TokenKind & tkMacroDef))
            //{
            //    if (tkn->m_Args.size() > 0)
            //        addParentheses = true;
            //}

            //// cache args to avoid locking
            //wxString tokenArgs = tkn->GetStrippedArgs();

            wxString tokenArgs = "";
            if (addParentheses)
            {
                tokenArgs = token.displayName.BeforeFirst(')') + ")";
                tokenArgs = tokenArgs.AfterFirst('(');
                tokenArgs.Prepend("(");
            }

            //CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
            //
            if (addParentheses)
            {
                bool insideFunction = true;
                //if (m_CCDetectImplementation)
                //{
                //    ccSearchData searchData = { stc, ed->GetFilename() };
                //    int funcToken;
                //    if (GetParseManager()->FindCurrentFunctionStart(&searchData, 0, 0, &funcToken) == -1)
                //    {
                //        // global scope
                //        itemText += tokenArgs;
                //        insideFunction = false;
                //    }
                //    else // Found something, but result may be false positive.
                //    {
                //        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
                //
                //        const Token* parent = tree->at(funcToken);
                //        // Make sure that parent is not container (class, etc)
                //        if (parent && (parent->m_TokenKind & tkAnyFunction) == 0)
                //        {
                //            // class scope
                //            itemText += tokenArgs;
                //            insideFunction = false;
                //        }
                //
                //        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
                //    }
                //}

                //(ph 2021/01/27) compensate for above commented code
                // FIXME (ph#): commented code above should be implemented for LSP
                if (tokenArgs.size() > 2) // more than '()'
                {
                    itemText += tokenArgs;
                    insideFunction = false;

                }
                if (insideFunction)
                {
                    // Inside block
                    // Check if there are brace behind the target
                    if (stc->GetCharAt(curPos) != _T('('))
                    {
                        itemText += _T("()");
                        if (tokenArgs.size() > 2) // more than '()'
                        {
                            positionModificator = -1;
                            insideParentheses = true;
                        }
                    }
                    else
                        positionModificator = 1; // Set caret after '('
                }
            }
        } // if tkn
    } // if token.id

    stc->SetTargetStart(startPos);
    stc->SetTargetEnd(curPos);

    stc->AutoCompCancel();
    if (stc->GetTextRange(startPos, curPos) != itemText)
        stc->ReplaceTarget(itemText);
    stc->GotoPos(startPos + itemText.Length() + positionModificator);

    if (insideParentheses)
    {
        stc->EnableTabSmartJump();
        int tooltipMode = Manager::Get()->GetConfigManager(wxT("ccmanager"))->ReadInt(wxT("/tooltip_mode"), 1);
        if (tooltipMode != 3) // keybound only
        {
            CodeBlocksEvent evt(cbEVT_SHOW_CALL_TIP);
            Manager::Get()->ProcessEvent(evt);
        }
    }

    if (needReparse)
    {
        TRACE(F("CodeCompletion::%s: Starting m_TimerRealtimeParsing.", __FUNCTION__));
        m_TimerRealtimeParsing.Start(1, wxTIMER_ONE_SHOT);
    }
    stc->ChooseCaretX();
}
// ----------------------------------------------------------------------------
void CodeCompletion::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
// ----------------------------------------------------------------------------
{
    if (!IsAttached() || !m_InitDone)
    {
        event.Skip();
        return;
    }

    if ( !IsProviderFor(editor) )
    {
        event.Skip();
        return;
    }

    cbStyledTextCtrl* control = editor->GetControl();

    if      (event.GetEventType() == wxEVT_SCI_CHARADDED)
    {   TRACE(_T("wxEVT_SCI_CHARADDED")); }
    else if (event.GetEventType() == wxEVT_SCI_CHANGE)
    {   TRACE(_T("wxEVT_SCI_CHANGE")); }
    else if (event.GetEventType() == wxEVT_SCI_MODIFIED)
    {   TRACE(_T("wxEVT_SCI_MODIFIED")); }
    else if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_SELECTION)
    {   TRACE(_T("wxEVT_SCI_AUTOCOMP_SELECTION")); }
    else if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_CANCELLED)
    {   TRACE(_T("wxEVT_SCI_AUTOCOMP_CANCELLED")); }

    // if the user is modifying the editor, then CC should try to reparse the editor's content
    // and update the token tree.
    if (   GetParseManager()->GetParser().Options().whileTyping
        && (   (event.GetModificationType() & wxSCI_MOD_INSERTTEXT)
            || (event.GetModificationType() & wxSCI_MOD_DELETETEXT) ) )
    {
        m_NeedReparse = true;
    }
    // ----------------------------------------------------------------------------
    // Support for LSP code completion calls with keyboard dwell time (ph 2021/01/31)
    // ----------------------------------------------------------------------------
    if (   ((event.GetModificationType() & wxSCI_MOD_INSERTTEXT)
            || (event.GetModificationType() & wxSCI_MOD_DELETETEXT))
            and (GetLSPclient(editor))
        )
    {
        // set time of modification
        m_LastModificationMilliTime =  GetLSPclient(editor)->GetNowMilliSeconds();
        GetLSPclient(editor)->SetLSP_EditorModified(editor, true);

        // Ctrl-Z undo and redo may not set EditorBase::SetModified() especially on
        // the last undo/redo which places the editor text in the original condition.
        // But this leaves the LSP server in an out of sync condition.
        // Resync the LSP server to the current editor text.
        if (not editor->GetModified())
            GetLSPclient(editor)->LSP_DidChange(editor);
    }

    if (control->GetCurrentLine() != m_CurrentLine)
    {
        // reparsing the editor only happens in the condition that the caret's line number
        // is changed.
        if (m_NeedReparse)
        {
            TRACE(_T("CodeCompletion::EditorEventHook: Starting m_TimerRealtimeParsing."));
            m_TimerRealtimeParsing.Start(REALTIME_PARSING_DELAY, wxTIMER_ONE_SHOT);
            m_CurrentLength = control->GetLength();
            m_NeedReparse = false;
        }
        // wxEVT_SCI_UPDATEUI will be sent on caret's motion, but we are only interested in the
        // cases where line number is changed. Then we need to update the CC's toolbar.
        if (event.GetEventType() == wxEVT_SCI_UPDATEUI)
        {
            m_ToolbarNeedRefresh = true;
            TRACE(_T("CodeCompletion::EditorEventHook: Starting m_TimerToolbar."));
            if (m_TimerEditorActivated.IsRunning())
                m_TimerToolbar.Start(EDITOR_ACTIVATED_DELAY + 1, wxTIMER_ONE_SHOT);
            else
                m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
        }
    }

    // allow others to handle this event
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeCompletion::RereadOptions()
// ----------------------------------------------------------------------------
{
    // Keep this in sync with CCOptionsDlg::CCOptionsDlg and CCOptionsDlg::OnApply

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("clangd_client"));

    m_LexerKeywordsToInclude[0] = cfg->ReadBool(_T("/lexer_keywords_set1"), true);
    m_LexerKeywordsToInclude[1] = cfg->ReadBool(_T("/lexer_keywords_set2"), true);
    m_LexerKeywordsToInclude[2] = cfg->ReadBool(_T("/lexer_keywords_set3"), false);
    m_LexerKeywordsToInclude[3] = cfg->ReadBool(_T("/lexer_keywords_set4"), false);
    m_LexerKeywordsToInclude[4] = cfg->ReadBool(_T("/lexer_keywords_set5"), false);
    m_LexerKeywordsToInclude[5] = cfg->ReadBool(_T("/lexer_keywords_set6"), false);
    m_LexerKeywordsToInclude[6] = cfg->ReadBool(_T("/lexer_keywords_set7"), false);
    m_LexerKeywordsToInclude[7] = cfg->ReadBool(_T("/lexer_keywords_set8"), false);
    m_LexerKeywordsToInclude[8] = cfg->ReadBool(_T("/lexer_keywords_set9"), false);

    // for CC
    m_CCMaxMatches           = cfg->ReadInt(_T("/max_matches"),            16384);
    m_CCAutoAddParentheses   = cfg->ReadBool(_T("/auto_add_parentheses"),  true);
    m_CCDetectImplementation = cfg->ReadBool(_T("/detect_implementation"), false); //depends on auto_add_parentheses
    m_CCFillupChars          = cfg->Read(_T("/fillup_chars"),              wxEmptyString);
    m_CCDelay                = cfg->ReadInt("/cc_delay",                   300); //(ph 2021/09/2)
    m_CCEnableHeaders        = cfg->ReadBool(_T("/enable_headers"),        true);
    m_CCEnablePlatformCheck  = cfg->ReadBool(_T("/platform_check"),        true);

    // update the CC toolbar option, and tick the timer for toolbar
    // NOTE (ollydbg#1#12/06/14): why?
    if (m_ToolBar)
    {
        UpdateToolBar();
        CodeBlocksLayoutEvent evt(cbEVT_UPDATE_VIEW_LAYOUT);
        Manager::Get()->ProcessEvent(evt);
        m_ToolbarNeedReparse = true;
        TRACE(_T("CodeCompletion::RereadOptions: Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    }

    m_DocHelper.RereadOptions(cfg);
}
// ----------------------------------------------------------------------------
void CodeCompletion::UpdateToolBar()
// ----------------------------------------------------------------------------
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("clangd_client"));
    const bool showScope = cfg->ReadBool(_T("/scope_filter"), true);
    const int scopeLength = cfg->ReadInt(_T("/toolbar_scope_length"), 280);
    const int functionLength = cfg->ReadInt(_T("/toolbar_function_length"), 660);

    if (showScope && !m_Scope)
    {
        // Show the scope choice
        m_Scope = new wxChoice(m_ToolBar, XRCID("chcCodeCompletionScope"), wxPoint(0, 0), wxSize(scopeLength, -1), 0, 0);
        m_ToolBar->InsertControl(0, m_Scope);
    }
    else if (!showScope && m_Scope)
    {
        // Hide the scope choice
        m_ToolBar->DeleteTool(m_Scope->GetId());
        m_Scope = nullptr;
    }
    else if (m_Scope)
    {
        // Just apply new size to scope choice
        m_Scope->SetSize(wxSize(scopeLength, -1));
    }

    m_Function->SetSize(wxSize(functionLength, -1));

    m_ToolBar->Realize();
    m_ToolBar->SetInitialSize();
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnUpdateUI(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    wxString NameUnderCursor;
    bool IsInclude = false;
    const bool HasNameUnderCursor = CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude);

    const bool HasEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() != 0;
    if (m_EditMenu)
    {
        const bool RenameEnable = HasNameUnderCursor && !IsInclude && GetParseManager()->GetParser().Done();
        m_EditMenu->Enable(idMenuRenameSymbols, RenameEnable);
    }

    if (m_SearchMenu)
    {
        m_SearchMenu->Enable(idMenuGotoFunction,       HasEd);
        m_SearchMenu->Enable(idMenuGotoPrevFunction,   HasEd);
        m_SearchMenu->Enable(idMenuGotoNextFunction,   HasEd);

        const bool GotoEnable = HasNameUnderCursor && !IsInclude;
        m_SearchMenu->Enable(idMenuGotoDeclaration,    GotoEnable);
        m_SearchMenu->Enable(idMenuGotoImplementation, GotoEnable);
        const bool FindEnable = HasNameUnderCursor && !IsInclude && GetParseManager()->GetParser().Done();
        m_SearchMenu->Enable(idMenuFindReferences, FindEnable);
        const bool IncludeEnable = HasNameUnderCursor && IsInclude;
        m_SearchMenu->Enable(idMenuOpenIncludeFile, IncludeEnable);
    }

    if (m_ViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)GetParseManager()->GetClassBrowser());
        m_ViewMenu->Check(idViewClassBrowser, isVis);
    }

    if (m_ProjectMenu)
    {
        cbProject* pActivePrj = Manager::Get()->GetProjectManager()->GetActiveProject();
        m_ProjectMenu->Enable(idCurrentProjectReparse, pActivePrj);
    }

    // must do...
    event.Skip();
}//OnUpdateUI
// ----------------------------------------------------------------------------
void CodeCompletion::OnViewClassBrowser(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (!Manager::Get()->GetConfigManager(_T("clangd_client"))->ReadBool(_T("/use_symbols_browser"), true))
    {
        cbMessageBox(_("The symbols browser is disabled in code-completion options.\n"
                        "Please enable it there first..."), _("Information"), wxICON_INFORMATION);
        return;
    }
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = (wxWindow*)GetParseManager()->GetClassBrowser();
    Manager::Get()->ProcessEvent(evt);
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnGotoFunction(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    TRACE(_T("OnGotoFunction"));

    // --------------------------------------------------------
    // LSP GoToFunction checks
    // --------------------------------------------------------
    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (not pProject) return;
    if (not GetLSPclient(pProject) )
        return;
    if (not GetLSP_Initialized(ed) )
    {
       InfoWindow::Display("LSP", wxString::Format("%s\n not yet parsed.", ed->GetFilename()) );
       return;
    }
    if ((not GetLSPclient(ed)) or (not GetLSPclient(ed)->GetLSP_IsEditorParsed(ed)) )
    {
       InfoWindow::Display("LSP",wxString::Format("%s\n not yet parsed.", ed->GetFilename()) );
       return;
    }

    TokenTree* tree = nullptr;

    //the LSP way to gather functions from token tree
    tree = GetParseManager()->GetParser().GetTokenTree();

    // -----------------------------------------------------
    //CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
    // -----------------------------------------------------
    auto locker_result = s_TokenTreeMutex.LockTimeout(250);
    if (locker_result != wxMUTEX_NO_ERROR)
    {
        // lock failed, do not block the UI thread, call back when idle
        GetIdleCallbackHandler()->QueueCallback(this, &CodeCompletion::OnGotoFunction, event);
        return;
    }
    else /*lock succeeded*/
        s_TokenTreeMutex_Owner = wxString::Format("%s %d",__PRETTY_FUNCTION__, __LINE__); /*record owner*/


    if ( (not tree) or tree->empty())
    {
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        cbMessageBox(_("No functions parsed in this file..."));
    }
    else
    {
        wxString edFilename = ed->GetFilename();
        edFilename.Replace('\\','/');
        GotoFunctionDlg::Iterator iterator;

        for (size_t i = 0; i < tree->size(); i++)
        {
            Token* token = tree->at(i);
            bool isImpl = FileTypeOf(edFilename) == ftSource;
            if ( (not isImpl) && token && (token->GetFilename() != edFilename) ) continue;      //(ph 2021/05/22)
            if ( isImpl && token &&   (token->GetImplFilename() != edFilename) ) continue;        //(ph 2021/05/22)
            if ( token && (token->m_TokenKind & tkAnyFunction) )
            {
                //wxString tknFilename = token->GetFilename();          //**debugging**
                //wxString tknImplFilename = token->GetImplFilename();  //**debugging**
                GotoFunctionDlg::FunctionToken ft;
                // We need to clone the internal data of the strings to make them thread safe.
                ft.displayName = wxString(token->DisplayName().c_str());
                ft.name = wxString(token->m_Name.c_str());
                ft.line = token->m_Line;
                ft.implLine = token->m_ImplLine;
                if (!token->m_FullType.empty())
                    ft.paramsAndreturnType = wxString((token->m_Args + wxT(" -> ") + token->m_FullType).c_str());
                else
                    ft.paramsAndreturnType = wxString(token->m_Args.c_str());
                ft.funcName = wxString((token->GetNamespace() + token->m_Name).c_str());

                iterator.AddToken(ft);
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        iterator.Sort();
        GotoFunctionDlg dlg(Manager::Get()->GetAppWindow(), &iterator);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() == wxID_OK)
        {
            int selection = dlg.GetSelection();
            if (selection != wxNOT_FOUND) {
                const GotoFunctionDlg::FunctionToken *ft = iterator.GetToken(selection);
                if (ed && ft)
                {
                    TRACE(F(_T("OnGotoFunction() : Token '%s' found at line %u."), ft->name.wx_str(), ft->line));
                    if (FileTypeOf(edFilename) == ftSource)                       //(ph 2021/05/22)
                        ed->GotoTokenPosition(ft->implLine - 1, ft->name);
                    else                                                        //(ph 2021/05/22)
                        ed->GotoTokenPosition(ft->line - 1, ft->name);
                }
            }
        }
    }
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnGotoPrevFunction(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed) return;
    if (not GetLSP_Initialized(ed) ) return;

    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (not pProject) return;
    Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
    if (not pParser) return;
    // Register a callback redirected to OnLSP_GoToPrevFunctionResponse() for the LSP response
    size_t id = GetParseManager()->GetLSPEventSinkHandler()->LSP_RegisterEventSink(XRCID("textDocument/documentSymbol"), pParser, &Parser::OnLSP_GoToPrevFunctionResponse, event);
    // Ask clangd for symbols in this editor, OnLSP_GoToPrevFunctionResponse() will handle the response
    GetLSPclient(ed)->LSP_RequestSymbols(ed, id);
    return;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnGotoNextFunction(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed) return;
    if (not GetLSP_Initialized(ed) ) return;

    //-RegisterLSP_Callback(XRCID("textDocument/documentSymbol"),&CodeCompletion::OnLSP_GoToNextFunctionResponse);
    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (not pProject) return;
    Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
    if (not pParser) return;
    // Register a callback redirected to OnLSP_GoToNextFunctionResponse() for the LSP response
    size_t id = GetParseManager()->GetLSPEventSinkHandler()->LSP_RegisterEventSink(XRCID("textDocument/documentSymbol"), pParser, &Parser::OnLSP_GoToNextFunctionResponse, event);
    // Ask clangd for symbols in this editor. OnLSP_GoToNextFunctionResponse() will handle the response.
    GetLSPclient(ed)->LSP_RequestSymbols(ed, id);
    return;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnClassMethod(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)return;
    if (not GetLSP_Initialized(ed) ) return;

    //RegisterLSP_Callback(XRCID("textDocument/documentSymbol"),&CodeCompletion::OnLSP_GoToNextFunctionResponse);
    //GetLSPclient(ed)->LSP_RequestSymbols(ed);
    //return;

    DoClassMethodDeclImpl(); // **DEBUGGING**
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnUnimplementedClassMethods(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    DoAllMethodsImpl();
}

// ----------------------------------------------------------------------------
void CodeCompletion::OnGotoDeclaration(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    ProjectManager* pPrjMgr = Manager::Get()->GetProjectManager();
    cbProject* pActiveProject = pPrjMgr->GetActiveProject();
    if (not GetLSPclient(pActiveProject)) return;

    EditorManager* pEdMgr  = Manager::Get()->GetEditorManager();
    cbEditor*      pActiveEditor = pEdMgr->GetBuiltinActiveEditor();
    if (!pActiveEditor)
        return;

    TRACE(_T("OnGotoDeclaration"));

    const int pos      = pActiveEditor->GetControl()->GetCurrentPos();
    const int startPos = pActiveEditor->GetControl()->WordStartPosition(pos, true);
    const int endPos   = pActiveEditor->GetControl()->WordEndPosition(pos, true);

    wxString targetText;
    targetText << pActiveEditor->GetControl()->GetTextRange(startPos, endPos);
    if (targetText.IsEmpty())
        return;

    // prepare a boolean filter for declaration/implementation
    bool isDecl = event.GetId() == idGotoDeclaration    || event.GetId() == idMenuGotoDeclaration;
    bool isImpl = event.GetId() == idGotoImplementation || event.GetId() == idMenuGotoImplementation;
   // ----------------------------------------------------------------------------
   // LSP Goto Declaration/definition                //(ph 2020/10/12)
   // ----------------------------------------------------------------------------
    bool usingLSP_client = true;
    if (usingLSP_client)
    {
        // Assure editors file belongs to the active project (else it's not parsed yet).
        ProjectFile* pProjectFile = pActiveEditor->GetProjectFile();
        cbProject* pEdProject = pProjectFile ? pProjectFile->GetParentProject() : nullptr;
        wxString filename = pActiveEditor->GetFilename();
        if ( (not pEdProject)
             or (not (pEdProject == pActiveProject))
             or (not pActiveProject->GetFileByFilename(filename,false)) )
        {
            InfoWindow::Display("LSP " + wxString(__FUNCTION__), "Editor's file is not contained in the active project.", 6000);
            return;
        }

        if (GetLSPclient(pActiveEditor)->IsServerFilesParsing(pActiveEditor->GetFilename()) )
        {
            wxString msg = wxString::Format("LSP: Editor is being parsed. Try again...\n%s", pActiveEditor->GetShortName());
            InfoWindow::Display("LSP " + wxString(__FUNCTION__), msg, 6000);
            return;
        }
        if (not GetLSP_Initialized(pActiveEditor))
        {
            wxString msg = wxString::Format("LSP: Editor not parsed yet.\n%s", pActiveEditor->GetShortName());
            InfoWindow::Display("LSP " + wxString(__FUNCTION__), msg, 6000);
            return;
        }

        // if max parsing, spit out parsing is delayed message
        if (ParsingIsVeryBusy()) {;}

       //Confusing behaviour for original CC vs Clangd:
       // if caret is already on the definition (.h) clangd wont find it
        if (isDecl)
        {
            GetLSPclient(pActiveEditor)->LSP_GoToDeclaration(pActiveEditor, GetCaretPosition(pActiveEditor));
        }
        //Confusing behaviour of clangd which switches back and forth between def and decl
        if (isImpl)
        {
            GetLSPclient(pActiveEditor)->LSP_GoToDefinition(pActiveEditor, GetCaretPosition(pActiveEditor));
        }
        return;
    }

}//end OnGotoDeclaration()
// ----------------------------------------------------------------------------
void CodeCompletion::OnFindReferences(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    ProjectManager* pPrjMgr = Manager::Get()->GetProjectManager();
    // ----------------------------------------------------------------------------
    // LSP_FindReferences                              //(ph 2020/10/12)
    // ----------------------------------------------------------------------------
    // Call LSP now, else CodeRefactoring will change the editor
    cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not pEditor)
        return;
    ProjectFile* pProjectFile = pEditor->GetProjectFile();
    cbProject* pEdProject = pProjectFile ? pProjectFile->GetParentProject() : nullptr;

    // ----------------------------------------------------------------
    // LSP differentiate LSP vs CC
    // ----------------------------------------------------------------
    cbProject* pActiveProject = pPrjMgr->GetActiveProject();
    wxString filename = pEditor->GetFilename();
    if ( (not pEdProject)
        or (not (pEdProject == pActiveProject))
        or (not pActiveProject->GetFileByFilename(filename,false)) )
    {
        InfoWindow::Display("LSP "+wxString(__FUNCTION__), "Editor's file is not contained in the active project.", 6000);
        return;
    }

    ProcessLanguageClient* pClient = GetLSPclient(pEditor);
    if (not pClient) return;
    if (not GetLSP_Initialized(pEditor) )
    {
        InfoWindow::Display("LSP Find References", "Editor not parsed yet.", 6000);
        return;
    }
    if (pClient and pClient->IsServerFilesParsing(filename) )
    {
        InfoWindow::Display("LSP Find References", "Editor is being parsed.", 6000);
        return;
    }
    // check count of currently parsing files, and print info msg if max is parsing.
    if  (ParsingIsVeryBusy()) {;}

    GetLSPclient(pEditor)->LSP_FindReferences(pEditor, GetCaretPosition(pEditor));
    return;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnRenameSymbols(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    //-m_pCodeRefactoring->RenameSymbols();
    const wxString targetText = m_pCodeRefactoring->GetSymbolUnderCursor();
    if (targetText.IsEmpty())
        return;
    cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!pEditor)
        return;
    cbStyledTextCtrl* control = pEditor->GetControl();
    const int style = control->GetStyleAt(control->GetCurrentPos());
    if (control->IsString(style) || control->IsComment(style))
        return;
    const int pos = pEditor->GetControl()->GetCurrentPos();
    //const int start = pEditor->GetControl()->WordStartPosition(pos, true);
    //const int end = pEditor->GetControl()->WordEndPosition(pos, true);

    wxString replaceText = cbGetTextFromUser(_("Rename symbols under cursor"),
                                             _("Code Refactoring"),
                                             targetText,
                                             Manager::Get()->GetAppWindow());

    if (!replaceText.IsEmpty() && (replaceText != targetText) )
    {
        GetLSPclient(pEditor)->LSP_RequestRename(pEditor, pos, replaceText);
    }
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnOpenIncludeFile(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxString lastIncludeFileFrom;
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        lastIncludeFileFrom = editor->GetFilename();

    // check one more time because menu entries are enabled only when it makes sense
    // but the shortcut accelerator can always be executed
    bool MoveOn = false;
    wxString NameUnderCursor;
    bool IsInclude = false;
    if (CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude))
    {
        if (IsInclude)
            MoveOn = true;
    }

    if (!MoveOn)
        return; // nothing under cursor or thing under cursor is not an include

    TRACE(_T("OnOpenIncludeFile"));

    wxArrayString foundSet = GetParseManager()->GetParser().FindFileInIncludeDirs(NameUnderCursor); // search in all parser's include dirs

    // look in the same dir as the source file
    wxFileName fname = NameUnderCursor;
    wxFileName base = lastIncludeFileFrom;
    NormalizePath(fname, base.GetPath());
    if (wxFileExists(fname.GetFullPath()) )
        foundSet.Add(fname.GetFullPath());

    // search for the file in project files
    cbProject* project = GetParseManager()->GetProjectByEditor(editor);
    if (project)
    {
        for (FilesList::const_iterator it = project->GetFilesList().begin();
                                       it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (!pf)
                continue;

            if ( IsSuffixOfPath(NameUnderCursor, pf->file.GetFullPath()) )
                foundSet.Add(pf->file.GetFullPath());
        }
    }

    // Remove duplicates
    for (int i = 0; i < (int)foundSet.Count() - 1; i++)
    {
        for (int j = i + 1; j < (int)foundSet.Count(); )
        {
            if (foundSet.Item(i) == foundSet.Item(j))
                foundSet.RemoveAt(j);
            else
                j++;
        }
    }

    wxString selectedFile;
    if (foundSet.GetCount() > 1)
    {    // more than 1 hit : let the user choose
        SelectIncludeFile Dialog(Manager::Get()->GetAppWindow());
        Dialog.AddListEntries(foundSet);
        PlaceWindow(&Dialog);
        if (Dialog.ShowModal() == wxID_OK)
            selectedFile = Dialog.GetIncludeFile();
        else
            return; // user cancelled the dialog...
    }
    else if (foundSet.GetCount() == 1)
        selectedFile = foundSet[0];

    if (!selectedFile.IsEmpty())
    {
        EditorManager* edMan = Manager::Get()->GetEditorManager();
        edMan->Open(selectedFile);
        return;
    }

    cbMessageBox(wxString::Format(_("Not found: %s"), NameUnderCursor.c_str()), _("Warning"), wxICON_WARNING);
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnCurrentProjectReparse(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    /// Don't do Skip(). All connects and binds are about to be re-established
    // and this event will become invalid. Let wxWidgets delete it.
    //causes crash ==> event.Skip();

    // Invoked from menu items "Reparse active project" and Symbols context menu "Re-parse now"
    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (pProject)
    {
        // Send a quit instruction
        ShutdownLSPclient(pProject);
        // Close and create a new parser
        GetParseManager()->ReparseCurrentProject();
        // Then create a new ProcessLanguageClient
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        // LSP_DidOpen() any active file in an editor belong to this project
        if (pParser)
        {
            // The new parser has already queued files to be parsed.
            // Freeze parsing for this parser and create a client.
            // The response to LSP initialization will unfreeze the parser.
            pParser->PauseParsingForReason("AwaitClientInitialization", true);
            ProcessLanguageClient* pClient = CreateNewLanguageServiceProcess(pProject);
            if (not pClient)
            {
                // stop the batch parse timer and clear the Batch parsing queue
                pParser->ClearBatchParse();
                wxString msg = wxString::Format("%s failed to create an LSP client", __PRETTY_FUNCTION__);
                cbMessageBox(msg, "Error");
                return;
            }

            // Issue idle event to do DidOpen()s for this parser.
            // It will await client initialization, then do client DidOpen()s for
            // this new parser/client process before allowing parsing to proceed.
            //  Here's the re-schedule call for the Idle time Callback queue //(ph 2021/09/27)
            pParser->GetIdleCallbackHandler()->QueueCallback(pParser, &Parser::LSP_OnClientInitialized, pProject);
        }//endif parser
    }//endif project
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnReparseSelectedProject(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
     event.Skip();

    switch (1)
    {   //(ph 2021/02/12)
        // Shutdown the current LSP client/server and start another one.
        default:

        wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetUI().GetTree();
        if (!tree) break;

        wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetUI().GetTreeSelection();
        if (!treeItem.IsOk()) break;

        const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
        if (!data) break;

        if (data->GetKind() == FileTreeData::ftdkProject)
        {
            cbProject* project = data->GetProject();
            if (project)
            {
                // Send a quit instruction
                ShutdownLSPclient(project);
                // Close and create a new parser
                GetParseManager()->ReparseSelectedProject();
                // Then create a new ProcessLanguageClient
                Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(project);
                // LSP_DidOpen() any active file in an editor belong to this project
                if (pParser)
                {
                    // The new parser has already queued files to be parsed.
                    // Freeze parsing for this parser and create a client.
                    // The response to LSP initialization will unfreeze the parser.
                    pParser->PauseParsingForReason("AwaitClientInitialization", true);
                    ProcessLanguageClient* pClient = CreateNewLanguageServiceProcess(project);
                    if (not pClient)
                    {
                        // stop the batch parse timer and clear the Batch parsing queue
                        pParser->ClearBatchParse();
                        wxString msg = wxString::Format("%s failed to create an LSP client", __PRETTY_FUNCTION__);
                        cbMessageBox(msg, "Error");
                        return;
                    }

                    // Issue idle event to do DidOpen()s for this parser.
                    // It will await client initialization, then do client DidOpen()s for
                    // this new parser/client process before allowing parsing to proceed.
                    //  Here's the re-schedule call for the Idle time Callback queue //(ph 2021/09/27)
                    pParser->GetIdleCallbackHandler()->QueueCallback(pParser, &Parser::LSP_OnClientInitialized, project);
                }
            }
        }
    }//end switch(1)

    return;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnSelectedPauseParsing(wxCommandEvent& event) //(ph 2020/11/22)
// ----------------------------------------------------------------------------
{
    // if Ctrl-Shift keys are down, toggle ccLogger external logging on/off
    if (wxGetKeyState(WXK_ALT) && wxGetKeyState(WXK_SHIFT))
    {
        bool logStat = CCLogger::Get()->GetExternalLogStatus();
        logStat = (not logStat);
        CCLogger::Get()->SetExternalLog(logStat);
        wxString infoTitle = wxString::Format("External CCLogging is %s", logStat?"ON":"OFF");
        wxString infoText = wxString::Format("External CCLogging now %s", logStat?"ON":"OFF");
        InfoWindow::Display(infoTitle, infoText, 6000);

        return;
    }
    //Toggle pause LSP parsing on or off for selected project
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetUI().GetTree();
    if (!tree) return;

    wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetUI().GetTreeSelection();
    if (!treeItem.IsOk()) return;

    const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
    if (!data) return;

    if (data->GetKind() == FileTreeData::ftdkProject)
    {
        cbProject* pProject = data->GetProject();
        if (not pProject) return;

        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        if (pParser) //active parser
        {
            wxString projectTitle = pProject->GetTitle();
            bool paused = pParser->GetUserParsingPaused();
            paused = (not paused);
            pParser->SetUserParsingPaused(paused);
            wxString infoTitle = wxString::Format("Parsing is %s", paused?"PAUSED":"ACTIVE");
            wxString infoText = wxString::Format("%s parsing now %s", projectTitle, paused?"PAUSED":"ACTIVE");
            InfoWindow::Display(infoTitle, infoText, 6000);
            //CCLogger::Get()->->Log(infoLSP); done by infowindow.cpp:297
        }
    }
}//end OnSelectedPauseParsing
// ----------------------------------------------------------------------------
void CodeCompletion::OnSelectedFileReparse(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();
    return OnLSP_SelectedFileReparse(event); //(ph 2021/05/13)

}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_SelectedFileReparse(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetUI().GetTree();
    if (!tree)
        return;

    wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetUI().GetTreeSelection();
    if (!treeItem.IsOk())
        return;

    const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
    if (!data)
        return;

    if (data->GetKind() == FileTreeData::ftdkFile)
    {
        cbProject* project = data->GetProject();
        ProjectFile* pf = data->GetProjectFile();

        if (project and pf)
        {
            ProcessLanguageClient* pClient = GetLSPclient(project);
            if (not pClient) return;
            // if file is open in editor, send a didSave() causing a clangd reparse
            // if file is not open in editor do a didOpen()/didClose() sequence
            //      to cause a background parse.
            EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
            wxString filename = pf->file.GetFullPath();
            cbEditor* pEditor = pEdMgr->GetBuiltinEditor(filename);
            if (pEditor) pClient->LSP_DidSave(pEditor);
            else {
                // do a background didOpen(). It will be didClose()ed in OnLSP_RequestedSymbolsResponse();
                // If its a header file, OnLSP_DiagnosticsResponse() will do the LSP idClose().
                // We don't ask for symbols on headers because they incorrectly clobbler the TokenTree .cpp symbols.
                pClient->LSP_DidOpen(filename, project);
            }
        }
    }

    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorFileReparse(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    return OnLSP_EditorFileReparse(event); //(ph 2021/05/13)
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_EditorFileReparse(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not pEditor) return;
    wxFileName fnFilename = pEditor->GetFilename();

    if (fnFilename.Exists())
    {
        ProjectFile* pf = pEditor->GetProjectFile();
        cbProject* pProject = pf ? pf->GetParentProject() : nullptr;
        // FIXME (ph#): if not project or pf, Tell user file does not belong to a project
        if (pProject and pf)
        {
            ProcessLanguageClient* pClient = GetLSPclient(pProject);
            if (not pClient)
            {
                wxString msg = "The project needs to be parsed first.";
                cbMessageBox(msg, "Error");
                return;
            }
            // if file is open in editor, send a didSave() causing a clangd reparse
            // if file is not open in editor do a didOpen()/didClose() sequence
            //      to cause a background parse.
            wxString filename = pf->file.GetFullPath();
            if (pEditor) pClient->LSP_DidSave(pEditor);
            else {
                // do a background didOpen(). It will be didClose()ed in OnLSP_RequestedSymbolsResponse();
                // If its a header file, OnLSP_DiagnosticsResponse() will do the LSP idClose().
                // We don't ask for symbols on headers because they incorrectly clobbler the TokenTree .cpp symbols.
                pClient->LSP_DidOpen(filename, pProject);
            }
        }//endif
    }//end if exists
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnAppDoneStartup(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (!m_InitDone)
        DoParseOpenedProjectAndActiveEditor();

    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnWorkspaceChanged(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_WORKSPACE_CHANGED is a powerful event, it's sent after any project
    // has finished loading or closing. It's the *LAST* event to be sent when
    // the workspace has been changed. So it's the ideal time to parse files
    // and update your widgets.
    if (IsAttached() && m_InitDone)
    {
        cbProject* pActiveProject = Manager::Get()->GetProjectManager()->GetActiveProject();
        // if we receive a workspace changed event, but the project is NULL, this means two condition
        // could happen.
        // (1) the user try to close the application, so we don't need to update the UI here.
        // (2) the user just open a new project after cb started up
        if (pActiveProject)
        {
            if (!GetParseManager()->GetParserByProject(pActiveProject))
                GetParseManager()->CreateParser(pActiveProject);

            // Update the Function toolbar
            TRACE(_T("CodeCompletion::OnWorkspaceChanged: Starting m_TimerToolbar."));
            m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);

            // Update the class browser
            if (GetParseManager()->GetParser().ClassBrowserOptions().displayFilter == bdfProject)
                GetParseManager()->UpdateClassBrowser();

            // ----------------------------------------------------------------------------
            // create LSP process for any editor that may have been missed during project loading
            // ----------------------------------------------------------------------------
            EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
            for (int ii=0; ii< pEdMgr->GetEditorsCount(); ++ii)
            {
                cbEditor* pcbEd = pEdMgr->GetBuiltinEditor(ii);
                if (pcbEd)
                {
                    // don't re-open an already open editor
                    // An opened editor will have, at least, a didOpen request id
                    ProcessLanguageClient* pClient = GetLSPclient(pcbEd);
                    if (pClient) continue; //file already processed

                    //-wxString filename = pcbEd->GetFilename();
                    // Find the ProjectFile and project containing this editors file.
                    ProjectFile* pProjectFile = pcbEd->GetProjectFile();
                    if (not pProjectFile) continue;
                    cbProject* pEdProject = pProjectFile->GetParentProject();
                    // For LSP, file must belong to a project, because LSP needs target compile parameters.
                    if (not pEdProject) continue;
                    if (pEdProject != pActiveProject) continue;
                    Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pActiveProject);
                    if (not pParser) continue;
                    if (pParser->GetLSPClient()) continue;
                    // creating the client/server, will initialize it and issue LSP didOpen for its open project files.
                    CreateNewLanguageServiceProcess(pActiveProject);
                }//endif pcbEd
            }//endfor editor count
        }//endif project

    }//endif attached

    event.Skip();
}//end onWorkspaceChanged
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectOpened(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

}
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // OnEditorOpen can occur before this project activaate event


    // The Class browser shouldn't be updated if we're in the middle of loading/closing
    // a project/workspace, because the class browser would need to be updated again.
    // So we need to update it with the EVT_WORKSPACE_CHANGED event, which gets
    // triggered after everything's finished loading/closing.

    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        cbProject* project = event.GetProject();
        if (project && !GetParseManager()->GetParserByProject(project) && project->GetFilesCount() > 0)
            GetParseManager()->CreateParser(project); //Reads options and connects events to new parser

        if (GetParseManager()->GetParser().ClassBrowserOptions().displayFilter == bdfProject)
            GetParseManager()->UpdateClassBrowser();
    }
        // when debugging, the cwd may be the executable path, not project path //(ph 2020/11/9)
        cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
        wxString projDir =  pProject->GetBasePath();
        if (wxGetCwd().Lower() != projDir.Lower())
            wxSetWorkingDirectory(projDir);

    m_NeedsBatchColour = true;
    // ----------------------------------------------------------------
    // LSP
    // ----------------------------------------------------------------
    // OnProjectOpened may have already started a LSP client/server for the project. //(ph 2021/03/9)
    // Do not use IsBusy(). IsLoading() is true even though CB is activating the last loaded project.
    ProjectManager* pPrjMgr = Manager::Get()->GetProjectManager();
    if (IsAttached() && m_InitDone & (not pPrjMgr->IsClosingWorkspace()) )
    {
        cbProject* pProject = event.GetProject();
        if ( (not GetLSPclient(pProject)) //if no project yet
            and GetParseManager()->GetParserByProject(pProject) ) // but have Parser//(ph 2021/05/8)
            CreateNewLanguageServiceProcess(pProject);
    }
    event.Skip(); //<-- not necessary for CB
}
// ----------------------------------------------------------------------------
bool CodeCompletion::DoLockClangd_CacheAccess(cbProject* pcbProject, bool release)      //(ph 2021/02/3)
// ----------------------------------------------------------------------------
{
    // Multiple processes must not write to the Clangd cashe else crashes happen
    // from bad cache indexes or asserts.

    // Create a cache lock file containing the pid and windows label of the first
    // process to use the Clangd-cache file.
    // On subsequent attempts to use the cache, verify that it's the same process that
    // first opened the Clangd-cache.

    // definition of LockFile line contents
    const int LOCK_FILE_PID = 0;
    const int LOCK_FILE_EXE = 1;
    const int LOCK_FILE_CBP = 2;

    bool success = false;
    if (not pcbProject) return success = false;

    wxFileName fnCBPfile = pcbProject->GetFilename();
    wxString cbpDirectory = fnCBPfile.GetPath();

    // if no .cache dir, create one
    wxString Clangd_cacheDir = cbpDirectory + "/.cache";
    success = wxDirExists(Clangd_cacheDir);
    if (not success )
        success = wxFileName::Mkdir(Clangd_cacheDir);
    if (not success) return success = false;

    wxString lockFilename = Clangd_cacheDir + "/Clangd-cache.lock";
    if (platform::windows) lockFilename.Replace("/", "\\");
    // Get this process PID

    // if release the lock file     //(ph 2021/05/11)
    if (release)
    {
        wxRemoveFile(lockFilename);
        return true;
    }

    long pid = wxGetProcessId();
    wxString pidStr = std::to_string(pid);
    // Get this process exec path
    wxString newExePath =  ProcUtils::GetProcessNameByPid(pid);

    wxTextFile lockFile(lockFilename);
    if (not wxFileExists(lockFilename) )
    {
        lockFile.Create();
        lockFile.AddLine(pidStr);
        lockFile.AddLine(newExePath);
        lockFile.AddLine(fnCBPfile.GetFullPath());
        lockFile.Write();
        lockFile.Close();
        return success = true;
    }
    // lock file already exists. Check if it's ours or another process owns it
    bool opened = lockFile.Open();
    if (not opened) return success = false; //lock file in use

    if (lockFile.GetLine(LOCK_FILE_PID) == pidStr)
    {    // this process pid owns the cache
        lockFile.Close();
        return success = true;
    }
    //lockFile owning pid Not our pid, is the lockFile owning pid still running ?
    long lockPid = std::stol(lockFile.GetLine(0).ToStdString());
    wxString pidProcessName = ProcUtils::GetProcessNameByPid(lockPid);
    // if pidProcessName not empty, lockPid is running
    if (pidProcessName.Length()) switch(1)
    {
        default:
        // The lockFile pid is running but is it a reused pid? (not codeblocks)
        // if the running pid process name is different from lockFile pid process name
        // it's a reused pid. Probably from a system reboot.
        wxFileName fnLockFileExeName = lockFile.GetLine(LOCK_FILE_EXE);
        wxFileName fnRunningExeName  = pidProcessName;
            //-debugging- wxString lockExe = fnLockFileExeName.GetFullName().Lower();
            //-debugging- wxString runExe = fnRunningExeName.GetFullName().Lower();
        if (fnRunningExeName.GetFullName().Lower() == fnLockFileExeName.GetFullName().Lower())
        {
            // A running pid is using the old stale lockFile pid, and it's still running codeblocks
            // See if the new project file is the same as the old lockFile project file.
            bool sameProj = pcbProject->GetFilename().Lower() == lockFile.GetLine(LOCK_FILE_CBP).Lower();
            if (not sameProj) break;

            // Summary:
            // These's already a running process that used to own the cache and it's running codeblocks.
            // And THIS process is trying to access the same cache that the running process used to own.
            // Most likely, THAT running process is a rebooted codeblocks that's using the cache.
            // If it walks, quacks, and looks like a duck ...
            wxString deniedMsg = wxString::Format(
                               "Process: %s Pid(%d)\nis denied access to:\n%s"
                               "\nbeing used by\n"
                               "process: %s Pid(%d)",
                                newExePath, pid,
                                Clangd_cacheDir,
                                pidProcessName, lockPid
                               );
            deniedMsg += "\n\nTo debug without this problem, debug a copy of the project directory.";
            deniedMsg += "\nBut do not copy the .cache directory.";
            cbMessageBox(deniedMsg);
            lockFile.Close();
            return success = false;
        }//endif execs match
    }//endif switch

    // old owning pid no longer exists; Set this process as new owning pid
    lockFile[LOCK_FILE_PID] = pidStr;
    lockFile[LOCK_FILE_EXE] = newExePath;
    lockFile[LOCK_FILE_CBP] = fnCBPfile.GetFullPath();
    lockFile.Write();
    lockFile.Close();
    return success = true;

    return success = false;
}
// ----------------------------------------------------------------------------
bool CodeCompletion::DoUnlockClangd_CacheAccess(cbProject* pcbProject)      //(ph 2021/02/3)
// ----------------------------------------------------------------------------
{
    // Multiple processes must not write to the Clangd cashe else crashes happen
    // from bad cache indexes or asserts.

   // Call this function to remove the cache Access lock

    bool success = false;
    if (not pcbProject) return success = false;

    wxFileName fnCBPfile = pcbProject->GetFilename();
    wxString cbpDirectory = fnCBPfile.GetPath();

    // if no .Clangd-cache dir, create one , just return
    wxString Clangd_cacheDir = cbpDirectory + "/.cache";
    success = wxDirExists(Clangd_cacheDir);
    if (not success) return success = false;

    wxString lockFilename = Clangd_cacheDir + "/Clangd-cache.lock";
    if (platform::windows) lockFilename.Replace("/", "\\");
    success = wxRemoveFile(lockFilename);
    return success;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_ProcessTerminated(wxCommandEvent& event)     //(ph 2021/06/28)
// ----------------------------------------------------------------------------
{
    cbProject* pProject = (cbProject*)event.GetEventObject();
    cbAssertNonFatal(pProject && "Entered with null project ptr.")
    if (not pProject) return;
    m_LSP_Clients[pProject] = nullptr;
    return;
}
// ----------------------------------------------------------------------------
ProcessLanguageClient* CodeCompletion::CreateNewLanguageServiceProcess(cbProject* pcbProject)                                     //(ph 2020/11/4)
// ----------------------------------------------------------------------------
{
    cbAssertNonFatal(pcbProject && "CreateNewLanguageServiceProcess requires a project");
    if (not pcbProject) return nullptr;

    // Don't allow a second process to write to the current clangd symbol caches
    if (not DoLockClangd_CacheAccess(pcbProject) ) return nullptr;

    ProcessLanguageClient* pLSPclient = nullptr;
    if (m_LSP_Clients.count(pcbProject) and GetLSPclient(pcbProject))
        pLSPclient = m_LSP_Clients[pcbProject];
    else
    {
       pLSPclient = new ProcessLanguageClient(pcbProject); //(ph 2021/11/18)
        CCLogger* pLogMgr = CCLogger::Get();
        if (pLSPclient and  pLSPclient->GetLSP_Server_PID() )
            pLogMgr->DebugLog("LSP: Started new LSP client/server for "
                              + pcbProject->GetFilename() + " @("
                              + pLSPclient->LSP_GetTimeHMSM() + ")"
                             );
    }
    if ( (not pLSPclient) or (not pLSPclient->GetLSP_Server_PID()) )
    {
        if (pLSPclient)
            delete pLSPclient;
        pLSPclient = nullptr;
        DoUnlockClangd_CacheAccess(pcbProject);
    }
    else
    {
        m_LSP_Clients[pcbProject] = pLSPclient;
        pLSPclient->SetCBProject(pcbProject);
        pLSPclient->SetLSP_UserEventID(LSPeventID);
        Bind(wxEVT_COMMAND_MENU_SELECTED, &CodeCompletion::OnLSP_Event, this, LSPeventID);
        wxFileName cbpName(pcbProject->GetFilename());
        wxString rootURI = cbpName.GetPath();
        //-rootURI.Replace("F:", "");
        ParserBase* pParser = GetParseManager()->GetParserByProject(pcbProject);
        if (not pParser)
        {
            wxString msg("CreateNewLanguageServiceProcess() CC pParser is null.");
            cbMessageBox(msg);
        }
        if (pParser)
            pParser->SetLSP_Client(pLSPclient);

        pLSPclient->LSP_Initialize(pcbProject);
    }

    return pLSPclient;
}
// ----------------------------------------------------------------------------
wxString CodeCompletion::GetLineTextFromFile(const wxString& file, const int lineNum) //(ph 2020/10/26)
// ----------------------------------------------------------------------------
{
    // Fetch a single line from a text file

    EditorManager* edMan = Manager::Get()->GetEditorManager();

    wxWindow* parent = edMan->GetBuiltinActiveEditor()->GetParent();
    cbStyledTextCtrl* control = new cbStyledTextCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(0, 0));
    control->Show(false);

    wxString resultText;
   switch(1) //once only
    {
        default:

        // check if the file is already opened in built-in editor and do search in it
        cbEditor* ed = edMan->IsBuiltinOpen(file);
        if (ed)
            control->SetText(ed->GetControl()->GetText());
        else // else load the file in the control
        {
            EncodingDetector detector(file, false);
            if (not detector.IsOK())
            {
                wxString msg(wxString::Format("%s():%d failed EncodingDetector for %s", __FUNCTION__, __LINE__, file));
                CCLogger::Get()->Log(msg);
                delete control;
                return wxString();
            }
            control->SetText(detector.GetWxStr());
        }

            resultText = control->GetLine(lineNum).Trim(true).Trim(false);
            break;
    }

    delete control; // done with it

    return resultText;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_Event(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{

    if (Manager::IsAppShuttingDown())
        return;

    wxString evtString = event.GetString();
    ProcessLanguageClient* pClient = (ProcessLanguageClient*)event.GetEventObject();
    cbProject* pProject = nullptr;
    if (pClient)
    {
        LSPClientsMapType::iterator it = m_LSP_Clients.begin();
        while (it != m_LSP_Clients.end())
        {
            cbProject* pMapProject = it->first;
            ProcessLanguageClient* pMapClient = it->second;
            if (pMapClient == pClient)
            {
                pProject = pMapProject;
                break;
            }
            it++;
        }
    }
    if ( not (pClient and pProject) )
    {
        wxString msg;
        if (not pClient)
            msg = "OnLSP_Event without a client ptr" ;
        else if (not pProject)
            msg = "OnLSP_Event without a Project ptr";
        cbMessageBox(msg, "Event error");
    }
    // ----------------------------------------------------------------------------
    ///Take ownership of event client data pointer to assure it's freed
    // ----------------------------------------------------------------------------
    std::unique_ptr<json> pJson;
    pJson.reset( (json*)event.GetClientData() );

    // create an event id from the the LSP event string, eg.,"textDocument/definition"STX"RRID####"
    // Extract the RequestResponseID from the event string
    // *unused for now* int lspID = XRCID(evtString.BeforeFirst(STX)); //LSP request type
    long lspRRID = 0;
    int posn = wxNOT_FOUND;
    if ( wxFound(posn = evtString.Find(wxString(STX) + "RRID")) )
    {
        wxString RRIDstr = evtString.Mid(posn+1);
        RRIDstr = RRIDstr.BeforeFirst(STX); //eliminate any other trailing strings
        RRIDstr = RRIDstr.Mid(4);           //skip over 'RRID" to char number
        bool ok = RRIDstr.ToLong(&lspRRID);
        if (not ok) lspRRID = 0;
    }
    // ----------------------------------------------------------------------------
    // Check for LSP Error message
    // ----------------------------------------------------------------------------
    if (evtString.EndsWith(wxString(STX) + "error"))
    {
        if (pJson->contains("error"))
        {
            wxString errorMsg = pJson->at("error").at("message").get<std::string>();
            if (errorMsg == "drop older completion request")
                return;
            if (errorMsg == "Request cancelled because the document was modified")
                return;
            cbMessageBox("LSP: " + errorMsg);
        }

        // clear any call back for this request/response id
        if (lspRRID)
        {
            GetLSPEventSinkHandler()->ClearLSPEventCallback(lspRRID);
            return;
        }
    }

    // ----------------------------------------------------------------------------
    // Invoke any queued call backs first; if none, fall into default processing
    // ----------------------------------------------------------------------------
    if (GetLSPEventSinkHandler()->Count() and lspRRID)
        GetLSPEventSinkHandler()->OnLSPEventCallback(lspRRID, event);

    // ----------------------------------------------------
    // LSP client/server Initialized
    // ----------------------------------------------------
    if ( evtString.StartsWith("LSP_Initialized:true"))
    {

        // ----------------------------------------------------------------------
        // capture the semanticTokens legends returned from LSP initialization response
        //https://microsoft.github.io/language-server-protocol/specification#textDocument_semanticTokens
        // ----------------------------------------------------------------------
        try{
            if (pJson->at("result")["capabilities"].contains("semanticTokensProvider") )
            {
                json legend = pJson->at("result")["capabilities"]["semanticTokensProvider"]["legend"];
                m_SemanticTokensTypes = legend["tokenTypes"].get<std::vector<std::string>>();
                m_SemanticTokensModifiers = legend["tokenModifiers"].get<std::vector<std::string>>();
            }//endif
        }catch (std::exception &e)
        {
            wxString msg = wxString::Format("%s() %s", __PRETTY_FUNCTION__, e.what());
            cbMessageBox(msg, "LSP initialization");
            return;
        }

        return;
    }//endif LSP_Initialized

    // ----------------------------------------------------------------------------
    // textDocument/declaration textDocument/definition event
    // ----------------------------------------------------------------------------
    // LSP Result of FindDeclaration or FindImplementation
    // a CB "find declaration"   == Clangd/Clangd "declaration/signature"
    // a CB "find implementation == Clangd/Clangd "definition"
    // this event.string contains type of eventType:result or error
    // example:
    // {"jsonrpc":"2.0","id":"textDocument/definition","result":[{"uri":"file:///F%3A/usr/Proj/HelloWxWorld/HelloWxWorldMain.cpp","range":{"start":{"line":89,"character":24},"end":{"line":89,"character":30}}}]}

    bool isDecl = false; bool isImpl = false;
    if (evtString.StartsWith("textDocument/declaration") )
        isDecl = true;
    else if (evtString.StartsWith("textDocument/definition") )
        isImpl = true;

    if ( isImpl or isDecl)
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        return pParser->OnLSP_DeclDefResponse(event); //default processing
    }

    // ----------------------------------------------------
    // textDocument references event
    // ----------------------------------------------------
    if (evtString.StartsWith("textDocument/references") )
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_ReferencesResponse(event);
        return;
    }
    // ----------------------------------------------------------------------------
    // textDocument/DocumentSymbol event
    // ----------------------------------------------------------------------------
    else if (evtString.StartsWith("textDocument/documentSymbol") )
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_RequestedSymbolsResponse(event);

    }//end textDocument/documentSymbol
    // ----------------------------------------------------------------------------
    // textDocument/publishDiagnostics method
    // ----------------------------------------------------------------------------
    else if (evtString.StartsWith("textDocument/publishDiagnostics") )
    {
        // There might be two for each file: an empty one for DidClose() and a good one for DidOpen()
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_DiagnosticsResponse(event);

    }//endiftextDocument/publishDiagnostics
    // ----------------------------------------------------------------------------
    // Completion event
    // ----------------------------------------------------------------------------
    else if ( evtString.StartsWith("textDocument/completion"))
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_CompletionResponse(event, m_CompletionTokens);
    }
    // ----------------------------------------------------------------------------
    // Hover event
    // ----------------------------------------------------------------------------
    else if ( evtString.StartsWith("textDocument/hover"))
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_HoverResponse(event, m_HoverTokens, m_HoverLastPosition);
    }
    // ----------------------------------------------------------------------------
    // SignatureHelp event
    // ----------------------------------------------------------------------------
    else if ( evtString.StartsWith("textDocument/signatureHelp"))
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_SignatureHelpResponse(event, m_SignatureTokens, m_HoverLastPosition);
    }
    // ----------------------------------------------------------------------------
    // "textDocument/rename" event
    // ----------------------------------------------------------------------------
    else if ( evtString.StartsWith("textDocument/rename"))
    {
        Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
        pParser->OnLSP_RenameResponse(event);
    }
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_SemanticTokenResponse(wxCommandEvent& event)  //(ph 2021/03/16)
// ----------------------------------------------------------------------------
{
    // This is a callback after requesting textDocument/semanticTokens/full
    // It's not being used for now, but works and will be useful in the future
    // ----------------------------------------------------------------------------
    ///  GetClientData() contains ptr to json object
    ///  DONT free it, The return to OnLSP_Event() will free it as a unique_ptr
    // ----------------------------------------------------------------------------

    json* pJson = (json*)event.GetClientData();
    wxString idStr = event.GetString();

    // event string looks like: "textDocument/semanticTokens/full\002file:///f:/somefile.xxx\0002id:method"
    // \00002 == utf-8 STX (start of text)
    // the "\00002id:method"  may be missing

    wxString URI = idStr.AfterFirst(STX);
    if (URI.Contains(STX))
        URI = URI.BeforeFirst(STX); //filename
    wxString uriFilename = URI.Mid(8); // jump over file:/// prefix
    wxURI uriFile(uriFilename);
    uriFilename = uriFile.BuildUnescapedURI();
    if (platform::windows)
        uriFilename.Replace("/","\\");

    cbEditor* pEditor =  nullptr;
    EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
    EditorBase* pEdBase = pEdMgr->IsOpen(uriFilename);
    pEditor = pEdMgr->GetBuiltinEditor(pEdBase);
    if (not pEditor) return;

    ProjectFile* pProjectFile = pEditor->GetProjectFile();
    cbProject* pProject = nullptr;
    if (pProjectFile) pProject = pProjectFile->GetParentProject();
    if ( (not pProjectFile) or (not pProject) ) return;
    ParserBase* pParser = GetParseManager()->GetParserByProject(pProject);
    if (not pParser) return;

    if (pParser->m_SemanticTokensTypes.size() == 0)             //(ph 2021/03/22)
    {
        pParser->m_SemanticTokensTypes = this->m_SemanticTokensTypes;
        pParser->m_SemanticTokensModifiers = this->m_SemanticTokensModifiers;
    }
    //-testing- size_t cnt = pParser->m_SemanticTokensTypes.size();

    // Issue event to anyone looking for semant tokens
    wxCommandEvent symEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("textDocument/semanticTokens/full"));
    symEvent.SetString(uriFilename);
    symEvent.SetClientData(pJson);
    Manager::Get()->GetAppFrame()->GetEventHandler()->ProcessEvent(symEvent);

    return;
}//end OnLSP_SemanticTokenResponse
// ----------------------------------------------------------------------------
void CodeCompletion::ShutdownLSPclient(cbProject* pProject)
// ----------------------------------------------------------------------------
{   //(ph 2021/02/12)
    if (IsAttached() && m_InitDone)
    {
        CCLogger* pLogMgr = CCLogger::Get();

        // ------------------------------------------------------------
        //  LSP client/server
        // ------------------------------------------------------------
        ProcessLanguageClient* pClient = GetLSPclient(pProject);
        if (pClient)
        {
            // Stop all parsing for this parser
            Parser* pParser = (Parser*)GetParseManager()->GetParserByProject(pProject);
            if (pParser)
                pParser->PauseParsingForReason("ShutDown", true);

            // If project is the current active project do a didClose for its files.
            // Tell LSP server to didClose() all open files for this project, then delete server for this project
            // If project is not active, LSP server needs to do nothing.

            // Get current time
            int startMillis = pClient->GetDurationMilliSeconds(0);

            // Tell LSP we closed all open files for this project
            EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
            for (int ii=0; ii< pEdMgr->GetEditorsCount(); ++ii)
            {
                cbEditor* pcbEd = pEdMgr->GetBuiltinEditor(ii);
                if (not pcbEd) continue; //happens because of "Start here" tab
                ProjectFile* pPrjFile = pcbEd->GetProjectFile();
                if (not pPrjFile) continue;
                if (pPrjFile->GetParentProject() == pProject)
                    GetLSPclient(pProject)->LSP_DidClose(pcbEd);
            }
            long closing_pid = pClient->GetLSP_Server_PID();

            // Tell LSP server to quit
            pClient->LSP_Shutdown();
            m_LSP_Clients.erase(pProject); // erase first or crash
            delete pClient;
            pClient = nullptr;

            // The clangd process is probably already terminated by LSP_Shutdown above.
            while(ProcUtils::GetProcessNameByPid(closing_pid).Length())
            {
                Manager::Yield(); //give time for process to shutdown
                wxMilliSleep(50);
            }

            // The event project just got deleted, see if there's another project we can use
            // to get client info.
            cbProject* pActiveProject =  Manager::Get()->GetProjectManager()->GetActiveProject();
            if (pActiveProject && GetLSPclient(pActiveProject) )
                pLogMgr->DebugLog(wxString::Format("LSP OnProjectClosed duration:%d millisecs. ", GetLSPclient(pActiveProject)->GetDurationMilliSeconds(startMillis)) );

        }//endif m_pLSPclient
    }
}//end ShutdownLSPclient()
// ----------------------------------------------------------------------------
void CodeCompletion::CleanUpLSPLogs()
// ----------------------------------------------------------------------------
{ //(ph 2021/02/15)

    // MS Windows has a lock on closed clangd logs.
    // Windows will not allow deletion of clangd logs until the project closes.
    // Having to do it here is stupid, but works.

    // Delete any logs not listed in %temp%/CBclangd_LogsIndex.txt
    // eg., CBclangd_client-10412.log or CBclangd_server-6224.log
    // The index file contains log names for recent log files we want to keep for awhile.
    // They are usually the last active log files and will be removed from the index when
    // new logs are created.
    wxString logIndexFilename(wxFileName::GetTempDir() + wxFILE_SEP_PATH + "CBclangd_LogsIndex.txt");
    if ( wxFileExists(logIndexFilename))
    switch (1)
    {
        default:
        // The log index file contains entries of log filenames to keep.
        // The first item is the PID of an active log. For example:
        // 21320;02/14/21_12:49:58;F:\user\programs\msys64\mingw64\bin\clangd.exe;F:\usr\Proj\HelloWxWorld\HelloWxWorld.cbp
        wxLogNull noLog;
        wxTextFile logIndexFile(logIndexFilename);
        logIndexFile.Open();
        if (not logIndexFile.IsOpened()) break;
        if (not logIndexFile.GetLineCount() ) break;
        size_t logIndexLineCount = logIndexFile.GetLineCount();

        wxString tempDirName = wxFileName::GetTempDir();
        wxArrayString logFilesVec;

        wxString logFilename = wxFindFirstFile(tempDirName + wxFILE_SEP_PATH + "CBclangd_*-*.log", wxFILE);
        while(logFilename.Length())
        {
            logFilesVec.Add(logFilename);
            logFilename = wxFindNextFile();
            if (logFilename.empty()) break;
        }
        if (not logFilesVec.GetCount()) break;
        // Delete log files with PIDs not in the log index
        for (size_t ii=0; ii<logFilesVec.GetCount(); ++ii)
        {
            wxString logName = logFilesVec[ii];
            wxString logPID = logName.AfterFirst('-').BeforeFirst('.');
            // if log filename pid == an index file pid, it lives
            for (size_t jj=0; jj<logIndexLineCount; ++jj)
            {
                // pid string is the first item in the log index
                wxString ndxPID = logIndexFile.GetLine(jj).BeforeFirst(';');
                if (logPID == ndxPID) break;   //This log lives, get next log.
                if (jj == logIndexLineCount-1) //no match for pids, delete this log
                    wxRemoveFile(logName);
            }//endfor index filenames
        }//endfor log Filenames
        if (logIndexFile.IsOpened())
            logIndexFile.Close();
    }//end switch
}//end CleanUpLSPLogs()
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // After this, the Class Browser needs to be updated. It will happen
    // when we receive the next EVT_PROJECT_ACTIVATED event.
    if (IsAttached() && m_InitDone)
    {
        cbProject* project = event.GetProject();

        // ------------------------------------------------------------
        //  LSP client/server
        // ------------------------------------------------------------
        if (GetLSPclient(project))
        {
            // Tell LSP server to didClose() all open files for this project
            // and delete LSP client/server for this project
            ShutdownLSPclient(project);
            // MS Windows had a lock on closed clangd logs until the project was closed.
            CleanUpLSPLogs();
            DoUnlockClangd_CacheAccess(project);

        }//endif m_pLSPclient


       if (project && GetParseManager()->GetParserByProject(project))
       {
            // remove the CC Parser instance associated with the project
            GetParseManager()->DeleteParser(project);
        }
    }

    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectSaved(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectFileAdded(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (( not IsAttached()) or (not m_InitDone) ) return;

    // ------------------------------------------------------------
    //  LSP client/server
    // ------------------------------------------------------------
    switch(1)
    {
        default:
        cbProject* pProject = event.GetProject();
        if (not GetLSPclient(pProject)) break;
        wxString filename = event.GetString();
        cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
        if (not pEditor) break;
        if (GetLSPclient(pProject)->GetLSP_EditorIsOpen(pEditor))
            break;

        // CodeBlocks has not yet added the ProjectFile* prior to calling this event.
        // Delay GetLSPclient(pProject)->LSP_DidOpen(pEditor) with a callback.
        // Allows ProjectManager to add ProjectFile* to the project following this
        // event but before the callback event is invoked.
         CallAfter(&CodeCompletion::OnLSP_ProjectFileAdded, pProject, filename);
    }

    GetParseManager()->AddFileToParser(event.GetProject(), event.GetString());
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnLSP_ProjectFileAdded(cbProject* pProject, wxString filename)
// ----------------------------------------------------------------------------
{
    // This is a delayed callback caused by the fact that cbEVT_PROJECT_FILE_ADDED
    // is sent before a ProjectFile* is set in either the editor or the project.
    // It's nill until after the event completes.

    if (( not IsAttached()) or (not m_InitDone) ) return;

    // ------------------------------------------------------------
    //  LSP client/server           //(ph 2021/03/5)
    // ------------------------------------------------------------
    if (not GetLSPclient(pProject)) return;
    cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
    if (not pEditor) return;
    if (GetLSPclient(pProject)->GetLSP_EditorIsOpen(pEditor))
        return;
    ProjectFile* pProjectFile = pProject->GetFileByFilename(filename, false);
    if (not pProjectFile)
        return;
    GetLSPclient(pProject)->LSP_DidOpen(pEditor);
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnProjectFileRemoved(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (IsAttached() && m_InitDone)
        GetParseManager()->RemoveFileFromParser(event.GetProject(), event.GetString());
    event.Skip();
}
// ----------------------------------------------------------------------------
//void CodeCompletion::OnProjectFileChanged(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
//{
//    This event is never issued by CodeBlocks.
//
//    if (IsAttached() && m_InitDone)
//    {
//        // TODO (Morten#5#) make sure the event.GetProject() is valid.
//        cbProject* project = event.GetProject();
//        wxString filename = event.GetString();
//        if (!project)
//            project = GetParseManager()->GetProjectByFilename(filename);
//        if (project && GetParseManager()->ReparseFile(project, filename))
//            CCLogger::Get()->DebugLog(_T("Reparsing when file changed: ") + filename);
//    }
//    event.Skip();
//}
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorSave(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone && event.GetEditor())
    {

        // ----------------------------------------------------------------------------
        // LSP didSave
        // ----------------------------------------------------------------------------
        EditorBase* pEb = event.GetEditor();
        cbEditor* pcbEd = Manager::Get()->GetEditorManager()->GetBuiltinEditor(pEb);
        // Note: headers dont get initialized bec. clangd does not send back
        // diagnostics for unchanged editors. Check if changed.
        if (GetLSP_Initialized(pcbEd) or pcbEd->GetModified())
        {
            GetLSPclient(pcbEd)->LSP_DidSave(pcbEd);
        }
        return;
    }

}//end OnEditorSave()
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorOpen(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{

    /// Note Well: the ProjectFile in the cbEditor is a nullptr

    if (!Manager::IsAppShuttingDown() && IsAttached() && m_InitDone)
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
        if (ed)
        {
            FunctionsScopePerFile* funcdata = &(m_AllFunctionsScopes[ed->GetFilename()]);
            funcdata->parsed = false;
            m_OnEditorOpenEventOccured = true;
        }//end if ed

    }//endif IsApp....

    event.Skip(); //Unnecessary to skip CodeBlockEvents.
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    TRACE(_T("CodeCompletion::OnEditorActivated(): Enter"));

    if (not ProjectManager::IsBusy() && IsAttached() && m_InitDone && event.GetEditor())
    {
        m_LastEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());

        TRACE(_T("CodeCompletion::OnEditorActivated(): Starting m_TimerEditorActivated."));
        m_TimerEditorActivated.Start(EDITOR_ACTIVATED_DELAY, wxTIMER_ONE_SHOT);

        if (m_TimerToolbar.IsRunning())
            m_TimerToolbar.Stop();

        // The LSP didOpen is issued here because OnEditorOpen() does not have the ProjectFile set.
        // Verify that it was OnEditorOpen() that activated this editor.
        cbEditor* pEd = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
        if (pEd and m_OnEditorOpenEventOccured)
        {
            m_OnEditorOpenEventOccured = false;
            // Here for Language Service Process to send didOpen(). //(ph 2020/10/3)
            // The OnEditorOpen() event cannot be used because the editor does not yet have
            // a ProjectFile pointer to determine the files project parent.

            // This OnEditorActivated() event is occuring BEFORE OnProjectActivated().
            // The LSP server may not be initialized when editors are opened during project load.
            // To compensate for any missed editor opens, LSP_Initialize() will scan the editors notebook
            // and send didOpen() for files already opened before event OnProjectActivated().

            // Here, we check for editors opened AFTER OnProjectActivated() to send a single
            // didOpen() to the LSP server. The m_OnEditorOpenEventOccured flag indicates
            // this was a valid open event, not just a user click on the notebook tab, or a
            // mouse focus activation.

            // Find the project and ProjectFile this editor is dependent on.
            // We need the project and base project .cbp file location
            // to do a didOpen() on a file belonging to a non-active project.
            cbProject* pActiveProject = Manager::Get()->GetProjectManager()->GetActiveProject();
            if (not pActiveProject)
                return;
            ProjectFile* pProjectFile = pEd->GetProjectFile();
            if (not pProjectFile) return;
            cbProject* pEdProject = pProjectFile->GetParentProject();
            if (not pEdProject) return;
            Parser* pParser = dynamic_cast<Parser*>( GetParseManager()->GetParserByProject(pEdProject));
            if (GetLSP_Initialized(pEdProject) and pParser)
            {
                // if parsing is pause, add file to background parse queue
                //-if (pParser->GetUserParsingPaused())
                if (pParser->PauseParsingCount())
                    pParser->AddFile(pEd->GetFilename(), pEdProject, true);
                else
                    GetLSPclient(pEd)->LSP_DidOpen(pEd);
            }
        }//if pEd

    }

    event.Skip();
    TRACE(_T("CodeCompletion::OnEditorActivated(): Leave"));
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    EditorManager* edm = Manager::Get()->GetEditorManager();
    if (!edm)
    {
        event.Skip();
        return;
    }

    wxString activeFile;
    EditorBase* eb = edm->GetActiveEditor();
    if (eb)
        activeFile = eb->GetFilename();

    TRACE(_T("CodeCompletion::OnEditorClosed(): Closed editor's file is %s"), activeFile.wx_str());

    // ----------------------------------------------------------------------------
    // Tell Language Service Process that an editor was closed //(ph 2020/10/3) LSP
    // ----------------------------------------------------------------------------
    // Invoke didClose for the event editor, not the active editor
    // This file may have already been didClose'd by OnProjectClose() LSP shutdown.
    cbEditor* pcbEd = edm->GetBuiltinEditor(event.GetEditor());
    ProcessLanguageClient* pClient = GetLSPclient(pcbEd);
    //-if (pcbEd and GetLSP_Initialized(pcbEd) and GetLSPclient(pcbEd) )
    //^^ NoteToSelf: editor would not show initialized if it never got diagnostics //(ph 2021/06/4)
    if (pcbEd and pClient and pClient->GetLSP_EditorIsOpen(pcbEd))
            GetLSPclient(pcbEd)->LSP_DidClose(pcbEd);

    if (m_LastEditor == event.GetEditor())
    {
        m_LastEditor = nullptr;
        if (m_TimerEditorActivated.IsRunning())
            m_TimerEditorActivated.Stop();
    }

    // tell m_ParseManager that a builtin editor was closed
    if ( edm->GetBuiltinEditor(event.GetEditor()) )
        GetParseManager()->OnEditorClosed(event.GetEditor());

    m_LastFile.Clear();

    // we need to clear CC toolbar only when we are closing last editor
    // in other situations OnEditorActivated does this job
    // If no editors were opened, or a non-buildin-editor was active, disable the CC toolbar
    if (edm->GetEditorsCount() == 0 || !edm->GetActiveEditor() || !edm->GetActiveEditor()->IsBuiltinEditor())
    {
        EnableToolbarTools(false);

        // clear toolbar when closing last editor
        if (m_Scope)
            m_Scope->Clear();
        if (m_Function)
            m_Function->Clear();

        cbEditor* ed = edm->GetBuiltinEditor(event.GetEditor());
        wxString filename;
        if (ed)
            filename = ed->GetFilename();

        m_AllFunctionsScopes[filename].m_FunctionsScope.clear();
        m_AllFunctionsScopes[filename].m_NameSpaces.clear();
        m_AllFunctionsScopes[filename].parsed = false;
        if (GetParseManager()->GetParser().ClassBrowserOptions().displayFilter == bdfFile)
            GetParseManager()->UpdateClassBrowser();
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
void CodeCompletion::OnCCLogger(CodeBlocksThreadEvent& event)
// ----------------------------------------------------------------------------
{
    if (!Manager::IsAppShuttingDown())
        Manager::Get()->GetLogManager()->Log(event.GetString());
}

// ----------------------------------------------------------------------------
void CodeCompletion::OnCCDebugLogger(CodeBlocksThreadEvent& event)
// ----------------------------------------------------------------------------
{
    if (!Manager::IsAppShuttingDown())
    {
        if (event.GetId() == g_idCCDebugLogger)
         Manager::Get()->GetLogManager()->DebugLog(event.GetString());
        if (event.GetId() == g_idCCDebugErrorLogger)
         Manager::Get()->GetLogManager()->DebugLogError(event.GetString());
    }
}
// ----------------------------------------------------------------------------
int CodeCompletion::DoClassMethodDeclImpl() //not yet used for clangd plugin
// ----------------------------------------------------------------------------
{
    // ContextMenu->Insert-> declaration/implementation

    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    if ( ft != ftHeader && ft != ftSource && ft != ftTemplateSource) // only parse source/header files
        return -4;

    if (!GetParseManager()->GetParser().Done())
    {
        wxString msg = _("The Parser is still parsing files.");
        msg += GetParseManager()->GetParser().NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        return -5;
    }

    int success = -6;

    // ----------------------------------------------------
    // CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
    // ----------------------------------------------------
    // Do not block the main UI. If the lock is busy, this code re-queues a callback on idle time.
    auto lock_result = s_TokenTreeMutex.LockTimeout(250);
    if (lock_result != wxMUTEX_NO_ERROR)
    {
        // lock failed, don't block UI thread, requeue a callback on the idle queue instead.
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idClassMethod);
        GetIdleCallbackHandler()->QueueCallback(this, &CodeCompletion::OnClassMethod, evt);
        return -5; //parser is busy
    }
    else  s_TokenTreeMutex_Owner = wxString::Format("%s %d",__PRETTY_FUNCTION__, __LINE__); /*record owner*/  \

    // open the insert class dialog
    wxString filename = ed->GetFilename();
    InsertClassMethodDlg dlg(Manager::Get()->GetAppWindow(), &GetParseManager()->GetParser(), filename);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        int pos = control->GetCurrentPos();
        int line = control->LineFromPosition(pos);
        control->GotoPos(control->PositionFromLine(line));

        wxArrayString result = dlg.GetCode();
        for (unsigned int i = 0; i < result.GetCount(); ++i)
        {
            pos = control->GetCurrentPos();
            line = control->LineFromPosition(pos);
            // get the indent string from previous line
            wxString str = ed->GetLineIndentString(line - 1) + result[i];
            MatchCodeStyle(str, control->GetEOLMode(), ed->GetLineIndentString(line - 1), control->GetUseTabs(), control->GetTabWidth());
            control->SetTargetStart(pos);
            control->SetTargetEnd(pos);
            control->ReplaceTarget(str);
            control->GotoPos(pos + str.Length());// - 3);
        }
        success = 0;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}
// ----------------------------------------------------------------------------
int CodeCompletion::DoAllMethodsImpl()
// ----------------------------------------------------------------------------
{
    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    if ( ft != ftHeader && ft != ftSource && ft != ftTemplateSource) // only parse source/header files
        return -4;

    wxArrayString paths = GetParseManager()->GetAllPathsByFilename(ed->GetFilename());
    TokenTree*    tree  = GetParseManager()->GetParser().GetTokenTree();

    // ----------------------------------------------------
    // CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
    // ----------------------------------------------------
    auto lock_result = s_TokenTreeMutex.LockTimeout(250);
    if (lock_result != wxMUTEX_NO_ERROR)
    {
        // lock failed, but don't block UI thread, requeue an idle time callback instead.
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idClassMethod);
        GetIdleCallbackHandler()->QueueCallback(this, &CodeCompletion::OnClassMethod, evt);
        return -5; //parser is busy
    }
    else  s_TokenTreeMutex_Owner = wxString::Format("%s %d",__PRETTY_FUNCTION__, __LINE__); /*record owner*/  \

    // get all filenames' indices matching our mask
    TokenFileSet result;
    for (size_t i = 0; i < paths.GetCount(); ++i)
    {
        CCLogger::Get()->DebugLog(_T("CodeCompletion::DoAllMethodsImpl(): Trying to find matches for: ") + paths[i]);
        TokenFileSet result_file;
        tree->GetFileMatches(paths[i], result_file, true, true);
        for (TokenFileSet::const_iterator it = result_file.begin(); it != result_file.end(); ++it)
            result.insert(*it);
    }

    if (result.empty())
    {
        // ------------------------------------------------
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        // ------------------------------------------------

        cbMessageBox(_("Could not find any file match in parser's database."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    // loop matching files, loop tokens in file and get list of un-implemented functions
    wxArrayString arr; // for selection (keeps strings)
    wxArrayInt arrint; // for selection (keeps indices)
    typedef std::map<int, std::pair<int, wxString> > ImplMap;
    ImplMap im;
    for (TokenFileSet::const_iterator itf = result.begin(); itf != result.end(); ++itf)
    {
        const TokenIdxSet* tokens = tree->GetTokensBelongToFile(*itf);
        if (!tokens) continue;

        // loop tokens in file
        for (TokenIdxSet::const_iterator its = tokens->begin(); its != tokens->end(); ++its)
        {
            const Token* token = tree->at(*its);
            if (   token // valid token
                && (token->m_TokenKind & (tkFunction | tkConstructor | tkDestructor)) // is method
                && token->m_ImplLine == 0 ) // is un-implemented
            {
                im[token->m_Line] = std::make_pair(*its, token->DisplayName());
            }
        }
    }

    for (ImplMap::const_iterator it = im.begin(); it != im.end(); ++it)
    {
        arrint.Add(it->second.first);
        arr.Add(it->second.second);
    }

    if (arr.empty())
    {
        // ------------------------------------------------
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        // ------------------------------------------------

        cbMessageBox(_("No classes declared or no un-implemented class methods found."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    int success = -5;

    // select tokens
    MultiSelectDlg dlg(Manager::Get()->GetAppWindow(), arr, true);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        int pos = control->GetCurrentPos();
        int line = control->LineFromPosition(pos);
        control->GotoPos(control->PositionFromLine(line));

        bool addDoxgenComment = Manager::Get()->GetConfigManager(_T("clangd_client"))->ReadBool(_T("/add_doxgen_comment"), false);

        wxArrayInt indices = dlg.GetSelectedIndices();


        for (size_t i = 0; i < indices.GetCount(); ++i)
        {
            const Token* token = tree->at(arrint[indices[i]]);
            if (!token)
                continue;

            pos  = control->GetCurrentPos();
            line = control->LineFromPosition(pos);

            // actual code generation
            wxString str;
            if (i > 0)
                str << _T("\n");
            else
                str << ed->GetLineIndentString(line - 1);
            if (addDoxgenComment)
                str << _T("/** @brief ") << token->m_Name << _T("\n  *\n  * @todo: document this function\n  */\n");
            wxString type = token->m_FullType;
            if (!type.IsEmpty())
            {
                // "int *" or "int &" ->  "int*" or "int&"
                if (   (type.Last() == _T('&') || type.Last() == _T('*'))
                    && type[type.Len() - 2] == _T(' '))
                {
                    type[type.Len() - 2] = type.Last();
                    type.RemoveLast();
                }
                str << type << _T(" ");
            }
            if (token->m_ParentIndex != -1)
            {
                const Token* parent = tree->at(token->m_ParentIndex);
                if (parent)
                    str << parent->m_Name << _T("::");
            }
            str << token->m_Name << token->GetStrippedArgs();
            if (token->m_IsConst)
                str << _T(" const");
            if (token->m_IsNoExcept)
                str << _T(" noexcept");
            str << _T("\n{\n\t\n}\n");

            MatchCodeStyle(str, control->GetEOLMode(), ed->GetLineIndentString(line - 1), control->GetUseTabs(), control->GetTabWidth());

            // add code in editor
            control->SetTargetStart(pos);
            control->SetTargetEnd(pos);
            control->ReplaceTarget(str);
            control->GotoPos(pos + str.Length());
        }
        if (!indices.IsEmpty())
        {
            pos  = control->GetCurrentPos();
            line = control->LineFromPosition(pos);
            control->GotoPos(control->GetLineEndPosition(line - 2));
        }
        success = 0;
    }

    // ----------------------------------------------------
    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    // ----------------------------------------------------

    return success;
}//end DoAllMethodsImpl()
// ----------------------------------------------------------------------------
void CodeCompletion::MatchCodeStyle(wxString& str, int eolStyle, const wxString& indent, bool useTabs, int tabSize)
// ----------------------------------------------------------------------------
{
    str.Replace(wxT("\n"), GetEOLStr(eolStyle) + indent);
    if (!useTabs)
        str.Replace(wxT("\t"), wxString(wxT(' '), tabSize));
    if (!indent.IsEmpty())
        str.RemoveLast(indent.Length());
}
// help method in finding the function position in the vector for the function containing the current line
// ----------------------------------------------------------------------------
void CodeCompletion::FunctionPosition(int &scopeItem, int &functionItem) const
// ----------------------------------------------------------------------------
{
    scopeItem = -1;
    functionItem = -1;

    for (unsigned int idxSc = 0; idxSc < m_ScopeMarks.size(); ++idxSc)
    {
        // this is the start and end of a scope
        unsigned int start = m_ScopeMarks[idxSc];
        unsigned int end = (idxSc + 1 < m_ScopeMarks.size()) ? m_ScopeMarks[idxSc + 1] : m_FunctionsScope.size();

        // the scope could have many functions, so loop on the functions
        for (int idxFn = 0; start + idxFn < end; ++idxFn)
        {
            const FunctionScope fs = m_FunctionsScope[start + idxFn];
            if (m_CurrentLine >= fs.StartLine && m_CurrentLine <= fs.EndLine)
            {
                scopeItem = idxSc;
                functionItem = idxFn;
            }
        }
    }
}
// ----------------------------------------------------------------------------
void CodeCompletion::GotoFunctionPrevNext(bool next /* = false */)
// ----------------------------------------------------------------------------
{
    // Original CC code is faster than a LSP request/response
    // but it has a habit of displaying the wrong line because ClassBrowser
    // may be stale. So the clangd version is used instead.

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;
    if (not GetLSP_Initialized(ed))
    {
        InfoWindow::Display("LSP " + wxString(__FUNCTION__), "Editor not parsed yet.", 6000);
        return;
    }

    int current_line = ed->GetControl()->GetCurrentLine();

    if (!m_FunctionsScope.size())
        return;

    // search previous/next function from current line, default: previous
    int          line            = -1;
    unsigned int best_func       = 0;
    bool         found_best_func = false;
    for (unsigned int idx_func=0; idx_func<m_FunctionsScope.size(); ++idx_func)
    {
        int best_func_line  = m_FunctionsScope[best_func].StartLine;
        int func_start_line = m_FunctionsScope[idx_func].StartLine;
        if (next)
        {
            if         (best_func_line  > current_line)     // candidate: is after current line
            {
                if (   (func_start_line > current_line  )   // another candidate
                    && (func_start_line < best_func_line) ) // decide which is more near
                { best_func = idx_func; found_best_func = true; }
            }
            else if    (func_start_line > current_line)     // candidate: is after current line
            {     best_func = idx_func; found_best_func = true; }
        }
        else // prev
        {
            if         (best_func_line  < current_line)     // candidate: is before current line
            {
                if (   (func_start_line < current_line  )   // another candidate
                    && (func_start_line > best_func_line) ) // decide which is closer
                { best_func = idx_func; found_best_func = true; }
            }
            else if    (func_start_line < current_line)     // candidate: is before current line
            {     best_func = idx_func; found_best_func = true; }
        }
    }

    if      (found_best_func)
    { line = m_FunctionsScope[best_func].StartLine; }
    else if ( next && m_FunctionsScope[best_func].StartLine>current_line)
    { line = m_FunctionsScope[best_func].StartLine; }
    else if (!next && m_FunctionsScope[best_func].StartLine<current_line)
    { line = m_FunctionsScope[best_func].StartLine; }

    if (line != -1)
    {
        ed->GotoLine(line);
        ed->SetFocus();
    }
}
// help method in finding the namespace position in the vector for the namespace containing the current line
// ----------------------------------------------------------------------------
int CodeCompletion::NameSpacePosition() const
// ----------------------------------------------------------------------------
{
    int pos = -1;
    int startLine = -1;
    for (unsigned int idxNs = 0; idxNs < m_NameSpaces.size(); ++idxNs)
    {
        const NameSpace& ns = m_NameSpaces[idxNs];
        if (m_CurrentLine >= ns.StartLine && m_CurrentLine <= ns.EndLine && ns.StartLine > startLine)
        {
            // got one, maybe there might be a better fitting namespace
            // (embedded namespaces) so keep on looking
            pos = static_cast<int>(idxNs);
            startLine = ns.StartLine;
        }
    }

    return pos;
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnScope(wxCommandEvent&)
// ----------------------------------------------------------------------------
{
    int sel = m_Scope->GetSelection();
    if (sel != -1 && sel < static_cast<int>(m_ScopeMarks.size()))
        UpdateFunctions(sel);
}

// ----------------------------------------------------------------------------
void CodeCompletion::OnFunction(cb_unused wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    int selSc = (m_Scope) ? m_Scope->GetSelection() : 0;
    if (selSc != -1 && selSc < static_cast<int>(m_ScopeMarks.size()))
    {
        int idxFn = m_ScopeMarks[selSc] + m_Function->GetSelection();
        if (idxFn != -1 && idxFn < static_cast<int>(m_FunctionsScope.size()))
        {
            cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (ed)
                ed->GotoTokenPosition(m_FunctionsScope[idxFn].StartLine,
                                      m_FunctionsScope[idxFn].ShortName);
        }
    }
}

/** Here is the expansion of how the two wxChoices are constructed.
 * for a file which has such contents below
 * @code{.cpp}
 *  Line  0     void g_func1(){
 *  Line  1     }
 *  Line  2
 *  Line  3     void ClassA::func1(){
 *  Line  4     }
 *  Line  5
 *  Line  6     void ClassA::func2(){
 *  Line  7     }
 *  Line  8
 *  Line  9     void ClassB::func1(){
 *  Line 10     }
 *  Line 11
 *  Line 12     void ClassB::func2(){
 *  Line 13     }
 *  Line 14
 *  Line 15     namespace NamespaceA{
 *  Line 16         void func3(){
 *  Line 17         }
 *  Line 18
 *  Line 19         class ClassC {
 *  Line 20
 *  Line 21             void func4(){
 *  Line 22             }
 *  Line 23         }
 *  Line 24     }
 *  Line 25
 *
 * @endcode
 *
 * The two key variable will be constructed like below
 * @code
 *  m_FunctionsScope is std::vector of length 9, capacity 9 =
 *  {
 *  {StartLine = 0, EndLine = 1, ShortName = L"g_func1", Name = L"g_func1() : void", Scope = L"<global>"},
 *  {StartLine = 3, EndLine = 4, ShortName = L"func1", Name = L"func1() : void", Scope = L"ClassA::"},
 *  {StartLine = 6, EndLine = 7, ShortName = L"func2", Name = L"func2() : void", Scope = L"ClassA::"},
 *  {StartLine = 9, EndLine = 10, ShortName = L"func1", Name = L"func1() : void", Scope = L"ClassB::"},
 *  {StartLine = 12, EndLine = 13, ShortName = L"func2", Name = L"func2() : void", Scope = L"ClassB::"},
 *  {StartLine = 14, EndLine = 23, ShortName = L"", Name = L"", Scope = L"NamespaceA::"},
 *  {StartLine = 16, EndLine = 17, ShortName = L"func3", Name = L"func3() : void", Scope = L"NamespaceA::"},
 *  {StartLine = 19, EndLine = 23, ShortName = L"", Name = L"", Scope = L"NamespaceA::ClassC::"},
 *  {StartLine = 21, EndLine = 22, ShortName = L"func4", Name = L"func4() : void", Scope = L"NamespaceA::ClassC::"}
 *  }
 *
 *  m_NameSpaces is std::vector of length 1, capacity 1 =
 *  {{Name = L"NamespaceA::", StartLine = 14, EndLine = 23}}
 *
 *  m_ScopeMarks is std::vector of length 5, capacity 8 = {0, 1, 3, 5, 7}
 * which is the start of Scope "<global>", Scope "ClassA::" and Scope "ClassB::",
 * "NamespaceA::" and "NamespaceA::ClassC::"
 * @endcode
 *
 * Then we have wxChoice Scopes and Functions like below
 * @code
 *      <global>          ClassA::        ClassB::
 *        |- g_func1()      |- func1()      |- func1()
 *                          |- func2()      |- func2()
 *
 *      NamespaceA::      NamespaceA::ClassC::
 *        |- func3()        |- func4()
 * @endcode
 */
// ----------------------------------------------------------------------------
void CodeCompletion::ParseFunctionsAndFillToolbar()
// ----------------------------------------------------------------------------
{
    TRACE(_T("ParseFunctionsAndFillToolbar() Entered: m_ToolbarNeedReparse=%d, m_ToolbarNeedRefresh=%d, "),
          m_ToolbarNeedReparse?1:0, m_ToolbarNeedRefresh?1:0);

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan) // Closing the app?
        return;

    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if ( !ed || !ed->GetControl())
    {
        if (m_Function)
            m_Function->Clear();
        if (m_Scope)
            m_Scope->Clear();

        EnableToolbarTools(false);
        m_LastFile.Clear();
        return;
    }

    const wxString filename = ed->GetFilename();
    if (filename.IsEmpty())
        return;

    bool fileParseFinished = GetParseManager()->GetParser().IsFileParsed(filename);

    // FunctionsScopePerFile contains all the function and namespace information for
    // a specified file, m_AllFunctionsScopes[filename] will implicitly insert an new element in
    // the map if no such key(filename) is found.
    FunctionsScopePerFile* funcdata = &(m_AllFunctionsScopes[filename]);

    #if CC_CODECOMPLETION_DEBUG_OUTPUT == 1 //(ph 2021/04/3)
        wxString debugMsg = wxString::Format("ParseFunctionsAndFillToolbar() : m_ToolbarNeedReparse=%d, m_ToolbarNeedRefresh=%d, ",
              m_ToolbarNeedReparse?1:0, m_ToolbarNeedRefresh?1:0);
        debugMsg += wxString::Format("\n%s: funcdata->parsed[%d] ", __FUNCTION__, funcdata->parsed);
        LogManager* pLogMgr = CCLogger::Get()->pLogMgr->DebugLog(debugMsg);
    #endif

    // *** Part 1: Parse the file (if needed) ***
    if (m_ToolbarNeedReparse || !funcdata->parsed)
    {
        TRACE("ParseFunctionsAndFillToolbar() Entered Part 1: Parse the file (if needed)");

        // -----------------------------------------------------
        //CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)             // LOCK TokenTree
        // -----------------------------------------------------
        // If the lock is busy, a callback is queued for idle time.
        auto locker_result = s_TokenTreeMutex.LockTimeout(250);
        if (locker_result != wxMUTEX_NO_ERROR)
        {
            // lock failed, do not block the UI thread, call back when idle
            GetIdleCallbackHandler()->QueueCallback(this, &CodeCompletion::ParseFunctionsAndFillToolbar);
            return;
        }
        else /*lock succeeded*/
            s_TokenTreeMutex_Owner = wxString::Format("%s %d",__PRETTY_FUNCTION__, __LINE__); /*record owner*/

        if (m_ToolbarNeedReparse)
        {
            m_ToolbarNeedReparse = false;
            //?m_ToolbarNeedRefresh = true;    //(ph 2021/04/3)
        }

        funcdata->m_FunctionsScope.clear();
        funcdata->m_NameSpaces.clear();


        // collect the function implementation information, just find the specified tokens in the TokenTree
        TokenIdxSet result;
        bool hasTokenTreeLock = true;
        GetParseManager()->GetParser().FindTokensInFile(filename, result,
                                                    tkAnyFunction | tkEnum | tkClass | tkNamespace, hasTokenTreeLock);
        if ( ! result.empty())
            funcdata->parsed = true;    // if the file had some containers, flag it as parsed
        else
            fileParseFinished = false;  // this indicates the batch parser has not finish parsing for the current file

        TokenTree* tree = GetParseManager()->GetParser().GetTokenTree();

        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && token->m_ImplLine != 0)
            {
                FunctionScope fs;
                fs.StartLine = token->m_ImplLine    - 1;
                fs.EndLine   = token->m_ImplLineEnd - 1;
                const size_t fileIdx = tree->InsertFileOrGetIndex(filename);
                if (token->m_TokenKind & tkAnyFunction && fileIdx == token->m_ImplFileIdx)
                {
                    fs.Scope = token->GetNamespace();
                    if (fs.Scope.IsEmpty())
                        fs.Scope = g_GlobalScope;
                    wxString result_str = token->m_Name;
                    fs.ShortName = result_str;
                    result_str << token->GetFormattedArgs();
                    if (!token->m_BaseType.IsEmpty())
                        result_str << _T(" : ") << token->m_BaseType;
                    fs.Name = result_str;
                    funcdata->m_FunctionsScope.push_back(fs);
                }
                else if (token->m_TokenKind & (tkEnum | tkClass | tkNamespace))
                {
                    fs.Scope = token->GetNamespace() + token->m_Name + _T("::");
                    funcdata->m_FunctionsScope.push_back(fs);
                }
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        FunctionsScopeVec& functionsScopes = funcdata->m_FunctionsScope;
        NameSpaceVec& nameSpaces = funcdata->m_NameSpaces;

        // collect the namespace information in the current file, this is done by running a parserthread
        // on the editor's buffer
        //-GetParseManager()->GetParser().ParseBufferForNamespaces(ed->GetControl()->GetText(), nameSpaces);
        //^^ namespaces already parsed by clangd and entered into the tree by OnLSP_RequestedSymbolsResponse() //(ph 2021/07/27)

        std::sort(nameSpaces.begin(), nameSpaces.end(), CodeCompletionHelper::LessNameSpace);

        // copy the namespace information collected in ParseBufferForNamespaces() to
        // the functionsScopes, note that the element type FunctionScope has a constructor
        // FunctionScope(const NameSpace& ns), type conversion is done automatically
        std::copy(nameSpaces.begin(), nameSpaces.end(), back_inserter(functionsScopes));
        std::sort(functionsScopes.begin(), functionsScopes.end(), CodeCompletionHelper::LessFunctionScope);

        // remove consecutive duplicates
        FunctionsScopeVec::const_iterator it;
        it = unique(functionsScopes.begin(), functionsScopes.end(), CodeCompletionHelper::EqualFunctionScope);
        functionsScopes.resize(it - functionsScopes.begin());

        TRACE(F(_T("Found %lu namespace locations"), static_cast<unsigned long>(nameSpaces.size())));
    #if CC_CODECOMPLETION_DEBUG_OUTPUT == 1
        for (unsigned int i = 0; i < nameSpaces.size(); ++i)
            CCLogger::Get()->DebugLog(wxString::Format(_T("\t%s (%d:%d)"),
                nameSpaces[i].Name.wx_str(), nameSpaces[i].StartLine, nameSpaces[i].EndLine));
    #endif

        if (!m_ToolbarNeedRefresh)
            m_ToolbarNeedRefresh = true;
    }

    // *** Part 2: Fill the toolbar ***
    TRACE("ParseFunctionsAndFillToolbar() Entered: Part 2: Fill the toolbar");
    m_FunctionsScope = funcdata->m_FunctionsScope;
    m_NameSpaces     = funcdata->m_NameSpaces;

    m_ScopeMarks.clear();
    unsigned int fsSize = m_FunctionsScope.size();
    if (!m_FunctionsScope.empty())
    {
        m_ScopeMarks.push_back(0);

        if (m_Scope) // show scope wxChoice
        {
            wxString lastScope = m_FunctionsScope[0].Scope;
            for (unsigned int idx = 1; idx < fsSize; ++idx)
            {
                const wxString& currentScope = m_FunctionsScope[idx].Scope;

                // if the scope name has changed, push a new index
                if (lastScope != currentScope)
                {
                    m_ScopeMarks.push_back(idx);
                    lastScope = currentScope;
                }
            }
        }
    }

    TRACE(F(_T("Parsed %lu functionScope items"), static_cast<unsigned long>(m_FunctionsScope.size())));
    #if CC_CODECOMPLETION_DEBUG_OUTPUT == 1
    for (unsigned int i = 0; i < m_FunctionsScope.size(); ++i)
        CCLogger::Get()->DebugLog(wxString::Format(_T("\t%s%s (%d:%d)"),
            m_FunctionsScope[i].Scope.wx_str(), m_FunctionsScope[i].Name.wx_str(),
            m_FunctionsScope[i].StartLine, m_FunctionsScope[i].EndLine));
    #endif

    // Does the toolbar need a refresh?
    if (m_ToolbarNeedRefresh || m_LastFile != filename)
    {
        // Update the last editor and changed flag...
        if (m_ToolbarNeedRefresh)
            m_ToolbarNeedRefresh = false;
        if (m_LastFile != filename)
        {
            TRACE(_T("ParseFunctionsAndFillToolbar() : Update last file is %s"), filename.wx_str());
            m_LastFile = filename;
        }

        TRACE(wxString::Format("%s(): m_Scope[%d] m_FunctionScope[%d]", __PRETTY_FUNCTION__, m_ScopeMarks.size(), m_FunctionsScope.size() ));
        //- **debugging** CCLogger::Get()->DebugLog(wxString::Format("%s(): m_Scope[%d] m_FunctionScope[%d]", __PRETTY_FUNCTION__, m_ScopeMarks.size(), m_FunctionsScope.size() ));

        // ...and refresh the toolbars.
        m_Function->Clear();

        if (m_Scope)
        {
            m_Scope->Freeze();
            m_Scope->Clear();

            // add to the choice controls
            for (unsigned int idxSc = 0; idxSc < m_ScopeMarks.size(); ++idxSc)
            {
                int idxFn = m_ScopeMarks[idxSc];
                const FunctionScope& fs = m_FunctionsScope[idxFn];
                m_Scope->Append(fs.Scope);
            }

            m_Scope->Thaw();
        }
        else
        {
            m_Function->Freeze();

            for (unsigned int idxFn = 0; idxFn < m_FunctionsScope.size(); ++idxFn)
            {
                const FunctionScope& fs = m_FunctionsScope[idxFn];
                if (fs.Name != wxEmptyString)
                    m_Function->Append(fs.Scope + fs.Name);
                else if (fs.Scope.EndsWith(wxT("::")))
                    m_Function->Append(fs.Scope.substr(0, fs.Scope.length()-2));
                else
                    m_Function->Append(fs.Scope);
            }

            m_Function->Thaw();
        }
    }

    // Find the current function and update
    FindFunctionAndUpdate(ed->GetControl()->GetCurrentLine());

    // Control the toolbar state, if the batch parser has not finished parsing the file, no need to update CC toolbar.
    EnableToolbarTools(fileParseFinished);
}
// ----------------------------------------------------------------------------
void CodeCompletion::FindFunctionAndUpdate(int currentLine)
// ----------------------------------------------------------------------------
{
    if (currentLine == -1)
        return;

    m_CurrentLine = currentLine;

    int selSc, selFn;
    FunctionPosition(selSc, selFn);

    if (m_Scope)
    {
        if (selSc != -1 && selSc != m_Scope->GetSelection())
        {
            m_Scope->SetSelection(selSc);
            UpdateFunctions(selSc);
        }
        else if (selSc == -1)
            m_Scope->SetSelection(-1);
    }

    if (selFn != -1 && selFn != m_Function->GetSelection())
        m_Function->SetSelection(selFn);
    else if (selFn == -1)
    {
        m_Function->SetSelection(-1);

        wxChoice* choice = (m_Scope) ? m_Scope : m_Function;

        int NsSel = NameSpacePosition();
        if (NsSel != -1)
            choice->SetStringSelection(m_NameSpaces[NsSel].Name);
        else if (!m_Scope)
            choice->SetSelection(-1);
        else
        {
            choice->SetStringSelection(g_GlobalScope);
            wxCommandEvent evt(wxEVT_COMMAND_CHOICE_SELECTED, XRCID("chcCodeCompletionScope"));
            wxPostEvent(this, evt);
        }
    }
}
// ----------------------------------------------------------------------------
void CodeCompletion::UpdateFunctions(unsigned int scopeItem)
// ----------------------------------------------------------------------------
{
    m_Function->Freeze();
    m_Function->Clear();

    unsigned int idxEnd = (scopeItem + 1 < m_ScopeMarks.size()) ? m_ScopeMarks[scopeItem + 1] : m_FunctionsScope.size();
    for (unsigned int idxFn = m_ScopeMarks[scopeItem]; idxFn < idxEnd; ++idxFn)
    {
        const wxString &name = m_FunctionsScope[idxFn].Name;
        m_Function->Append(name);
    }

    m_Function->Thaw();
}
// ----------------------------------------------------------------------------
void CodeCompletion::EnableToolbarTools(bool enable)
// ----------------------------------------------------------------------------
{
    if (m_Scope)
        m_Scope->Enable(enable);
    if (m_Function)
        m_Function->Enable(enable);
}

// ----------------------------------------------------------------------------
void CodeCompletion::DoParseOpenedProjectAndActiveEditor()
// ----------------------------------------------------------------------------
{
    // Let the app startup before parsing
    // This is to prevent the Splash Screen from delaying so much. By adding
    // the timer, the splash screen is closed and Code::Blocks doesn't take
    // so long in starting.

    m_InitDone = true;

    // Clangd_plugin does not yet support non-project files
    // Neither do we want to create a parser yet
    // FIXME (ph#): Re-state this code when non-project files supported.
    return; //(ph 2021/10/14) deprecated for now.

    // Dreaded DDE-open bug related: do not touch the following lines unless for a good reason

    // parse any projects opened through DDE or the command-line
    cbProject* curProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (curProject && !GetParseManager()->GetParserByProject(curProject))
        GetParseManager()->CreateParser(curProject);

    // parse any files opened through DDE or the command-line
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        GetParseManager()->OnEditorActivated(editor);
}
// ----------------------------------------------------------------------------
void CodeCompletion::UpdateEditorSyntax(cbEditor* ed)
// ----------------------------------------------------------------------------
{
    if (!Manager::Get()->GetConfigManager(wxT("clangd_client"))->ReadBool(wxT("/semantic_keywords"), false))
        return;
    if (!ed)
        ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || ed->GetControl()->GetLexer() != wxSCI_LEX_CPP)
        return;

    TokenIdxSet result;
    int flags = tkAnyContainer | tkAnyFunction;
    if (ed->GetFilename().EndsWith(wxT(".c")))
        flags |= tkVariable;

    // -----------------------------------------------------
    //CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)
    // -----------------------------------------------------
    // Avoid blocking main thread, If the lock is busy, queue a callback at idle time.
    auto locker_result = s_TokenTreeMutex.LockTimeout(250);
    if (locker_result != wxMUTEX_NO_ERROR)
    {
        // lock failed, do not block the UI thread, call back when idle
        GetIdleCallbackHandler()->QueueCallback(this, &CodeCompletion::UpdateEditorSyntax, ed);
        return;
    }
    else /*lock succeeded*/
        s_TokenTreeMutex_Owner = wxString::Format("%s %d",__PRETTY_FUNCTION__, __LINE__); /*record owner*/

    bool hasTokenTreeLock = true;
    GetParseManager()->GetParser().FindTokensInFile(ed->GetFilename(), result, flags, hasTokenTreeLock);
    TokenTree* tree = GetParseManager()->GetParser().GetTokenTree();

    std::set<wxString> varList;
    TokenIdxSet parsedTokens;

    for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
    {
        Token* token = tree->at(*it);
        if (!token)
            continue;
        if (token->m_TokenKind == tkVariable) // global var - only added in C
        {
            varList.insert(token->m_Name);
            continue;
        }
        else if (token->m_TokenKind & tkAnyFunction) // find parent class
        {
            if (token->m_ParentIndex == wxNOT_FOUND)
                continue;
            else
                token = tree->at(token->m_ParentIndex);
        }
        if (!token || parsedTokens.find(token->m_Index) != parsedTokens.end())
            continue; // no need to check the same token multiple times
        parsedTokens.insert(token->m_Index);
        for (TokenIdxSet::const_iterator chIt = token->m_Children.begin();
             chIt != token->m_Children.end(); ++chIt)
        {
            const Token* chToken = tree->at(*chIt);
            if (chToken && chToken->m_TokenKind == tkVariable)
            {
                varList.insert(chToken->m_Name);
            }
        }
        // inherited members
        if (token->m_Ancestors.empty())
            tree->RecalcInheritanceChain(token);
        for (TokenIdxSet::const_iterator ancIt = token->m_Ancestors.begin();
             ancIt != token->m_Ancestors.end(); ++ancIt)
        {
            const Token* ancToken = tree->at(*ancIt);
            if (!ancToken || parsedTokens.find(ancToken->m_Index) != parsedTokens.end())
                continue;
            for (TokenIdxSet::const_iterator chIt = ancToken->m_Children.begin();
                 chIt != ancToken->m_Children.end(); ++chIt)
            {
                const Token* chToken = tree->at(*chIt);
                if (   chToken && chToken->m_TokenKind == tkVariable
                    && chToken->m_Scope != tsPrivate) // cannot inherit these...
                {
                    varList.insert(chToken->m_Name);
                }
            }
        }
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    EditorColourSet* colour_set = Manager::Get()->GetEditorManager()->GetColourSet();
    if (!colour_set)
        return;

    wxString keywords = colour_set->GetKeywords(ed->GetLanguage(), 3);
    for (std::set<wxString>::const_iterator keyIt = varList.begin();
         keyIt != varList.end(); ++keyIt)
    {
        keywords += wxT(" ") + *keyIt;
    }
    ed->GetControl()->SetKeyWords(3, keywords);
    ed->GetControl()->Colourise(0, -1);
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnToolbarTimer(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Allow others to call OnToolbarTimer() via event.     //(ph 2021/09/11)
    // Invoked by Parser::OnLSP_ParseDocumentSymbols() to update the scope toolbar
    if (not m_CodeCompletionEnabled) return;

    m_ToolbarNeedReparse = true;
    m_ToolbarNeedRefresh = true;

    wxTimerEvent evt(m_TimerToolbar);
    OnToolbarTimer(evt);
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnToolbarTimer(cb_unused wxTimerEvent& event)
// ----------------------------------------------------------------------------
{
    // Update the Code completion tool bar

    TRACE(_T("CodeCompletion::OnToolbarTimer(): Enter"));

    // stop any timer event since non timer events can enter here, esp., from parsers //(ph 2021/09/11)
    if (m_TimerToolbar.IsRunning())
        m_TimerToolbar.Stop();

    if (not ProjectManager::IsBusy())
        ParseFunctionsAndFillToolbar();
    else
    {
        TRACE(_T("CodeCompletion::OnToolbarTimer(): Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    }

    TRACE(_T("CodeCompletion::OnToolbarTimer(): Leave"));
}
// ----------------------------------------------------------------------------
void CodeCompletion::OnEditorActivatedTimer(cb_unused wxTimerEvent& event)
// ----------------------------------------------------------------------------
{
    // the m_LastEditor variable was updated in CodeCompletion::OnEditorActivated, after that,
    // the editor-activated-timer was started. So, here in the timer handler, we need to check
    // whether the saved editor and the current editor are the same, otherwise, no need to update
    // the toolbar, because there must be another editor activated before the timer hits.
    // Note: only the builtin active editor is considered.
    EditorBase* editor  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!editor || editor != m_LastEditor)
    {
        TRACE(_T("CodeCompletion::OnEditorActivatedTimer(): Not a builtin editor."));
        //m_LastEditor = nullptr;
        EnableToolbarTools(false);
        return;
    }

    const wxString& curFile = editor->GetFilename();
    // if the same file was activated, no need to update the toolbar
    if ( !m_LastFile.IsEmpty() && m_LastFile == curFile )
    {
        TRACE(_T("CodeCompletion::OnEditorActivatedTimer(): Same as the last activated file(%s)."), curFile.wx_str());
        return;
    }

    TRACE(_T("CodeCompletion::OnEditorActivatedTimer(): Need to notify ParseManager and Refresh toolbar."));

    GetParseManager()->OnEditorActivated(editor);
    TRACE(_T("CodeCompletion::OnEditorActivatedTimer: Starting m_TimerToolbar."));
    m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    TRACE(_T("CodeCompletion::OnEditorActivatedTimer(): Current activated file is %s"), curFile.wx_str());
    UpdateEditorSyntax();
}
// ----------------------------------------------------------------------------
wxBitmap CodeCompletion::GetImage(ImageId::Id id, int fontSize) //unused
// ----------------------------------------------------------------------------
{
    const int size = cbFindMinSize16to64(fontSize);
    const ImageId key(id, size);
    ImagesMap::const_iterator it = m_images.find(key);
    if (it == m_images.end())
    {
        const wxString prefix = ConfigManager::GetDataFolder()
                              + wxString::Format(_T("/codecompletion.zip#zip:images/%dx%d/"), size,
                                                 size);

        wxString filename;
        switch (id)
        {
            case ImageId::HeaderFile:
                filename = prefix + wxT("header.png");
                break;
            case ImageId::KeywordCPP:
                filename = prefix + wxT("keyword_cpp.png");
                break;
            case ImageId::KeywordD:
                filename = prefix + wxT("keyword_d.png");
                break;
            case ImageId::Unknown:
                filename = prefix + wxT("unknown.png");
                break;

            case ImageId::Last:
            default:
                ;
        }

        if (!filename.empty())
        {
            wxBitmap bitmap = cbLoadBitmap(filename);
            if (!bitmap.IsOk())
            {
                const wxString msg = wxString::Format(_("Cannot load image: '%s'!"),
                                                      filename.wx_str());
                Manager::Get()->GetLogManager()->LogError(msg);
                CCLogger::Get()->DebugLog(msg);
            }
            m_images[key] = bitmap;
            return bitmap;
        }
        else
        {
            m_images[key] = wxNullBitmap;
            return wxNullBitmap;
        }
    }
    else
        return it->second;
}
// ----------------------------------------------------------------------------
wxString CodeCompletion::GetTargetsOutFilename(cbProject* pProject)                  //(ph 2021/05/11)
// ----------------------------------------------------------------------------
{
    // Return the build targets output file name or nullString

    ProjectBuildTarget* pTarget = nullptr;
    //-Compiler* actualCompiler = 0;
    wxString buildOutputFile;
    wxString activeBuildTarget;

    if ( pProject)
    {
        //-Log(_("Selecting target: "));
        activeBuildTarget = pProject->GetActiveBuildTarget();
        if (not pProject->BuildTargetValid(activeBuildTarget, false))
        {
            int tgtIdx = pProject->SelectTarget();
            if (tgtIdx == -1)
            {
                //-Log(_("canceled"));
                return wxString();
            }
            pTarget = pProject->GetBuildTarget(tgtIdx);
            activeBuildTarget = (pTarget ? pTarget->GetTitle() : wxString(wxEmptyString));
        }
        else
            pTarget = pProject->GetBuildTarget(activeBuildTarget);

        // make sure it's not a commands-only target
        if (pTarget && pTarget->GetTargetType() == ttCommandsOnly)
        {
            //cbMessageBox(_("The selected target is only running pre/post build step commands\n"
            //               "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            //Log(_("aborted"));
            return wxString();
        }
        //-if (target) Log(target->GetTitle());

        if (pTarget )
            buildOutputFile = pTarget->GetOutputFilename();
    }

    if (buildOutputFile.Length())
    {
        return buildOutputFile;
    }

    return wxString();

}
// ----------------------------------------------------------------------------
void CodeCompletion::OnDebuggerStarting(CodeBlocksEvent& event)                  //(ph 2021/05/11)
// ----------------------------------------------------------------------------
{
    cbProject* pProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    PluginManager* pPlugMgr = Manager::Get()->GetPluginManager();
    ProcessLanguageClient* pClient = GetLSPclient(pProject);
    if (not pClient) return;

    PluginElement* pPlugElements = pPlugMgr->FindElementByName("Clangd_Client");
    wxFileName pluginLibName = pPlugElements->fileName;

    // if projects filename matches the LSP client/server dll,
    // shutdown the debuggers LSP client/server to avoid clobbering symbols cache.
    wxFileName fnOutFilename = GetTargetsOutFilename(pProject);

    wxString outFilename = fnOutFilename.GetName().Lower();
    wxString pluginDllName = pluginLibName.GetName().Lower();
    if ( not (outFilename.Contains(pluginDllName.Lower())) )
        return;

    wxString msg = "Clangd client/server can be shutdown for the project about to be debugged";
    msg += "\n to avoid multiple processes writing to the same clangd symbols cache.";
    msg += "\n If you are going to load a project OTHER than the current project as the debuggee";
    msg += "\n you do not have to shut down the current clangd client.";
    msg += "\n\n If you choose to shutdown, you can, later, restart clangd via menu 'Project/Reparse current project'.";
    msg += "\n\nShut down clangd client for this project?";
    AnnoyingDialog annoyingDlg("Debugger Starting", msg, wxART_QUESTION, AnnoyingDialog::YES_NO, AnnoyingDialog::rtSAVE_CHOICE);
    PlaceWindow(&annoyingDlg);
    int answ = annoyingDlg.ShowModal();
    if (answ == AnnoyingDialog::rtNO) return;

    ShutdownLSPclient(pProject);

    bool release = true;
    DoLockClangd_CacheAccess(pProject, release);

}
// ----------------------------------------------------------------------------
void CodeCompletion::OnDebuggerFinished(CodeBlocksEvent& event)                 //(ph 2021/05/11)
// ----------------------------------------------------------------------------
{

}
// ----------------------------------------------------------------------------
bool CodeCompletion::ParsingIsVeryBusy()
// ----------------------------------------------------------------------------
{
    // suggestion: max parallel files parsing should be no more than half of processors
    int max_parallel_processes = std::max(1, wxThread::GetCPUCount());
    if (max_parallel_processes > 1) max_parallel_processes = max_parallel_processes >> 1; //use only half of cpus
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("clangd_client"));
    int cfg_parallel_processes = std::max(cfg->ReadInt("/max_threads", 1), 1);            //don't allow 0
    max_parallel_processes = std::min(max_parallel_processes, cfg_parallel_processes);

    cbEditor* pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not pEditor) return false;

    ProcessLanguageClient* pClient = GetLSPclient(pEditor);
    if ( int(pClient->LSP_GetServerFilesParsingCount()) > max_parallel_processes)
    {
        wxString msg = "Parsing is very busy, response may be delayed.";
        InfoWindow::Display("LSP parsing", msg, 6000);
        return true;
    }
    return false;
}
