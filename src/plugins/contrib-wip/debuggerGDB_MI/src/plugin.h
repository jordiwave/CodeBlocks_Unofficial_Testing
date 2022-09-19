/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef _DEBUGGER_GDB_MI_PLUGIN_H_
#define _DEBUGGER_GDB_MI_PLUGIN_H_

// System and library includes
#include <tinyxml2.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    // For compilers that support precompilation, includes <wx/wx.h>
    #include <wx/wx.h>
#endif
#include <memory>

// CB includes
#include <cbplugin.h> // for "class cbPlugin"

// GDB includes
#include "cmd_queue.h"
#include "definitions.h"
#include "events.h"
#include "gdb_executor.h"
#include "gdb_logger.h"
#include "remotedebugging.h"

class TextCtrlLogger;
class Compiler;

namespace dbg_mi
{

class DebuggerConfiguration;

} // namespace dbg_mi

class Debugger_GDB_MI : public cbDebuggerPlugin
{
    public:
        /** Constructor. */
        Debugger_GDB_MI();
        /** Destructor. */
        virtual ~Debugger_GDB_MI();

    public:
        virtual void SetupToolsMenu(wxMenu & menu) override;
        virtual bool ToolMenuEnabled() const override
        {
            return true;
        }

        virtual bool SupportsFeature(cbDebuggerFeature::Flags flag) override;
        virtual cbDebuggerConfiguration * LoadConfig(const ConfigManagerWrapper & config) override;
        dbg_mi::DebuggerConfiguration & GetActiveConfigEx();
        cbConfigurationPanel * GetProjectConfigurationPanel(wxWindow * parent, cbProject * project) override;
        virtual bool Debug(bool breakOnEntry) override;
        virtual void Continue() override;
        virtual bool RunToCursor(const wxString & filename, int line, const wxString & line_text) override;
        virtual void SetNextStatement(const wxString & filename, int line) override;
        virtual void Next() override;
        virtual void NextInstruction() override;
        virtual void StepIntoInstruction() override;
        virtual void Step() override;
        virtual void StepOut() override;
        virtual void Break() override;
        virtual void Stop() override;
        virtual bool IsRunning() const override;
        virtual bool IsStopped() const override;
        virtual bool IsBusy() const override;
        virtual int GetExitCode() const override;
        void SetExitCode(int code)
        {
            m_exit_code = code;
        }

        // stack frame calls;
        virtual int GetStackFrameCount() const override;
        virtual cb::shared_ptr<const cbStackFrame> GetStackFrame(int index) const override;
        virtual void SwitchToFrame(int number) override;
        virtual int GetActiveStackFrame() const override;

        // breakpoints calls
        virtual cb::shared_ptr<cbBreakpoint> AddBreakpoint(const wxString & filename, int line) override;
        cb::shared_ptr<cbBreakpoint> AddBreakpoint(cb::shared_ptr<dbg_mi::GDBBreakpoint> bp);
        virtual cb::shared_ptr<cbBreakpoint> AddDataBreakpoint(const wxString & dataExpression) override;
        virtual int GetBreakpointsCount() const override;
        virtual cb::shared_ptr<cbBreakpoint> GetBreakpoint(int index) override;
        virtual cb::shared_ptr<const cbBreakpoint> GetBreakpoint(int index) const override;
        virtual void UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) override;
        virtual void DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) override;
        virtual void DeleteAllBreakpoints() override;
        virtual void ShiftBreakpoint(int index, int lines_to_shift) override;
        virtual void EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable) override;

        // threads
        virtual int GetThreadsCount() const override;
        virtual cb::shared_ptr<const cbThread> GetThread(int index) const override;
        virtual bool SwitchToThread(int thread_number) override;

        // watches
        virtual cb::shared_ptr<cbWatch> AddWatch(const wxString & symbol, bool update) override;
        virtual cb::shared_ptr<cbWatch> AddWatch(dbg_mi::GDBWatch * watch, cb_unused bool update);
        cb::shared_ptr<cbWatch> AddMemoryRange(uint64_t address, uint64_t size, const wxString & symbol, bool update) override;
        void AddTooltipWatch(const wxString & symbol, wxRect const & rect);
        virtual void DeleteWatch(cb::shared_ptr<cbWatch> watch) override;
        virtual bool HasWatch(cb::shared_ptr<cbWatch> watch) override;
        bool IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch);
        virtual void ShowWatchProperties(cb::shared_ptr<cbWatch> watch) override;
        virtual bool SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value) override;
        virtual void ExpandWatch(cb::shared_ptr<cbWatch> watch) override;
        virtual void CollapseWatch(cb::shared_ptr<cbWatch> watch) override;
        virtual void UpdateWatch(cb::shared_ptr<cbWatch> watch) override;
        virtual void SendCommand(const wxString & cmd, bool debugLog) override;
        virtual void AttachToProcess(const wxString & pid) override;
        virtual void DetachFromProcess() override;
        virtual bool IsAttachedToProcess() const override;
        virtual void GetCurrentPosition(wxString & filename, int & line) override;
        virtual void RequestUpdate(DebugWindows window) override;
        virtual void OnValueTooltip(const wxString & token, const wxRect & evalRect) override;
        virtual bool ShowValueTooltip(int style) override;

        void StripQuotes(wxString & str);
        void ConvertToGDBFriendly(wxString & str);
        void ConvertToGDBDirectory(wxString & str, wxString base = "", bool relative = true);
        wxArrayString ParseSearchDirs(cbProject * project);
        TiXmlElement * GetElementForSaving(cbProject & project, const char * elementsToClear);
        void SetSearchDirs(cbProject & project, const wxArrayString & dirs);

        dbg_mi::RemoteDebuggingMap ParseRemoteDebuggingMap(cbProject & project);
        void SetRemoteDebuggingMap(cbProject & project, const dbg_mi::RemoteDebuggingMap & map);

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
        virtual void OnAttachReal() override;

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnReleaseReal(bool appShutDown) override;

    protected:
        virtual void ConvertDirectory(wxString & /*str*/, wxString /*base*/, bool /*relative*/) override;
        virtual cbProject * GetProject() override
        {
            return m_pProject;
        }
        virtual void ResetProject() override
        {
            m_pProject = NULL;
        }
        virtual void CleanupWhenProjectClosed(cbProject * project) override;
        virtual bool CompilerFinished(bool compilerFailed, StartType startType) override;

    public:
        void UpdateWhenStopped();
        void UpdateOnFrameChanged(bool wait);
        dbg_mi::GDBCurrentFrame & GetGDBCurrentFrame()
        {
            return m_current_frame;
        }

        dbg_mi::GDBExecutor & GetGDBExecutor()
        {
            return m_executor;
        }

        dbg_mi::LogPaneLogger * GetGDBLogger()
        {
            return m_pLogger;
        }
    private:
        DECLARE_EVENT_TABLE();

        void OnGDBOutput(wxCommandEvent & event);
        void OnGDBError(wxCommandEvent & event);
        void OnGDBTerminated(wxCommandEvent & event);
        void OnTimer(wxTimerEvent & event);
        void OnIdle(wxIdleEvent & event);
        void OnMenuInfoCommandStream(wxCommandEvent & event);
        int LaunchDebugger(cbProject * project, wxString const & debugger, wxString const & debuggee, wxString const & args,
                           wxString const & working_dir, int pid, bool console, StartType start_type);
        void AddStringCommand(wxString const & command);
        void DoSendCommand(const wxString & cmd);
        void RunQueue();
        void ParseOutput(wxString const & str);
        bool SelectCompiler(cbProject & project, Compiler *& compiler, ProjectBuildTarget *& target, long pid_to_attach);
        int StartDebugger(cbProject * project, StartType startType);
        void CommitBreakpoints(bool force);
        void CommitRunCommand(wxString const & command);
        void CommitWatches();
        void KillConsole();
        void OnProjectOpened(CodeBlocksEvent & event);
        void OnProjectClosed(CodeBlocksEvent & event);
        bool SaveStateToFile(cbProject * prj);
        bool LoadStateFromFile(cbProject * prj);
        void DoWatches();

    private:
        wxTimer m_timer_poll_debugger;
        cbProject * m_pProject;

        dbg_mi::GDBExecutor m_executor;
        dbg_mi::ActionsMap  m_actions;
        dbg_mi::LogPaneLogger * m_pLogger;
        dbg_mi::GDBBreakpointsContainer m_breakpoints;
        dbg_mi::GDBBreakpointsContainer m_temporary_breakpoints;
        dbg_mi::GDBBacktraceContainer m_backtrace;
        dbg_mi::GDBThreadsContainer m_threads;
        dbg_mi::GDBWatchesContainer m_watches;

        cb::shared_ptr<dbg_mi::GDBWatch> m_WatchLocalsandArgs;

        dbg_mi::GDBMemoryRangeWatchesContainer m_memoryRanges;
        dbg_mi::GDBMapWatchesToType m_mapWatchesToType;
        dbg_mi::GDBTextInfoWindow * m_command_stream_dialog;
        dbg_mi::GDBCurrentFrame m_current_frame;
        int m_exit_code;
        int m_console_pid;
        int m_pid_attached;
        bool m_hasStartUpError;
};

#endif // _DEBUGGER_GDB_MI_PLUGIN_H_
