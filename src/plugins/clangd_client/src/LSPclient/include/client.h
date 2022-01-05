//
// Created by Alex on 2020/1/28.
//


#ifndef LSP_CLIENT_H
#define LSP_CLIENT_H


#include "transport.h"
#include "protocol.h"

//#include "windows.h"
#include "wx/event.h"
#include "wx/string.h"
#include <wx/app.h>                 // wxWakeUpIdle

#include "globals.h"        //(ph 2020/09/23)
#include "sdk_events.h"     //(ph 2020/09/23)
#include "asyncprocess.h"          // cl asyncProcess
#include "processreaderthread.h"   // cl asyncProcess
#include "cl_command_event.h"      // cl asyncProcess
#include "winprocess_impl.h"       // cl asyncProcess

#include "editormanager.h"
#include "cbeditor.h"
#include "editorbase.h"     //(ph 2021/04/10)
#include "cbstyledtextctrl.h"
#include "cbproject.h"
#include "..\lspdiagresultslog.h"
#include "logmanager.h"
#include "configmanager.h"
#include "encodingdetector.h"

class TextCtrlLogger;

// ----------------------------------------------------------------------------
class LanguageClient : public JsonTransport
// ----------------------------------------------------------------------------
{
    public:
        virtual ~LanguageClient() = default;
        const char STX = '\u0002'; //(ph 2021/03/17)

    public:
        RequestID Initialize(option<DocumentUri> rootUri = {}) {
            InitializeParams params;
            params.processId = GetCurrentProcessId();
            params.rootUri = rootUri;
            return SendRequest("initialize", params);
        }

        RequestID Shutdown() {
            return SendRequest("shutdown");
        }

        RequestID Sync() {
            return SendRequest("sync");
        }

        void Exit() {
            SendNotify("exit");
        }

        void Initialized() {
            SendNotify("initialized");
        }

        RequestID RegisterCapability() {
            return SendRequest("client/registerCapability");
        }

        void DidOpen(DocumentUri uri, string_ref text, string_ref languageId = "cpp") {
            DidOpenTextDocumentParams params;
            params.textDocument.uri = std::move(uri);
            params.textDocument.text = text;
            params.textDocument.languageId = languageId;
            SendNotify("textDocument/didOpen", params);
        }

        void DidClose(DocumentUri uri) {
            DidCloseTextDocumentParams params;
            params.textDocument.uri = std::move(uri);
            SendNotify("textDocument/didClose", params);
        }

        void DidSave(DocumentUri uri) {                 //(ph 2020/11/21)
            DidCloseTextDocumentParams params;
            params.textDocument.uri = std::move(uri);
            SendNotify("textDocument/didSave", params);
        }

        void DidChange(DocumentUri uri, std::vector<TextDocumentContentChangeEvent> &changes,
                       option<bool> wantDiagnostics = {}) {
            DidChangeTextDocumentParams params;
            params.textDocument.uri = std::move(uri);
            params.contentChanges = std::move(changes);
            params.wantDiagnostics = wantDiagnostics;
            SendNotify("textDocument/didChange", params);
        }
        //(ph 2021/02/11)
        void NotifyDidChangeConfiguration(ConfigurationSettings &settings) {
            DidChangeConfigurationParams params;
            params.settings = std::move(settings);
            SendNotify("workspace/didChangeConfiguration", std::move(params));
        }

        RequestID RangeFomatting(DocumentUri uri, Range range) {
            DocumentRangeFormattingParams params;
            params.textDocument.uri = std::move(uri);
            params.range = range;
            return SendRequest("textDocument/rangeFormatting", params);
        }

        RequestID FoldingRange(DocumentUri uri) {
            FoldingRangeParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequest("textDocument/foldingRange", params);
        }

        RequestID SelectionRange(DocumentUri uri, std::vector<Position> &positions) {
            SelectionRangeParams params;
            params.textDocument.uri = std::move(uri);
            params.positions = std::move(positions);
            return SendRequest("textDocument/selectionRange", params);
        }

        RequestID OnTypeFormatting(DocumentUri uri, Position position, string_ref ch) {
            DocumentOnTypeFormattingParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            params.ch = std::move(ch);
            return SendRequest("textDocument/onTypeFormatting", std::move(params));
        }

        RequestID Formatting(DocumentUri uri) {
            DocumentFormattingParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequest("textDocument/formatting", std::move(params));
        }

        RequestID CodeAction(DocumentUri uri, Range range, CodeActionContext context) {
            CodeActionParams params;
            params.textDocument.uri = std::move(uri);
            params.range = range;
            params.context = std::move(context);
            return SendRequest("textDocument/codeAction", std::move(params));
        }

        RequestID Completion(DocumentUri uri, Position position, option<CompletionContext> context = {}) {
            CompletionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            params.context = context;
            return SendRequest("textDocument/completion", params);
        }

        RequestID SignatureHelp(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/signatureHelp", std::move(params));
        }

        RequestID GoToDefinition(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/definition", std::move(params));
        }

        RequestID GoToDefinitionByID(DocumentUri uri, Position position, std::string reqID)
        {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            //return SendRequest("textDocument/definition", std::move(params));
            return SendRequestByID("textDocument/definition", reqID, std::move(params));

        }

        RequestID GoToDeclaration(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/declaration", std::move(params));
        }

        RequestID GoToDeclarationByID(DocumentUri uri, Position position, std::string reqID)
        {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            //-return SendRequest("textDocument/declaration", std::move(params));
            return SendRequestByID("textDocument/declaration", reqID, std::move(params));
        }

        RequestID References(DocumentUri uri, Position position) {
            ReferenceParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/references", std::move(params));
        }

        RequestID SwitchSourceHeader(DocumentUri uri) {
            TextDocumentIdentifier params;
            params.uri = std::move(uri);
            return SendRequest("textDocument/references", std::move(params));
        }

        RequestID Rename(DocumentUri uri, Position position, string_ref newName) {
            RenameParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            params.newName = newName;
            return SendRequest("textDocument/rename", std::move(params));
        }

        RequestID Hover(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/hover", std::move(params));
        }

        RequestID DocumentSymbol(DocumentUri uri) {
            DocumentSymbolParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequest("textDocument/documentSymbol", std::move(params));
        }

        RequestID DocumentSymbolByID(DocumentUri uri, std::string reqID) {
            DocumentSymbolParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequestByID("textDocument/documentSymbol", reqID, std::move(params));
        }

        RequestID DocumentColor(DocumentUri uri) {
            DocumentSymbolParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequest("textDocument/documentColor", std::move(params));
        }

        RequestID DocumentHighlight(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/documentHighlight", std::move(params));
        }
        RequestID SymbolInfo(DocumentUri uri, Position position) {
            TextDocumentPositionParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            return SendRequest("textDocument/symbolInfo", std::move(params));
        }
        RequestID TypeHierarchy(DocumentUri uri, Position position, TypeHierarchyDirection direction, int resolve) {
            TypeHierarchyParams params;
            params.textDocument.uri = std::move(uri);
            params.position = position;
            params.direction = direction;
            params.resolve = resolve;
            return SendRequest("textDocument/typeHierarchy", std::move(params));
        }
        RequestID WorkspaceSymbol(string_ref query) {
            WorkspaceSymbolParams params;
            params.query = query;
            return SendRequest("workspace/symbol", std::move(params));
        }
        RequestID ExecuteCommand(string_ref cmd, option<TweakArgs> tweakArgs = {}, option<WorkspaceEdit> workspaceEdit = {}) {
            ExecuteCommandParams params;
            params.tweakArgs = tweakArgs;
            params.workspaceEdit = workspaceEdit;
            params.command = cmd;
            return SendRequest("workspace/executeCommand", std::move(params));
        }
        RequestID DidChangeWatchedFiles(std::vector<FileEvent> &changes) {
            DidChangeWatchedFilesParams params;
            params.changes = std::move(changes);
            return SendRequest("workspace/didChangeWatchedFiles", std::move(params));
        }
        RequestID DidChangeConfiguration(ConfigurationSettings &settings) {
            DidChangeConfigurationParams params;
            params.settings = std::move(settings);
            return SendRequest("workspace/didChangeConfiguration", std::move(params));
        }
        RequestID SemanticTokensByID(DocumentUri uri, std::string reqID) {         //(ph 2021/03/16)
            DocumentSymbolParams params;
            params.textDocument.uri = std::move(uri);
            return SendRequestByID("textDocument/semanticTokens/full", reqID, std::move(params));
    }

    public:
        RequestID SendRequest(string_ref method, value params = json()) {
            RequestID id = method.str();
            request(method, params, id);
            return id;
        }

        RequestID SendRequestByID(string_ref method, string_ref reqID, value params = json()) //(ph 2021/03/12)
        {
            RequestID id = method.str();
            if (reqID.size())
                id += STX + reqID.str();
            request(method, params, id);
            return id;
        }
        void SendNotify(string_ref method, value params = json()) {
            notify(method, params);
        }
};
// ----------------------------------------------------------------------------
class ProcessLanguageClient : public wxEvtHandler, private LanguageClient
// ----------------------------------------------------------------------------
{
    //public:
        //        HANDLE fReadIn = nullptr, fWriteIn = nullptr;
        //        HANDLE fReadOut = nullptr, fWriteOut = nullptr;
        //        PROCESS_INFORMATION fProcess = {nullptr};
    private:
        //(ph 2020/09/23)
        IProcess*       pServerProcess = nullptr;
        long            processServerPID = 0;
        int             idLSP_Process = wxNewId(); //event id for this client instance
        wxString        LSP_IncomingStr;
        bool            terminateLSP = false;
        wxString        fromLSP_data;                   //data sent back from LSP
        int             LSP_UserEventID = -1;
        wxString        LSP_rootURI = wxEmptyString;
        cbProject*      m_pCBProject;

        std::map<cbEditor*, int> m_FileLinesHistory;

        size_t          m_msDidChangeTimeBusy = 0;     //Future time:  in eon milliSecs + busy milliseconds allowed
        size_t          m_msCompletionTimeBusy = 0;    //Future time:  in eon milliSecs + busy milliseconds allowed
        bool            m_LSP_initialized = false;
        bool            m_LSP_responseStatus = false;
        int             m_LSP_CompileCommandsChangedTime = 0; //contains eon time-of-day in milliseconds
        wxArrayString   m_LSP_aIgnoredDiagnostics;
        wxMutex         m_MutexInputBufGuard;

        //-std::map<cbEditor*,int> m_ParseStartMillsTODmap; //key:cbEditor* value: millisecs TOD time-of-eon

        //int                 m_LogPageIndex;
        LSPDiagnosticsResultsLog* m_pDiagnosticsLog = nullptr;

        // Default completion max busy time allowed is 2 secs
        // Set the current time + time allowed to be busy
        void SetCompletionTimeBusy(int msTime = 2000)
            { m_msCompletionTimeBusy = msTime ? (msTime + GetNowMilliSeconds()) : 0;}

        bool GetCompletionTimeBusy()
            {   // if a time has been set, return diff(previvouly set future time - now time).
                if (m_msCompletionTimeBusy)
                    return (m_msCompletionTimeBusy > GetNowMilliSeconds());
                return false;
            }

        // Default didModify max busy time allowed is 2 secs
        // Set the current time + time allowed to be busy
        // Cleared on receiving textDocument/publishDiagnostics
        void SetDidChangeTimeBusy(int msTime = 2000)
            { m_msDidChangeTimeBusy = msTime ? (msTime + GetNowMilliSeconds()) : 0;}

        bool GetDidChangeTimeBusy()
            {   // if a time has been set, return diff(previvouly set future time - now time).
                if (m_msDidChangeTimeBusy)
                {
                    //wxString msg = (m_msDidChangeTimeBusy > GetNowMilliSeconds())?"true":"false";
                    //Manager::Get()->GetLogManager()->DebugLog(LSP_GetTimeHMSM() +" DidChangeTime Waiting:"+ msg);
                    return (m_msDidChangeTimeBusy > GetNowMilliSeconds());
                }
                return false;
            }

        MapMessageHandler MapMsgHndlr; //LSP output to our input thread
        std::thread* pInputThread = nullptr;

        int  GetCompilationDatabaseEntry(wxArrayString& returnArray, cbProject* pProject, wxString filename);
        void UpdateCompilationDatabase(cbProject* pProject, wxString filename);
        bool AddFileToCompileDBJson(cbProject* pProject, ProjectBuildTarget* pTarget, const wxString& argFullFilePath, json* pJson);    //(ph 2021/01/4)
        wxArrayString GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf) const ;
        size_t GetCompilerDriverIncludesByFile(wxArrayString& resultArray, cbProject* pProject, wxString filename);
        wxString CreateLSPClientLogName(int pid, const cbProject* pProject); //(ph 2021/02/12)

        // \brief Check if the command line is too long for the current running system and create a response file if necessary
        //
        // If the command line (executableCmd) used to execute a command is longer than the operating system limit
        // this function breaks the command line at the limit of the OS and generates a response file in the path folder.
        // Then it replaces the original command line with a constructed command line with the added response file and a pre-appended '@' sign.
        // This is the common marker for response files for many compilers.
        // The command is spit, so that the final command with the response file does not exceed the command line length limit.
        // The filename of the response file is generated from the executable name (string part until the first space from left)
        // and the base name. This name has to be unique.
        // The length limit of the operating system can be overwritten with the CB_COMMAND_LINE_MAX_LENGTH defined during compile time
        // If it is not defined at compile time reasonable limits for windows and UNIX are used.
        //
        // For example:
        //  mingw32-gcc.exe -i this/is/in/the/line -i this/is/longer/then/the/operating/system/supports
        //
        // gets transformed to
        //  mingw32-gcc.exe -i this/is/in/the/lime -i @path/to/response/file.respfile
        //
        // \param[in,out] executableCmd The command line input to check. Returns the modified command line if it was too long
        // \param[in,out] outputCommandArray The command queue. Some logging information is added so it can appear in the output log
        // \param[in] basename A base name, used to create a unique name for the response file
        // \param[in] path Base path where the response file is created
        void CheckForTooLongCommandLine(wxString& executableCmd, wxArrayString& outputCommandArray, const wxString& basename ,const wxString& path) const;

        void CreateDiagnosticsLog(); //Formated Diagnostic messages from LSP server

        // Number to append to log filename to differentiate logs
        int  GetLogIndex(const wxString& logRequest); //(ph 2021/01/14)
        // Set when LSP responds to the initialize request
        bool GetLSP_Initialized() {return m_LSP_initialized;}
        void SetLSP_Initialized(bool trueOrFalse) {m_LSP_initialized = trueOrFalse;}
        // Return ptr to server process for this client instance
        IProcess*  GetLSP_Server(){return pServerProcess;}

        /* Get the Request/Response id number from the LSP message header*/
        wxString GetRRIDvalue(wxString& lspHdrString);

    private:
        std::map<wxString, wxString> m_LSP_LastRequestPerFile;
    public:
        void SetLastLSP_Request(wxString filename, wxString lspRequest)
            {m_LSP_LastRequestPerFile[filename] = lspRequest;}
        wxString GetLastLSP_Request(wxString filename)
            {return m_LSP_LastRequestPerFile[filename];}

    private:
        std::map<wxString,int> m_LSP_CurrBackgroundFilesParsing;
    public:
        void LSP_AddToServerFilesParsing(wxString filename)
            { m_LSP_CurrBackgroundFilesParsing[filename] = GetNowMilliSeconds(); }
        void LSP_RemoveFromServerFilesParsing(wxString filename)
            {   m_LSP_CurrBackgroundFilesParsing.erase(filename); }
        size_t LSP_GetServerFilesParsingCount(){return m_LSP_CurrBackgroundFilesParsing.size();}
        int LSP_GetServerFilesParsingStartTime(wxString filename)
            {
                std::map<wxString,int>::iterator it;
                if ( m_LSP_CurrBackgroundFilesParsing.find(filename) == m_LSP_CurrBackgroundFilesParsing.end())
                    return 0;
              return m_LSP_CurrBackgroundFilesParsing[filename];
            }
        int LSP_GetServerFilesParsingDurationTime(wxString filename)
        {
            int startTime = LSP_GetServerFilesParsingStartTime(filename);
            if (not startTime) return 0;
            return GetDurationMilliSeconds(startTime);
        }
        bool IsServerFilesParsing(wxString filename)
        {
            if ( m_LSP_CurrBackgroundFilesParsing.find(filename) != m_LSP_CurrBackgroundFilesParsing.end())
                return true;
            return false;
        }
        void ListenForSavedFileMethod();
        void SetSaveFileEventOccured(wxCommandEvent& event);
        bool GetSaveFileEventOccured();

    public:
        // Return PID of the server process
        long       GetLSP_Server_PID(){return processServerPID;}

        // Used as the event id for this client
        int        GetLSP_EventID(){return idLSP_Process;}
        // true when the client for this project has received the initialize response
        bool       GetLSP_Initialized(cbProject* pProject)
                    { return GetLSP_Initialized(); }
        // true when the editor/project has received the first LSP response
        bool       GetLSP_Initialized(cbEditor* pEditor)
                    { return GetLSP_Initialized(); }
        // ptr to the server error/diagnostics log for this client
        LSPDiagnosticsResultsLog* LSP_GetLog() { return m_pDiagnosticsLog;}
        // ID to use when defining clinet/server events
        void       SetLSP_UserEventID(int id) {LSP_UserEventID = id;}
        int        GetLSP_UserEventID() {return LSP_UserEventID;}
        // The project respresented by this client/server
        void       SetCBProject(cbProject* pProject) {m_pCBProject = pProject;}
        cbProject* GetCBProject(){return m_pCBProject;}

        // ptr to this client instance
        ProcessLanguageClient* pLSPClient;

        // For this project, a map containing cbEditor*(key), value: tuple<LSP serverFileOpenStatus, caretPosition, isParsed>
        #define EDITOR_STATUS_IS_OPEN 0         // editors file is open in server
        #define EDITOR_STATUS_CARET_POSITION 1  //eg., 1234
        #define EDITOR_STATUS_READY 2           //Is parsed
        #define EDITOR_STATUS_MODIFIED 3        //Is modified
        typedef std::tuple<bool,int,bool,bool> LSP_EditorStatusTuple; //fileOpenInServer, editorPosn, editor is ready, editor is modified
        const LSP_EditorStatusTuple emptyEditorStatus = LSP_EditorStatusTuple(false,0,false,false);
        std::map<cbEditor*,LSP_EditorStatusTuple> m_LSP_EditorStatusMap;

        // ----------------------------------------------------------------------------
        LSP_EditorStatusTuple GetLSP_EditorStatus(cbEditor* pEditor)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "GetLSP_EditorStatus():null pEditor parm");
            if (not pEditor) return emptyEditorStatus;
            if (m_LSP_EditorStatusMap.find(pEditor) != m_LSP_EditorStatusMap.end())
                return m_LSP_EditorStatusMap[pEditor];
            else return emptyEditorStatus;
        }

        // ----------------------------------------------------------------------------
        void SetLSP_EditorStatus(cbEditor* pEditor, LSP_EditorStatusTuple LSPeditorStatus)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return;
            // Only new "didOPen" editors are added to the map
            //if ((m_LSP_EditorStatusMap.find(pEditor) != m_LSP_EditorStatusMap.end())
            //    and (std::get<EDITOR_STATUS_LSPID>(LSPeditorStatus).Contains("textDocument/didOpen")) )

            m_LSP_EditorStatusMap[pEditor] = LSPeditorStatus;
        }
        // ----------------------------------------------------------------------------
        void SetLSP_EditorRemove(cbEditor* pEditor)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return;
            if (m_LSP_EditorStatusMap.find(pEditor) != m_LSP_EditorStatusMap.end())
                m_LSP_EditorStatusMap.erase(pEditor);
        }
        // ----------------------------------------------------------------------------
        bool GetLSP_IsEditorModified(cbEditor* pEditor)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return false;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            if ( std::get<EDITOR_STATUS_MODIFIED>(edStatus) == true )
                return true;
            return false;
        }
        // ----------------------------------------------------------------------------
        void SetLSP_EditorModified(cbEditor* pEditor, bool trueOrFalse)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            std::get<EDITOR_STATUS_MODIFIED>(edStatus) = trueOrFalse;
            SetLSP_EditorStatus(pEditor, edStatus);
        }
        // ----------------------------------------------------------------------------
        bool GetLSP_IsEditorParsed(cbEditor* pEditor)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return false;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            if ( std::get<EDITOR_STATUS_READY>(edStatus) == true )
                return true;
            return false;
        }
        // ----------------------------------------------------------------------------
        void SetLSP_EditorIsParsed(cbEditor* pEditor, bool trueOrFalse)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            std::get<EDITOR_STATUS_READY>(edStatus) = trueOrFalse;
            SetLSP_EditorStatus(pEditor, edStatus);
        }
        // ----------------------------------------------------------------------------
        void SetLSP_EditorIsOpen(cbEditor* pEditor, bool trueOrFalse)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            std::get<EDITOR_STATUS_IS_OPEN>(edStatus) = trueOrFalse;
            SetLSP_EditorStatus(pEditor, edStatus);
        }
        // ----------------------------------------------------------------------------
        bool GetLSP_EditorIsOpen(cbEditor* pEditor)
        // ----------------------------------------------------------------------------
        {
            cbAssertNonFatal(pEditor && "null pEditor");
            if (not pEditor) return false;
            LSP_EditorStatusTuple edStatus = GetLSP_EditorStatus(pEditor);
            if (std::get<EDITOR_STATUS_IS_OPEN>(edStatus) )
                return true;
            return false;
        }
    // ------------------------------------------------------------------------
    //Client callbacks             //(ph 2021/02/9)
    // ------------------------------------------------------------------------
    typedef void(ProcessLanguageClient::*LSP_ClientFnc)(cbEditor*);
    typedef std::map<cbEditor*, LSP_ClientFnc> LSP_ClientCallBackMap;
    LSP_ClientCallBackMap m_LSPClientCallBackSinks;

    // ----------------------------------------------------------------
    void SetLSP_ClientCallBack(cbEditor* pEditor, LSP_ClientFnc callback)
    // ----------------------------------------------------------------
    {
        m_LSPClientCallBackSinks.insert(std::pair<cbEditor*,LSP_ClientFnc>(pEditor,callback));
        wxWakeUpIdle();
    }

    // Map of include files from the compiler and added to the compile command line of compile_commands.json
    // Map of key:compilerID string, value:a string containing concatenated "-IcompilerIncludeFile"s
    // used by UpdateCompilationDatabase() avoiding unnecessary wxExecutes()'s
    std::map<wxString, wxString> CompilerDriverIncludesMap;

    wxFFile lspClientLogFile;
    wxFFile lspServerLogFile;

    explicit ProcessLanguageClient(const cbProject* pProject, const char* program = "", const char* arguments = "");
            ~ProcessLanguageClient() override;

    bool Has_LSPServerProcess();
    void OnLSP_stderr(clProcessEvent& event);
    void OnLSP_ResponseStdOut(clProcessEvent& event);
    void OnLSP_Terminated(wxCommandEvent& event_pipedprocess_terminated);
    void SkipLine();
    void SkipToJsonData();
    int  ReadLSPinputLength();
    void ReadLSPinput(int length, std::string &out);
    bool WriteHdr(const std::string &in);
    bool readJson(json &json) override;
    bool writeJson(json &json) override;
    //-void OnLSP_Response(wxCommandEvent& event);
    void OnLSP_Response(wxThreadEvent& event);
    void OnIDMethod(wxCommandEvent& event);
    void OnIDResult(wxCommandEvent& event);
    void OnIDError(wxCommandEvent& event);
    void OnMethodParams(wxCommandEvent& event);
    void LSP_Shutdown();
    void OnLSP_Idle(wxIdleEvent& event);

    void LSP_Initialize(cbProject* pProject);
    bool LSP_DidOpen(cbEditor* pcbEd);
    bool LSP_DidOpen(wxString filename, cbProject* pProject); //(ph 2021/04/10)
    void LSP_DidClose(cbEditor* pcbEd);
    void LSP_DidClose(wxString filename, cbProject* pProject);
    void LSP_DidSave(cbEditor* pcbEd);
    void LSP_GoToDefinition(cbEditor* pcbEd, int edCaretPosition, size_t id=0);
    void LSP_GoToDeclaration(cbEditor* pcbEd, int edCaretPosition, size_t id=0);
    void LSP_FindReferences(cbEditor* pEd, int caretPosn);
    void LSP_RequestRename(cbEditor* pEd, int argCaretPosition, wxString newName);        //(ph 2021/10/12)
    void LSP_RequestSymbols(cbEditor* pEd, size_t id=0);
    void LSP_RequestSymbols(wxString filename, cbProject* pProject, size_t id=0);
    void LSP_RequestSemanticTokens(cbEditor* pEd, size_t id=0); //(ph 2021/03/16)
    void LSP_DidChange(cbEditor* pEd);
    void LSP_Completion(cbEditor* pEd);
    //-void LSP_Completion(bool isAuto, cbEditor* pEd, int& tknStart, int& tknEnd);
    void LSP_Hover(cbEditor* pEd, int posn);
    void LSP_SignatureHelp(cbEditor* pEd, int posn);

    void writeClientLog(wxString logmsg);
    void writeServerLog(wxString logmsg);
    wxString GetTime();
    std::string LSP_GetTimeHMSM(); //Get time in hours minute second milliseconds
    std::string GetTime_in_HH_MM_SS_MMM();
    size_t GetNowMilliSeconds();
    size_t GetDurationMilliSeconds(int startMillis);
    //-void   SetParseTimeStart(cbEditor* pEditor) { m_ParseStartMillsTODmap[pEditor] = GetNowMilliSeconds();}
    //-int    GetParseTimeStart(cbEditor* pEditor) { return m_ParseStartMillsTODmap[pEditor];}

    bool ClientProjectOwnsFile(cbEditor* pcbEd, bool notify=true);
    cbProject* GetProjectFromEditor(cbEditor* pcbEd);

    int   GetCompileCommandsChangedTime()
        {return m_LSP_CompileCommandsChangedTime;}
    void  SetCompileCommandsChangedTime(bool trueOrFalse)
        {
            if (trueOrFalse)
                m_LSP_CompileCommandsChangedTime = GetNowMilliSeconds();
            else m_LSP_CompileCommandsChangedTime = 0;
        }

    // array of user desinated log messages to ignore
    wxArrayString& GetLSP_IgnoredDiagnostics()
    {
        // Read persistent config array of ignored messages to the config
        ConfigManager* pCfgMgr = Manager::Get()->GetConfigManager("clangd_client");
        m_LSP_aIgnoredDiagnostics.Clear();
        pCfgMgr->Read("ignored_diagnostics", &m_LSP_aIgnoredDiagnostics);

        return m_LSP_aIgnoredDiagnostics;
    }

    cbStyledTextCtrl* GetNewHiddenEditor(const wxString& filename);              //(ph 2021/04/10)

    // Verify that an event handler is still in the chain of event handlers
    wxEvtHandler* FindEventHandler(wxEvtHandler* pEvtHdlr)
    {
        wxEvtHandler* pFoundEvtHdlr =  Manager::Get()->GetAppWindow()->GetEventHandler();

        while (pFoundEvtHdlr != nullptr)
        {
            if (pFoundEvtHdlr == pEvtHdlr)
                return pFoundEvtHdlr;
            pFoundEvtHdlr = pFoundEvtHdlr->GetNextHandler();
        }
        return nullptr;
    }

};
#endif //LSP_CLIENT_H
