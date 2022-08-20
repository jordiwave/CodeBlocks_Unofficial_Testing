/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef _DEBUGGER_DAP_PLUGIN_H_
#define _DEBUGGER_DAP_PLUGIN_H_

// System and library includes
#include <memory>
#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/wxprec.h>

// CB includes
#include <cbplugin.h> // for "class cbPlugin"
#include <tinyxml2.h>

// Debugger includes
#include "debugger_logger.h"
#include "definitions.h"

// DAP includes
#include "Client.hpp"
#include "Process.hpp"

class TextCtrlLogger;
class Compiler;

namespace dbg_DAP
{
class DebuggerConfiguration;

} // namespace dbg_DAP

class Debugger_DAP : public cbDebuggerPlugin
{
    public:
        /** Constructor. */
        Debugger_DAP();
        /** Destructor. */
        virtual ~Debugger_DAP();

    public:
        virtual void SetupToolsMenu(wxMenu & menu);
        virtual bool ToolMenuEnabled() const
        {
            return true;
        }

        virtual bool SupportsFeature(cbDebuggerFeature::Flags flag);
        virtual cbDebuggerConfiguration * LoadConfig(const ConfigManagerWrapper & config);
        dbg_DAP::DebuggerConfiguration & GetActiveConfigEx();
        cbConfigurationPanel * GetProjectConfigurationPanel(wxWindow * parent, cbProject * project);
        virtual bool Debug(bool breakOnEntry);
        virtual void Continue();
        virtual bool RunToCursor(const wxString & filename, int line, const wxString & line_text);
        virtual void SetNextStatement(const wxString & filename, int line);
        virtual void Next();
        virtual void NextInstruction();
        virtual void StepIntoInstruction();
        virtual void Step();
        virtual void StepOut();
        virtual void Break();
        virtual void Stop();
        virtual bool IsRunning() const;
        virtual bool IsStopped() const;
        virtual bool IsBusy() const;
        virtual int GetExitCode() const;
        void SetExitCode(int code)
        {
            m_exit_code = code;
        }

        // stack frame calls;
        virtual int GetStackFrameCount() const;
        virtual cb::shared_ptr<const cbStackFrame> GetStackFrame(int index) const;
        virtual void SwitchToFrame(int number);
        virtual int GetActiveStackFrame() const;

        // breakpoints calls
        virtual cb::shared_ptr<cbBreakpoint> AddBreakpoint(const wxString & filename, int line);
        cb::shared_ptr<cbBreakpoint> UpdateOrAddBreakpoint(const wxString & filename, const int line, const int id);
        //        cb::shared_ptr<cbBreakpoint> AddBreakpoint(cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp);
        virtual cb::shared_ptr<cbBreakpoint> AddDataBreakpoint(const wxString & dataExpression);
        virtual int GetBreakpointsCount() const;
        virtual cb::shared_ptr<cbBreakpoint> GetBreakpoint(int index);
        virtual cb::shared_ptr<const cbBreakpoint> GetBreakpoint(int index) const;
        cb::shared_ptr<cbBreakpoint> GetBreakpointByID(int id);
        virtual void UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint);
        virtual void DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint);
        virtual void DeleteAllBreakpoints();
        virtual void ShiftBreakpoint(int index, int lines_to_shift);
        virtual void EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable);

        // threads
        virtual int GetThreadsCount() const;
        virtual cb::shared_ptr<const cbThread> GetThread(int index) const;
        virtual bool SwitchToThread(int thread_number);

        // watches
        void UpdateDAPWatches(int updateType);
        virtual cb::shared_ptr<cbWatch> AddWatch(const wxString & symbol, bool update);
        virtual cb::shared_ptr<cbWatch> AddWatch(dbg_DAP::DAPWatch * watch, cb_unused bool update);
        cb::shared_ptr<cbWatch> AddMemoryRange(uint64_t address, uint64_t size, const wxString & symbol, bool update);
        void AddTooltipWatch(const wxString & symbol, wxRect const & rect);
        virtual void DeleteWatch(cb::shared_ptr<cbWatch> watch);
        virtual bool HasWatch(cb::shared_ptr<cbWatch> watch);
        bool IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch);
        virtual void ShowWatchProperties(cb::shared_ptr<cbWatch> watch);
        virtual bool SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value);
        virtual void ExpandWatch(cb::shared_ptr<cbWatch> watch);
        virtual void CollapseWatch(cb::shared_ptr<cbWatch> watch);
        virtual void UpdateWatch(cb::shared_ptr<cbWatch> watch);

        //
        virtual void SendCommand(const wxString & cmd, bool debugLog) {};

        // Preocess
        virtual void AttachToProcess(const wxString & pid);
        virtual void DetachFromProcess();
        virtual bool IsAttachedToProcess() const;
        virtual void GetCurrentPosition(wxString & filename, int & line);

        // Misc
        virtual void RequestUpdate(DebugWindows window);
        virtual void OnValueTooltip(const wxString & token, const wxRect & evalRect);
        virtual bool ShowValueTooltip(int style);

        // Conversion
        void StripQuotes(wxString & str);
        void ConvertToDAPFriendly(wxString & str);
        void ConvertToDAPDirectory(wxString & str, wxString base = "", bool relative = true);
        wxArrayString ParseSearchDirs(cbProject * project);
        //        TiXmlElement* GetElementForSaving(cbProject &project, const char *elementsToClear);
        //        void SetSearchDirs(cbProject &project, const wxArrayString &dirs);

        //        dbg_DAP::RemoteDebuggingMap ParseRemoteDebuggingMap(cbProject &project);
        //        void SetRemoteDebuggingMap(cbProject &project, const dbg_DAP::RemoteDebuggingMap &map);

    protected:
        /** Any descendent plugin should override this virtual method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        virtual void OnAttachReal();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnReleaseReal(bool appShutDown);

    protected:
        virtual void ConvertDirectory(wxString & /*str*/, wxString /*base*/, bool /*relative*/);
        virtual cbProject * GetProject()
        {
            return m_pProject;
        }
        virtual void ResetProject()
        {
            m_pProject = NULL;
        }
        void OnProjectOpened(CodeBlocksEvent & event);
        virtual void CleanupWhenProjectClosed(cbProject * project);
        virtual bool CompilerFinished(bool compilerFailed, StartType startType);

    public:
        dbg_DAP::LogPaneLogger * GetDAPLogger()
        {
            return m_pLogger;
        }
    private:
        DECLARE_EVENT_TABLE();

        void OnTimer(wxTimerEvent & event);
        void OnIdle(wxIdleEvent & event);
        void OnMenuInfoCommandStream(wxCommandEvent & event);
        void LaunchDAPDebugger(cbProject * project, const wxString & dap_debugger, const wxString & dap_port_number);
        int LaunchDebugger(cbProject * project,
                           const wxString & dap_debugger,
                           const wxString & debuggee,
                           const wxString & dap_port_number,
                           const wxString & working_dir,
                           int pid,
                           bool console,
                           StartType start_type);
        bool SelectCompiler(cbProject & project, Compiler *& compiler, ProjectBuildTarget *& target, long pid_to_attach);
        int StartDebugger(cbProject * project, StartType startType);
        void CreateStartBreakpoints(bool force);
        void CreateStartWatches();
        bool SaveStateToFile(cbProject * prj);
        bool LoadStateFromFile(cbProject * prj);
        void DoWatches();
        void UpdateDebugDialogs(bool bClearAllData);

    private:
        wxTimer m_timer_poll_debugger;
        cbProject * m_pProject;

        //        dbg_DAP::DAPExecutor m_executor;
        //        dbg_DAP::ActionsMap  m_actions;
        dbg_DAP::LogPaneLogger * m_pLogger;
        dbg_DAP::DAPThreadsContainer m_threads;


        //        dbg_DAP::DAPMemoryRangeWatchesContainer m_memoryRanges;
        //        dbg_DAP::DAPTextInfoWindow * m_command_stream_dialog;
        int m_exit_code;
        //        int m_console_pid;
        //        int m_pid_attached;
        bool m_hasStartUpError;


    private:
        dap::Client m_dapClient;
        long m_dapPid;
        std::vector<wxString> m_DAP_DebuggeeStartCMD;
        dap::Process * m_DAPTerminalProcess = nullptr;

        enum eDAPState
        {
            NotConnected = 0,
            Connected,
            Stopped,
            Running
        };
        eDAPState  DAPDebuggerState;

        // breakpoints
        dbg_DAP::DAPBreakpointsContainer m_breakpoints;
        std::map<wxString, std::vector<cb::shared_ptr<dbg_DAP::DAPBreakpoint>>> m_map_filebreakpoints;
        dbg_DAP::DAPBreakpointsContainer m_temporary_breakpoints;
        cb::shared_ptr<dbg_DAP::DAPBreakpoint> FindBreakpoint(const cbProject * project, const wxString & filename, const int line);
        void UpdateMapFileBreakPoints(const wxString & filename, cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp, bool bAddBreakpoint);
        void UpdateDAPSetBreakpointsByFileName(const wxString & filename);

        // Stack
        dbg_DAP::DAPCurrentFrame m_current_frame;
        dbg_DAP::DAPBacktraceContainer m_backtrace;

        // Watches
        dbg_DAP::DAPWatchesContainer m_watches;
        dbg_DAP::DAPMapWatchesToType m_mapWatchesToType;
        cb::shared_ptr<dbg_DAP::DAPWatch> m_WatchLocalsandArgs;
        std::vector<dap::Variable> m_stackdapvariables;

        // misc
        void DAPDebuggerResetData(bool bClearAllData);
        void OnProcessBreakpointData(const wxString & brkDescription);

        /// Dap events
        void OnStopped(DAPEvent & event);
        void OnStackTrace(DAPEvent & event);
        void OnScopes(DAPEvent & event);
        void OnVariables(DAPEvent & event);
        void OnInitializedEvent(DAPEvent & event);
        void OnInitializeResponse(DAPEvent & event);
        void OnConfigurationDoneResponse(DAPEvent & event);
        void OnExited(DAPEvent & event);
        void OnTerminated(DAPEvent & event);
        void OnOutput(DAPEvent & event);
        void OnBreakpointLocations(DAPEvent & event);
        void OnConnectionError(DAPEvent & event);
        void OnBreakpointDataSet(DAPEvent & event);
        void OnBreakpointFunctionSet(DAPEvent & event);
        void OnLaunchResponse(DAPEvent & event);
        void OnRunInTerminalRequest(DAPEvent & event);
        void OnDapLog(DAPEvent & event);
        void OnDapModuleEvent(DAPEvent & event);

        void OnTreadResponse(DAPEvent & event);
        void OnStopOnEntryEvent(DAPEvent & event);
        void OnProcessEvent(DAPEvent & event);
        void OnBreakpointEvent(DAPEvent & event);
        void OnCcontinuedEvent(DAPEvent & event);
        void OnDebugPYWaitingForServerEvent(DAPEvent & event);


        // DAP Capabilities
        // Configured for a default of false and updated in the DAP debugger capability response message

        // The debug adapter supports the 'configurationDone' request.
        bool supportsConfigurationDoneRequest = false;

        // The debug adapter supports function breakpoints.
        bool supportsFunctionBreakpoints = false;

        // The debug adapter supports conditional breakpoints.
        bool supportsConditionalBreakpoints = false;

        // The debug adapter supports breakpoints that break execution after a specified number of hits.
        bool supportsHitConditionalBreakpoints = false;

        // The debug adapter supports a (side effect free) evaluate request for data hovers.
        bool supportsEvaluateForHovers = false;

        // The debug adapter supports stepping back via the 'stepBack' and 'reverseContinue' requests.
        bool supportsStepBack = false;

        // The debug adapter supports setting a variable to a value.
        bool supportsSetVariable = false;

        // The debug adapter supports restarting a frame.
        bool supportsRestartFrame = false;

        // The debug adapter supports the 'gotoTargets' request.
        bool supportsGotoTargetsRequest = false;

        // The debug adapter supports the 'stepInTargets' request.
        bool supportsStepInTargetsRequest = false;

        // The debug adapter supports the 'completions' request.
        bool supportsCompletionsRequest = false;

        // The debug adapter supports the 'modules' request.
        bool supportsModulesRequest = false;

        // The debug adapter supports the 'restart' request. In this case a client should not implement 'restart' by terminating and relaunching the adapter but by calling the RestartRequest.
        bool supportsRestartRequest = false;

        // The debug adapter supports 'exceptionOptions' on the setExceptionBreakpoints request.
        bool supportsExceptionOptions = false;

        // The debug adapter supports a 'format' attribute on the stackTraceRequest, variablesRequest, and evaluateRequest.
        bool supportsValueFormattingOptions = false;

        // The debug adapter supports the 'exceptionInfo' request.
        bool supportsExceptionInfoRequest = false;

        // The debug adapter supports the 'terminateDebuggee' attribute on the 'disconnect' request.
        bool supportTerminateDebuggee = false;

        // The debug adapter supports the `suspendDebuggee` attribute on the `disconnect` request.
        bool supportSuspendDebuggee = false;

        // The debug adapter supports the delayed loading of parts of the stack, which requires that both the 'startFrame' and 'levels' arguments and an std::optional 'totalFrames' result of the 'StackTrace' request are supported.
        bool supportsDelayedStackTraceLoading = false;

        // The debug adapter supports the 'loadedSources' request.
        bool supportsLoadedSourcesRequest = false;

        // The debug adapter supports logpoints by interpreting the 'logMessage' attribute of the SourceBreakpoint.
        bool supportsLogPoints = false;

        // The debug adapter supports the 'terminateThreads' request.
        bool supportsTerminateThreadsRequest = false;

        // The debug adapter supports the 'setExpression' request.
        bool supportsSetExpression = false;

        // The debug adapter supports the 'terminate' request.
        bool supportsTerminateRequest = false;

        // The debug adapter supports data breakpoints.
        bool supportsDataBreakpoints = false;

        // The debug adapter supports the 'readMemory' request.
        bool supportsReadMemoryRequest = false;

        // The debug adapter supports the `writeMemory` request.
        bool supportsWriteMemoryRequest = false;

        // The debug adapter supports the 'disassemble' request.
        bool supportsDisassembleRequest = false;

        // The debug adapter supports the 'cancel' request.
        bool supportsCancelRequest = false;

        // The debug adapter supports the 'breakpointLocations' request.
        bool supportsBreakpointLocationsRequest = false;

        // The debug adapter supports the 'clipboard' context value in the 'evaluate' request.
        bool supportsClipboardContext = false;

        // The debug adapter supports stepping granularities (argument 'granularity') for the stepping requests.
        bool supportsSteppingGranularity = false;

        // The debug adapter supports adding breakpoints based on instruction references.
        bool supportsInstructionBreakpoints = false;

        // The debug adapter supports 'filterOptions' as an argument on the 'setExceptionBreakpoints' request.
        bool supportsExceptionFilterOptions = false;

        // The debug adapter supports the `singleThread` property on the execution requests (`continue`, `next`, `stepIn`, `stepOut`, `reverseContinue`, `stepBack`).
        bool supportsSingleThreadExecutionRequests = false;

        // Available exception filter options for the 'setExceptionBreakpoints' request.
        std::vector<dap::ExceptionBreakpointsFilter> vExceptionBreakpointFilters;

        // The set of additional module information exposed by the debug adapter.
        std::vector<dap::ColumnDescriptor> vAdditionalModuleColumns;

        // Checksum algorithms supported by the debug adapter.
        std::vector<dap::ChecksumAlgorithm> vSupportedChecksumAlgorithms;

        // The set of characters that should trigger completion in a REPL. If not specified, the UI should assume the '.' character.
        std::vector<wxString> vCompletionTriggerCharacters;
};

#endif // _DEBUGGER_DAP_PLUGIN_H_
