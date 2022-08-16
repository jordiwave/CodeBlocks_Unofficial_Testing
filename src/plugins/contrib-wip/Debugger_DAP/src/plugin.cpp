/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

// System include files
#include <algorithm>
#include <tinyxml2.h>

#include <wx/app.h>
#include <wx/event.h>
#include <wx/xrc/xmlres.h>
#include <wx/wxscintilla.h>
#ifndef __WX_MSW__
    #include <dirent.h>
    #include <stdlib.h>
#endif
#ifdef __MINGW64__
    #include <debugapi.h>
#endif // __MINGW64__

// CB include files (not DAP)
#include "sdk.h"
#include "cbdebugger_interfaces.h"
#include "cbproject.h"
#include "compilercommandgenerator.h"
#include "compilerfactory.h"
#include "configurationpanel.h"
#include "configmanager.h"
#include "infowindow.h"
#include "macrosmanager.h"
#include "manager.h"
#include "pipedprocess.h"
#include "projectmanager.h"

// DAP include files
#include "dlg_ProjectOptions.h"
#include "dlg_SettingsOptions.h"
#include "dlg_WatchEdit.h"
#include "debugger_logger.h"
#include "plugin.h"
#include "dap.hpp"

//XML file root tag for data
static const char * XML_CFG_ROOT_TAG = "Debugger_layout_file";

namespace
{
int const id_gdb_poll_timer = wxNewId();
int const id_menu_info_command_stream = wxNewId();

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
// this auto-registers the plugin
PluginRegistrant<Debugger_DAP> reg("debugger_dap");
}


namespace
{
wxString GetLibraryPath(const wxString & oldLibPath, Compiler * compiler, ProjectBuildTarget * target, cbProject * project)
{
    if (compiler && target)
    {
        wxString newLibPath;
        const wxString libPathSep = platform::windows ? ";" : ":";
        newLibPath << "." << libPathSep;
        CompilerCommandGenerator * generator = compiler->GetCommandGenerator(project);
        newLibPath << GetStringFromArray(generator->GetLinkerSearchDirs(target), libPathSep);
        delete generator;

        if (newLibPath.Mid(newLibPath.Length() - 1, 1) != libPathSep)
        {
            newLibPath << libPathSep;
        }

        newLibPath << oldLibPath;
        return newLibPath;
    }
    else
    {
        return oldLibPath;
    }
}

} // anonymous namespace

// events handling
BEGIN_EVENT_TABLE(Debugger_DAP, cbDebuggerPlugin)
    EVT_IDLE(Debugger_DAP::OnIdle)
    EVT_TIMER(id_gdb_poll_timer, Debugger_DAP::OnTimer)

    EVT_MENU(id_menu_info_command_stream, Debugger_DAP::OnMenuInfoCommandStream)
END_EVENT_TABLE()

// constructor
Debugger_DAP::Debugger_DAP() :
    cbDebuggerPlugin("DAP", "debugger_dap"),
    m_pProject(nullptr),
    m_dapPid(0),
    DAPDebuggerState(eDAPState::NotConnected)
{
    if (!Manager::LoadResource("debugger_dap.zip"))
    {
        NotifyMissingFile("debugger_dap.zip");
    }

    m_pLogger = new dbg_DAP::LogPaneLogger(this);
    // bind the client events
    m_dapClient.Bind(wxEVT_DAP_STOPPED_EVENT,                   &Debugger_DAP::OnStopped,               this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZED_EVENT,               &Debugger_DAP::OnInitializedEvent,      this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZE_RESPONSE,             &Debugger_DAP::OnInitializeResponse,    this);
    m_dapClient.Bind(wxEVT_DAP_EXITED_EVENT,                    &Debugger_DAP::OnExited,                this);
    m_dapClient.Bind(wxEVT_DAP_TERMINATED_EVENT,                &Debugger_DAP::OnTerminated,            this);
    m_dapClient.Bind(wxEVT_DAP_STACKTRACE_RESPONSE,             &Debugger_DAP::OnStackTrace,            this);
    m_dapClient.Bind(wxEVT_DAP_SCOPES_RESPONSE,                 &Debugger_DAP::OnScopes,                this);
    m_dapClient.Bind(wxEVT_DAP_VARIABLES_RESPONSE,              &Debugger_DAP::OnVariables,             this);
    m_dapClient.Bind(wxEVT_DAP_OUTPUT_EVENT,                    &Debugger_DAP::OnOutput,                this);
    m_dapClient.Bind(wxEVT_DAP_BREAKPOINT_LOCATIONS_RESPONSE,   &Debugger_DAP::OnBreakpointLocations,   this);
    m_dapClient.Bind(wxEVT_DAP_LOST_CONNECTION,                 &Debugger_DAP::OnConnectionError,       this);
    m_dapClient.Bind(wxEVT_DAP_SET_SOURCE_BREAKPOINT_RESPONSE,  &Debugger_DAP::OnBreakpointDataSet,     this);
    m_dapClient.Bind(wxEVT_DAP_SET_FUNCTION_BREAKPOINT_RESPONSE, &Debugger_DAP::OnBreakpointFunctionSet, this);
    m_dapClient.Bind(wxEVT_DAP_LAUNCH_RESPONSE,                 &Debugger_DAP::OnLaunchResponse,        this);
    m_dapClient.Bind(wxEVT_DAP_RUN_IN_TERMINAL_REQUEST,         &Debugger_DAP::OnRunInTerminalRequest,  this);
    m_dapClient.Bind(wxEVT_DAP_LOG_EVENT,                       &Debugger_DAP::OnDapLog,                this);
    m_dapClient.Bind(wxEVT_DAP_MODULE_EVENT,                    &Debugger_DAP::OnDapModuleEvent,        this);
    m_dapClient.Bind(wxEVT_DAP_CONFIGURARIONE_DONE_RESPONSE,    &Debugger_DAP::OnConfigurationDoneResponse,  this);
    m_dapClient.Bind(wxEVT_DAP_THREADS_RESPONSE,                &Debugger_DAP::OnTreadResponse,         this);
    m_dapClient.Bind(wxEVT_DAP_STOPPED_ON_ENTRY_EVENT,          &Debugger_DAP::OnStopOnEntryEvent,      this);
    m_dapClient.Bind(wxEVT_DAP_PROCESS_EVENT,                   &Debugger_DAP::OnProcessEvent,          this);
    m_dapClient.Bind(wxEVT_DAP_BREAKPOINT_EVENT,                &Debugger_DAP::OnBreakpointEvent,       this);
    m_dapClient.Bind(wxEVT_DAP_CONTINUED_EVENT,                 &Debugger_DAP::OnCcontinuedEvent,       this);
    m_dapClient.Bind(wxEVT_DAP_DEBUGPYWAITINGFORSERVER_EVENT,   &Debugger_DAP::OnDebugPYWaitingForServerEvent,  this);
    m_dapClient.SetWantsLogEvents(true); // send use log events
}

// destructor
Debugger_DAP::~Debugger_DAP()
{
    // unbind the client events
    m_dapClient.Unbind(wxEVT_DAP_STOPPED_EVENT,                   &Debugger_DAP::OnStopped,                 this);
    m_dapClient.Unbind(wxEVT_DAP_INITIALIZED_EVENT,               &Debugger_DAP::OnInitializedEvent,        this);
    m_dapClient.Unbind(wxEVT_DAP_INITIALIZE_RESPONSE,             &Debugger_DAP::OnInitializeResponse,      this);
    m_dapClient.Unbind(wxEVT_DAP_EXITED_EVENT,                    &Debugger_DAP::OnExited,                  this);
    m_dapClient.Unbind(wxEVT_DAP_TERMINATED_EVENT,                &Debugger_DAP::OnTerminated,              this);
    m_dapClient.Unbind(wxEVT_DAP_STACKTRACE_RESPONSE,             &Debugger_DAP::OnStackTrace,              this);
    m_dapClient.Unbind(wxEVT_DAP_SCOPES_RESPONSE,                 &Debugger_DAP::OnScopes,                  this);
    m_dapClient.Unbind(wxEVT_DAP_VARIABLES_RESPONSE,              &Debugger_DAP::OnVariables,               this);
    m_dapClient.Unbind(wxEVT_DAP_OUTPUT_EVENT,                    &Debugger_DAP::OnOutput,                  this);
    m_dapClient.Unbind(wxEVT_DAP_BREAKPOINT_LOCATIONS_RESPONSE,   &Debugger_DAP::OnBreakpointLocations,     this);
    m_dapClient.Unbind(wxEVT_DAP_LOST_CONNECTION,                 &Debugger_DAP::OnConnectionError,         this);
    m_dapClient.Unbind(wxEVT_DAP_SET_SOURCE_BREAKPOINT_RESPONSE,  &Debugger_DAP::OnBreakpointDataSet,       this);
    m_dapClient.Unbind(wxEVT_DAP_SET_FUNCTION_BREAKPOINT_RESPONSE, &Debugger_DAP::OnBreakpointFunctionSet,  this);
    m_dapClient.Unbind(wxEVT_DAP_LAUNCH_RESPONSE,                 &Debugger_DAP::OnLaunchResponse,          this);
    m_dapClient.Unbind(wxEVT_DAP_RUN_IN_TERMINAL_REQUEST,         &Debugger_DAP::OnRunInTerminalRequest,    this);
    m_dapClient.Unbind(wxEVT_DAP_LOG_EVENT,                       &Debugger_DAP::OnDapLog,                  this);
    m_dapClient.Unbind(wxEVT_DAP_MODULE_EVENT,                    &Debugger_DAP::OnDapModuleEvent,          this);
    m_dapClient.Unbind(wxEVT_DAP_CONFIGURARIONE_DONE_RESPONSE,    &Debugger_DAP::OnConfigurationDoneResponse,  this);
    m_dapClient.Unbind(wxEVT_DAP_THREADS_RESPONSE,                &Debugger_DAP::OnTreadResponse,           this);
    m_dapClient.Unbind(wxEVT_DAP_STOPPED_ON_ENTRY_EVENT,          &Debugger_DAP::OnStopOnEntryEvent,        this);
    m_dapClient.Unbind(wxEVT_DAP_PROCESS_EVENT,                   &Debugger_DAP::OnProcessEvent,            this);
    m_dapClient.Unbind(wxEVT_DAP_BREAKPOINT_EVENT,                &Debugger_DAP::OnBreakpointEvent,         this);
    m_dapClient.Unbind(wxEVT_DAP_CONTINUED_EVENT,                 &Debugger_DAP::OnCcontinuedEvent,         this);
    m_dapClient.Unbind(wxEVT_DAP_DEBUGPYWAITINGFORSERVER_EVENT,   &Debugger_DAP::OnDebugPYWaitingForServerEvent,  this);

    if (m_dapPid != 0)
    {
        wxKill(m_dapPid);
        m_dapPid = 0;
    }
}

void Debugger_DAP::OnAttachReal()
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format("%s %d", __PRETTY_FUNCTION__, __LINE__));
    m_timer_poll_debugger.SetOwner(this, id_gdb_poll_timer);
    DebuggerManager & dbg_manager = *Manager::Get()->GetDebuggerManager();
    dbg_manager.RegisterDebugger(this);
    // Do no use cbEVT_PROJECT_OPEN as the project may not be active!!!!
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,  new cbEventFunctor<Debugger_DAP, CodeBlocksEvent>(this, &Debugger_DAP::OnProjectOpened));
}

void Debugger_DAP::OnReleaseReal(bool appShutDown)
{
    // Do not log anything as we are closing
    DAPDebuggerResetData(true);
    Manager::Get()->GetDebuggerManager()->UnregisterDebugger(this);
    //    if (m_command_stream_dialog)
    //    {
    //        m_command_stream_dialog->Destroy();
    //        m_command_stream_dialog = nullptr;
    //    }
}

void Debugger_DAP::GetCurrentPosition(wxString & filename, int & line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    if (m_State.HasDriver())
    //    {
    //        const Cursor& cursor = m_State.GetDriver()->GetCursor();
    //        filename = cursor.file;
    //        line = cursor.line;
    //    }
    //    else
    //    {
    //        filename = wxEmptyString;
    //        line = -1;
    //    }
}

void Debugger_DAP::SetupToolsMenu(wxMenu & menu)
{
    menu.Append(id_menu_info_command_stream, _("Show command stream"));
}

bool Debugger_DAP::SupportsFeature(cbDebuggerFeature::Flags flag)
{
    switch (flag)
    {
        case cbDebuggerFeature::Breakpoints:
        case cbDebuggerFeature::Callstack:
        case cbDebuggerFeature::Watches:
        case cbDebuggerFeature::RunToCursor:
        case cbDebuggerFeature::SetNextStatement:
            return true;

        case cbDebuggerFeature::CPURegisters:
        case cbDebuggerFeature::Disassembly:
        case cbDebuggerFeature::ExamineMemory:
        case cbDebuggerFeature::Threads:
        case cbDebuggerFeature::ValueTooltips:
        default:
            return false;
    }
}

cbDebuggerConfiguration * Debugger_DAP::LoadConfig(const ConfigManagerWrapper & config)
{
    return new dbg_DAP::DebuggerConfiguration(config);
}

dbg_DAP::DebuggerConfiguration & Debugger_DAP::GetActiveConfigEx()
{
    return static_cast<dbg_DAP::DebuggerConfiguration &>(GetActiveConfig());
}

cbConfigurationPanel * Debugger_DAP::GetProjectConfigurationPanel(wxWindow * parent, cbProject * project)
{
    dbg_DAP::DebuggerOptionsProjectDlg * dlg = new dbg_DAP::DebuggerOptionsProjectDlg(parent, this, project);
    return dlg;
}


void Debugger_DAP::OnIdle(wxIdleEvent & event)
{
    event.Skip();
}

void Debugger_DAP::OnTimer(wxTimerEvent & /*event*/)
{
    wxWakeUpIdle();
}

void Debugger_DAP::OnMenuInfoCommandStream(wxCommandEvent & /*event*/)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    wxString full;
    //
    //    for (int ii = 0; ii < m_executor.GetCommandQueueCount(); ++ii)
    //    {
    //        full += m_executor.GetQueueCommand(ii) + "\n";
    //    }
    //
    //    if (m_command_stream_dialog)
    //    {
    //        m_command_stream_dialog->SetText(full);
    //        m_command_stream_dialog->Show();
    //    }
    //    else
    //    {
    //        m_command_stream_dialog = new dbg_DAP::DAPTextInfoWindow(Manager::Get()->GetAppWindow(), _T("Command stream"), full);
    //        m_command_stream_dialog->Show();
    //    }
}

void Debugger_DAP::UpdateDebugDialogs(bool bClearAllData)
{
    cbBacktraceDlg * pDialogBacktrace = Manager::Get()->GetDebuggerManager()->GetBacktraceDialog();

    if (pDialogBacktrace)
    {
        pDialogBacktrace->Reload();
    }

    cbBreakpointsDlg * pDialogBreakpoint = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();

    if (pDialogBreakpoint)
    {
        if (bClearAllData)
        {
            pDialogBreakpoint->RemoveAllBreakpoints();
        }
        else
        {
            pDialogBreakpoint->Reload();
        }
    }

    if (bClearAllData)
    {
        cbExamineMemoryDlg * pDialogExamineMemory = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();

        if (pDialogExamineMemory)
        {
            pDialogExamineMemory->SetBaseAddress("");
            pDialogExamineMemory->Clear();
        }
    }

    cbThreadsDlg * pDialogThreads = Manager::Get()->GetDebuggerManager()->GetThreadsDialog();

    if (pDialogThreads)
    {
        pDialogThreads->Reload();
    }

    cbWatchesDlg * pDialogWatches = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();

    if (pDialogWatches)
    {
        pDialogWatches->RefreshUI();
    }

    if (bClearAllData)
    {
        cbCPURegistersDlg * pDialogCPURegisters = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

        if (pDialogCPURegisters)
        {
            pDialogCPURegisters->Clear();
        }
    }

    if (bClearAllData)
    {
        cbDisassemblyDlg * pDialogDisassembly = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

        if (pDialogDisassembly)
        {
            cbStackFrame sf;
            pDialogDisassembly->Clear(sf);
        }
    }
}

bool Debugger_DAP::Debug(bool breakOnEntry)
{
    m_hasStartUpError = false;
    ProjectManager & project_manager = *Manager::Get()->GetProjectManager();
    cbProject * project = project_manager.GetActiveProject();

    if (!project)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_DAP::LogPaneLogger::LineType::Error);
        return false;
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("starting debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    StartType start_type = breakOnEntry ? StartTypeStepInto : StartTypeRun;

    if (!EnsureBuildUpToDate(start_type))
    {
        return false;
    }

    //    if (!WaitingCompilerToFinish() && !m_executor.IsRunning() && !m_hasStartUpError)
    //    {
    //        return StartDebugger(project, start_type) == 0;
    //    }
    //    else
    {
        return true;
    }
}

// "=========================================================================================="
// "   ____    ___    __  __   ____    ___   _       _____   ____        ___      __  _____   "
// "  / ___|  / _ \  |  \/  | |  _ \  |_ _| | |     | ____| |  _ \      |_ _|    / / |  ___|  "
// " | |     | | | | | |\/| | | |_) |  | |  | |     |  _|   | |_) |      | |    / /  | |_     "
// " | |___  | |_| | | |  | | |  __/   | |  | |___  | |___  |  _ <       | |   / /   |  _|    "
// "  \____|  \___/  |_|  |_| |_|     |___| |_____| |_____| |_| \_\     |___| /_/    |_|      "
// "                                                                                          "
// "=========================================================================================="


bool Debugger_DAP::SelectCompiler(cbProject & project, Compiler *& compiler,
                                  ProjectBuildTarget *& target, long pid_to_attach)
{
    // select the build target to debug
    target = NULL;
    compiler = NULL;
    wxString active_build_target = project.GetActiveBuildTarget();

    if (pid_to_attach == 0)
    {
        if (!project.BuildTargetValid(active_build_target, false))
        {
            int tgtIdx = project.SelectTarget();

            if (tgtIdx == -1)
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Selecting target cancelled"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                return false;
            }

            target = project.GetBuildTarget(tgtIdx);
            active_build_target = target->GetTitle();
        }
        else
        {
            target = project.GetBuildTarget(active_build_target);
        }

        // make sure it's not a commands-only target
        if (target->GetTargetType() == ttCommandsOnly)
        {
            cbMessageBox(_("The selected target is only running pre/post build step commands\n"
                           "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("The selected target is only running pre/post build step commands,Can't debug such a target... ")), dbg_DAP::LogPaneLogger::LineType::Error);
            return false;
        }

        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Selecting target: %s"), target->GetTitle()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        // find the target's compiler (to see which debugger to use)
        compiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : project.GetCompilerID());
    }
    else
    {
        compiler = CompilerFactory::GetDefaultCompiler();
    }

    return true;
}

bool Debugger_DAP::CompilerFinished(bool compilerFailed, StartType startType)
{
    if (compilerFailed || startType == StartTypeUnknown)
    {
        m_temporary_breakpoints.clear();
    }
    else
    {
        ProjectManager & project_manager = *Manager::Get()->GetProjectManager();
        cbProject * project = project_manager.GetActiveProject();

        if (project)
        {
            return StartDebugger(project, startType) == 0;
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }

    return false;
}

// "===================================================================================="
// " ____    ____     ___        _   _____    ____   _____       ___      __  _____     "
// " |  _ \  |  _ \   / _ \      | | | ____|  / ___| |_   _|     |_ _|    / / |  ___|   "
// " | |_) | | |_) | | | | |  _  | | |  _|   | |       | |        | |    / /  | |_      "
// " |  __/  |  _ <  | |_| | | |_| | | |___  | |___    | |        | |   / /   |  _|     "
// " |_|     |_| \_\  \___/   \___/  |_____|  \____|   |_|       |___| /_/    |_|       "
// "                                                                                    "
// "===================================================================================="

struct BreakpointMatchProject
{
    BreakpointMatchProject(cbProject * project) : project(project) {}
    bool operator()(cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp) const
    {
        return bp->GetProject() == project;
    }
    cbProject * project;
};

void Debugger_DAP::OnProjectOpened(CodeBlocksEvent & event)
{
    // allow others to catch this
    event.Skip();

    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        LoadStateFromFile(event.GetProject());
    }
}

void Debugger_DAP::CleanupWhenProjectClosed(cbProject * project)
{
    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        SaveStateToFile(project);
    }

    m_breakpoints.clear();
    m_map_filebreakpoints.clear();
    m_backtrace.clear();
    cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
    dlg->Reload();

    for (dbg_DAP::DAPWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end();)
    {
        cb::shared_ptr<dbg_DAP::DAPWatch> watch = *it;

        if (watch->GetProject() == project)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Remove watch for \"%s\"", watch->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
            cbWatchesDlg * dialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            dialog->RemoveWatch(watch);  // This call removed the watch from the GUI and debugger
        }
        else
        {
            it++;
        }
    }

    if (!project)
    {
        UpdateDebugDialogs(true);
    }

    ClearLog();
}

// "========================================================================================================="
// "  _          _      _   _   _   _    ____   _   _        __    ____    _____      _      ____    _____   "
// " | |        / \    | | | | | \ | |  / ___| | | | |      / /   / ___|  |_   _|    / \    |  _ \  |_   _|  "
// " | |       / _ \   | | | | |  \| | | |     | |_| |     / /    \___ \    | |     / _ \   | |_) |   | |    "
// " | |___   / ___ \  | |_| | | |\  | | |___  |  _  |    / /      ___) |   | |    / ___ \  |  _ <    | |    "
// " |_____| /_/   \_\  \___/  |_| \_|  \____| |_| |_|   /_/      |____/    |_|   /_/   \_\ |_| \_\   |_|    "
// "                                                                                                         "
// "========================================================================================================="

int Debugger_DAP::StartDebugger(cbProject * project, StartType start_type)
{
    //    ShowLog(true);
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);

    if (!compiler)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no compiler found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 2;
    }

    if (!target)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no target found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 3;
    }

    // is gdb accessible, i.e. can we find it?
    wxString dap_debugger = GetActiveConfigEx().GetDAPExecutable(true);

    if (!wxFileExists(dap_debugger))
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Cannot find dap_debugger. Currently set to: %s"), dap_debugger), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 4;
    }

    wxString dap_port_number = GetActiveConfigEx().GetDAPPortNumber();

    if (dap_port_number.IsEmpty())
    {
        dap_port_number = "12345";
    }

    wxString debuggee, working_dir;

    if (!GetDebuggee(debuggee, working_dir, target))
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot find debuggee!"), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 6;
    }

    bool console = target->GetTargetType() == ttConsoleOnly;
    // ---------------------------------------------------------------------------------------------------------------------
    int res = LaunchDebugger(project, dap_debugger, debuggee, dap_port_number, working_dir, 0, console, start_type);
    // ---------------------------------------------------------------------------------------------------------------------

    if (res != 0)
    {
        m_hasStartUpError = true;
        return res;
    }

    m_pProject = project;
    m_hasStartUpError = false;
    return 0;
}

void Debugger_DAP::LaunchDAPDebugger(cbProject * project, const wxString & dap_debugger, const wxString & dap_port_number)
{
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);
    wxString dapStartCmd = wxString::Format("%s --port %s", dap_debugger, dap_port_number);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dapStartCmd: %s"), dapStartCmd), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Setup the environment
    wxFileName debuggerFN(dap_debugger);
    wxString wdir = debuggerFN.GetPath();

    if (wdir.empty())
    {
        wdir = m_pProject ? m_pProject->GetBasePath() : _T(".");
    }

    wxExecuteEnv execEnv;
    execEnv.cwd = wdir;
    // Read the current environment variables and then make changes to them.
    wxGetEnvMap(&execEnv.env);
#ifndef __WXMAC__
    wxString oldLibPath;
    wxGetEnv(CB_LIBRARY_ENVVAR, &oldLibPath);
    wxString newLibPath = GetLibraryPath(oldLibPath, compiler, target, project);
    execEnv.env[CB_LIBRARY_ENVVAR] = newLibPath;

    if (HasDebugLog())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("setting execEnv.env[\"%s\"]=%s"), CB_LIBRARY_ENVVAR, newLibPath),
                                 dbg_DAP::LogPaneLogger::LineType::Debug);
    }

#endif
    wxString newPythonHomeSetting = GetActiveConfigEx().GetDAPPythonHomeEnvSetting();

    if (!newPythonHomeSetting.IsEmpty())
    {
        execEnv.env["PYTHONHOME"] = newPythonHomeSetting;

        if (HasDebugLog())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("setting execEnv.env[PYTHONHOME]=%s"), newPythonHomeSetting),
                                     dbg_DAP::LogPaneLogger::LineType::Debug);
        }
    }

    if (HasDebugLog())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("\nexecEnv.cwd: %s"), execEnv.cwd),
                                 dbg_DAP::LogPaneLogger::LineType::Debug);

        for (wxEnvVariableHashMap::iterator it = execEnv.env.begin(); it != execEnv.env.end(); ++it)
        {
            m_pLogger->LogDAPMsgType("",
                                     __LINE__,
                                     wxString::Format("execEnv.env[%s]=%s", it->first, it->second),
                                     dbg_DAP::LogPaneLogger::LineType::Debug);
        }
    }

    // start the dap_debugger process
    // NOTE: If the debugger does not start check the PYTHONHOME environment variable is set correctly!!!!
    m_dapPid = wxExecute(dapStartCmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_SHOW_CONSOLE, NULL, &execEnv);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("finished dapStartCmd: %s , m_dapPid: %ld"), dapStartCmd, m_dapPid), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

int Debugger_DAP::LaunchDebugger(cbProject * project,
                                 const wxString & dap_debugger,
                                 const wxString & debuggee,
                                 const wxString & dap_port_number,
                                 const wxString & working_dir,
                                 int pid,
                                 bool console,
                                 StartType start_type)
{
    // Reset the client and data
    DAPDebuggerResetData(false);

    if (dap_debugger.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no debugger executable found (full path)!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 1;
    }

    if (dap_port_number.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("dap_debugger is empty!!!!"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        return 2;
    }

    if (!debuggee.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE: %s"), debuggee), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }

    wxBusyCursor cursor;

    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::RunDAPServer))
    {
        LaunchDAPDebugger(project, dap_debugger, dap_port_number);
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("You have configured the debugger so you need to manually run the DAP server on port %s"), dap_port_number), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        if (dap_port_number.IsEmpty())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_debugger: %s  port: %s"), dap_debugger, dap_port_number), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_debugger: %s  port: <empty>"), dap_debugger), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }

        if (!debuggee.IsEmpty())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE: %s"), debuggee), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
    }

    // For this we use socket transport. But you may choose
    // to write your own transport that implements the dap::Transport interface
    // This is useful when the user wishes to use stdin/out for communicating with
    // the dap and not over socket
    dap::SocketTransport * transport = new dap::SocketTransport();
    wxString connection = wxString::Format("tcp://127.0.0.1:%s", dap_port_number);

    if (!transport->Connect(connection, 5))
    {
        if (m_dapPid != 0)
        {
            wxKill(m_dapPid);
            m_dapPid = 0;
        }

        wxMessageBox("Failed to connect to DAP server", "DAP Debugger Plugin", wxICON_ERROR | wxOK | wxCENTRE);
        return 1;
    }

    DAPDebuggerState = eDAPState::Connected;
    // construct new client with the transport
    m_dapClient.SetTransport(transport);
    // This part is done in mode **sync**
    DAPDebuggerState = eDAPState::Running;
    // Create the DAP debuggee command line including any parameters
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);
    wxString debugeeArgs = target->GetExecutionParameters();
    m_DAP_DebuggeeStartCMD.clear();
    m_DAP_DebuggeeStartCMD.push_back(static_cast<wxString>(debuggee));

    if (!debugeeArgs.IsEmpty())
    {
        Manager::Get()->GetMacrosManager()->ReplaceMacros(debugeeArgs);
        wxArrayString debugeeArgsArray = wxSplit(debugeeArgs, ' ');

        for (wxString param : debugeeArgsArray)
        {
            m_DAP_DebuggeeStartCMD.push_back(param);
        }
    }

    // The protocol starts by us sending an initialize request
    dap::InitializeRequestArguments initArgs;
    initArgs.linesStartAt1 = true;
    initArgs.clientID = "CB_DAP_Plugin";
    initArgs.clientName = "CB_DAP_Plugin";
    m_dapClient.Initialize(&initArgs);
    //    if (active_config.GetFlag(dbg_DAP::DebuggerConfiguration::CatchExceptions))
    //    {
    //        DoSendCommand("catch throw");
    //        DoSendCommand("catch catch");
    //    }
    //
    //    wxString directorySearchPaths = wxEmptyString;
    //    const wxArrayString& pdirs = ParseSearchDirs(project);
    //    for (size_t i = 0; i < pdirs.GetCount(); ++i)
    //    {
    //        directorySearchPaths.Append(pdirs[i]);
    //        directorySearchPaths.Append(wxPATH_SEP);
    //    }
    //
    //    if (!directorySearchPaths.IsEmpty())
    //    {
    //        DoSendCommand(wxString::Format("directory %s", directorySearchPaths));
    //    }
    //
    //
    m_timer_poll_debugger.Start(20);
    return 0;
}

// "====================================================================================================="
// "  ____    _____   ____    _   _    ____        ____    ___    _   _   _____   ____     ___    _      "
// " |  _ \  | ____| | __ )  | | | |  / ___|      / ___|  / _ \  | \ | | |_   _| |  _ \   / _ \  | |     "
// " | | | | |  _|   |  _ \  | | | | | |  _      | |     | | | | |  \| |   | |   | |_) | | | | | | |     "
// " | |_| | | |___  | |_) | | |_| | | |_| |     | |___  | |_| | | |\  |   | |   |  _ <  | |_| | | |___  "
// " |____/  |_____| |____/   \___/   \____|      \____|  \___/  |_| \_|   |_|   |_| \_\  \___/  |_____| "
// "                                                                                                     "
// "====================================================================================================="

bool Debugger_DAP::RunToCursor(const wxString & filename, int line, const wxString & /*line_text*/)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (IsRunning())
    {
        //        if (IsStopped())
        //        {
        //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("=>-exec-until %s:%d<=", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        //            CommitRunCommand(wxString::Format("-exec-until %s:%d", filename.c_str(), line));
        //            return true;
        //        }
        //        else
        //        {
        //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("filename:%s line:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
        //        }
        //
        return false;
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("RunToCursor %s:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);
        cb::shared_ptr<dbg_DAP::DAPBreakpoint> ptr(new dbg_DAP::DAPBreakpoint(project, m_pLogger, filename, line, -1));
        m_temporary_breakpoints.push_back(ptr);
        return Debug(false);
    }
}

void Debugger_DAP::SetNextStatement(const wxString & filename, int line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (IsStopped())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("-break-insert -t & -exec-jump for filename:=>%s<= line:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        //        AddStringCommand(wxString::Format("-break-insert -t %s:%d", filename.c_str(), line));
    }
}

void Debugger_DAP::Continue()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Continue", dbg_DAP::LogPaneLogger::LineType::Debug);
    m_dapClient.Continue();
}

void Debugger_DAP::Next()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Next", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Next();
}

void Debugger_DAP::NextInstruction()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "NextInstruction", dbg_DAP::LogPaneLogger::LineType::Command);
    // m_dapClient.Next();
}

void Debugger_DAP::StepIntoInstruction()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Functionality not supported yet!", dbg_DAP::LogPaneLogger::LineType::Command);
    //m_dapClient.StepIn();
}

void Debugger_DAP::Step()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Error);
    m_dapClient.StepIn();
}

void Debugger_DAP::StepOut()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.StepOut();
}

void Debugger_DAP::Break()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Break", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Pause();
}

void Debugger_DAP::Stop()
{
    if (!IsRunning())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger failed as not running!!!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return;
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    DAPDebuggerResetData(false);
    UpdateDebugDialogs(false);
}

bool Debugger_DAP::IsRunning() const
{
    return (DAPDebuggerState != eDAPState::NotConnected);
}

bool Debugger_DAP::IsStopped() const
{
    return (DAPDebuggerState == eDAPState::Stopped);
}

bool Debugger_DAP::IsBusy() const
{
    return (DAPDebuggerState != eDAPState::Stopped);
}

int Debugger_DAP::GetExitCode() const
{
    return m_exit_code;
}

//  "================================================================================================"
//  "      ____                          _                      _           _                        "
//  "     | __ )   _ __    ___    __ _  | | __  _ __     ___   (_)  _ __   | |_   ___                "
//  "     |  _ \  | '__|  / _ \  / _` | | |/ / | '_ \   / _ \  | | | '_ \  | __| / __|               "
//  "     | |_) | | |    |  __/ | (_| | |   <  | |_) | | (_) | | | | | | | | |_  \__ \               "
//  "     |____/  |_|     \___|  \__,_| |_|\_\ | .__/   \___/  |_| |_| |_|  \__| |___/               "
//  "                                          |_|                                                   "
//  "                                                                                                "
//  "================================================================================================"

void Debugger_DAP::CreateStartBreakpoints(bool force)
{
    long lineNumBrk;

    for (dbg_DAP::DAPBreakpointsContainer::iterator itBP = m_breakpoints.begin(); itBP != m_breakpoints.end(); ++itBP)
    {
        if ((*itBP)->GetIndex() == -1 || force)
        {
            if ((*itBP)->GetLineString().ToLong(&lineNumBrk))
            {
                wxString brkFileName = (*itBP)->GetFilename();
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("File: %s  Line: %ld from m_breakpoints", brkFileName, lineNumBrk), dbg_DAP::LogPaneLogger::LineType::Debug);
                UpdateMapFileBreakPoints(brkFileName, (*itBP), true);
            }
        }
    }

    for (dbg_DAP::DAPBreakpointsContainer::const_iterator itTBP = m_temporary_breakpoints.begin(); itTBP != m_temporary_breakpoints.end(); ++itTBP)
    {
        if ((*itTBP)->GetLineString().ToLong(&lineNumBrk))
        {
            wxString brkFileName = (*itTBP)->GetFilename();
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("File: %s  Line: %ld from m_temporary_breakpoints", brkFileName, lineNumBrk), dbg_DAP::LogPaneLogger::LineType::Debug);
            UpdateMapFileBreakPoints(brkFileName, (*itTBP), true);
        }
    }

    for (std::map<wxString, dbg_DAP::DAPBreakpointsContainer>::const_iterator itMFP = m_map_filebreakpoints.begin(); itMFP != m_map_filebreakpoints.end(); ++itMFP)
    {
        UpdateDAPSetBreakpointsByFileName(itMFP->first);
    }

    m_temporary_breakpoints.clear();
}

int Debugger_DAP::GetBreakpointsCount() const
{
    return m_breakpoints.size();
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::GetBreakpoint(int index)
{
    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints[index]);
}

cb::shared_ptr<const cbBreakpoint> Debugger_DAP::GetBreakpoint(int index) const
{
    return cb::static_pointer_cast<const cbBreakpoint>(m_breakpoints[index]);
}


cb::shared_ptr<cbBreakpoint> Debugger_DAP::GetBreakpointByID(int id)
{
    for (dbg_DAP::DAPBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        if ((*it)->GetID() == id)
        {
            return *it;
        }
    }

    return cb::shared_ptr<cbBreakpoint>();
}

cb::shared_ptr<dbg_DAP::DAPBreakpoint> Debugger_DAP::FindBreakpoint(const cbProject * project, const wxString & filename, const int line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("filename: %s ,  Line %d"), filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);

    for (dbg_DAP::DAPBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        if (
            ((*it)->GetProject() == project)
            &&
            ((*it)->GetFilename() == filename)
            &&
            ((*it)->GetLine() == line)
        )
        {
            return *it;
        }
    }

    return cb::shared_ptr<dbg_DAP::DAPBreakpoint>();
}

void Debugger_DAP::UpdateMapFileBreakPoints(const wxString & filename, cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp, bool bAddBreakpoint)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("filename: %s ,  BP: %s Line %d  , bAddBreakpoint: %s"), filename, bp->GetFilename(), bp->GetLine(), bAddBreakpoint ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::Debug);
    std::map<wxString, dbg_DAP::DAPBreakpointsContainer>::iterator mapit;
    mapit = m_map_filebreakpoints.find(filename);

    if (mapit == m_map_filebreakpoints.end())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints includes filename %s"), filename), dbg_DAP::LogPaneLogger::LineType::Debug);

        if (bAddBreakpoint == true)
        {
            std::vector<cb::shared_ptr<dbg_DAP::DAPBreakpoint>> vecFileBPreakpoints;
            vecFileBPreakpoints.push_back(bp);
            m_map_filebreakpoints.insert(std::pair<wxString, std::vector<cb::shared_ptr<dbg_DAP::DAPBreakpoint>>>(filename, vecFileBPreakpoints));
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints insert for filename %s ,  BP %s Line %d bAddBreakpoint: %s"), filename, bp->GetFilename(), bp->GetLine(), bAddBreakpoint ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::Debug);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Trying to remove a break point line, but it is not in m_map_filebreakpoints!"), dbg_DAP::LogPaneLogger::LineType::Error);
            //                #ifdef __MINGW32__
            //                    if (IsDebuggerPresent())
            //                    {
            //                        DebugBreak();
            //                    }
            //                #endif // __MINGW32__
        }
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints does not include filename %s"), filename), dbg_DAP::LogPaneLogger::LineType::Debug);
        bool bFoundLine = false;
        int lineBP = bp->GetLine();
        dbg_DAP::DAPBreakpointsContainer::iterator itFBrk;

        for (itFBrk = (*mapit).second.begin(); itFBrk != (*mapit).second.end(); ++itFBrk)
        {
            if ((*itFBrk)->GetLine() == lineBP)
            {
                bFoundLine = true;
                break;
            }
        }

        if (bFoundLine)
        {
            if (bAddBreakpoint)
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Trying to add a break point line that is allready in m_map_filebreakpoints!"), dbg_DAP::LogPaneLogger::LineType::Error);
            }
            else
            {
                if ((*mapit).second.size() == 1)
                {
                    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints erase filename %s ,  BP %s Line %d bAddBreakpoint: %s"), filename, bp->GetFilename(), bp->GetLine(), bAddBreakpoint ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::Debug);

                    if (m_map_filebreakpoints.size() == 1)
                    {
                        m_map_filebreakpoints.clear();
                    }
                    else
                    {
                        m_map_filebreakpoints.erase(mapit);
                    }
                }
                else
                {
                    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints erase for filename %s ,  BP %s Line %d bAddBreakpoint: %s"), filename, bp->GetFilename(), bp->GetLine(), bAddBreakpoint ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::Debug);
                    (*mapit).second.erase(itFBrk);
                }
            }
        }
        else
        {
            if (bAddBreakpoint == true)
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_map_filebreakpoints add filename %s ,  BP %s Line %d bAddBreakpoint: %s"), filename, bp->GetFilename(), bp->GetLine(), bAddBreakpoint ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::Debug);
                (*mapit).second.push_back(bp);
            }
            else
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Trying to remove a break point line, but it is not in m_map_filebreakpoints!"), dbg_DAP::LogPaneLogger::LineType::Error);
            }
        }
    }
}

void Debugger_DAP::UpdateDAPSetBreakpointsByFileName(const wxString & filename)
{
    if (IsRunning())
    {
        std::vector<dap::SourceBreakpoint> vlines;
        wxString sLineInfo = wxEmptyString;

        if (m_map_filebreakpoints.size() > 0)
        {
            std::map<wxString, dbg_DAP::DAPBreakpointsContainer>::iterator mapit;
            mapit = m_map_filebreakpoints.find(filename);
            dbg_DAP::DAPBreakpointsContainer filebreakpoints = mapit->second;

            for (dbg_DAP::DAPBreakpointsContainer::iterator it = filebreakpoints.begin(); it != filebreakpoints.end(); ++it)
            {
                int line = static_cast<int>((*it)->GetLine());
                sLineInfo.Append(wxString::Format("%d ", line));
                vlines.push_back({ line, wxEmptyString });
            }
        }

        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("m_dapClient.SetBreakpointsFile(%s , [%s]", filename, sLineInfo), dbg_DAP::LogPaneLogger::LineType::Debug);
        m_dapClient.SetBreakpointsFile(filename, vlines);
    }
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddBreakpoint(const wxString & filename, int line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("AddBreakpoint %s:%d"), filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
    return UpdateOrAddBreakpoint(filename, line, -1);
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::UpdateOrAddBreakpoint(const wxString & filename, const int line, const int id)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Filename: %s Line: %d", filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
    cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);
    cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp = FindBreakpoint(project, filename, line);

    if (bp)
    {
        if (id != -1)
        {
            bp->SetID(id);
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Update %s:%d set ID %d"), filename, line, id), dbg_DAP::LogPaneLogger::LineType::Debug);
        }

        //        else
        //        {
        //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Exit as BP found and ID=-1 for %s:%d"), filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
        //        }
        return bp;
    }

    cb::shared_ptr<dbg_DAP::DAPBreakpoint> newDAPBreakpoint(new dbg_DAP::DAPBreakpoint(project, m_pLogger, filename, line, id));
    UpdateMapFileBreakPoints(filename, newDAPBreakpoint, true);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("m_breakpoints push_back %s:%d"), filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
    m_breakpoints.push_back(newDAPBreakpoint);
    UpdateDAPSetBreakpointsByFileName(filename);
    return newDAPBreakpoint;
}

void Debugger_DAP::DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp = cb::static_pointer_cast<dbg_DAP::DAPBreakpoint>(breakpoint);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("%s:%d"), bp->GetLocation(), bp->GetLine()), dbg_DAP::LogPaneLogger::LineType::Debug);
    dbg_DAP::DAPBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), bp);

    if (it != m_breakpoints.end())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Found %s:%d"), bp->GetLocation(), bp->GetLine()), dbg_DAP::LogPaneLogger::LineType::Debug);
        cb::shared_ptr<dbg_DAP::DAPBreakpoint> pBrkPt = *it;
        dbg_DAP::DAPBreakpoint::BreakpointType bpType = pBrkPt->GetType();

        switch (bpType)
        {
            case dbg_DAP::DAPBreakpoint::bptCode:
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Found dbg_DAP::DAPBreakpoint::bptCode breakpoint type")), dbg_DAP::LogPaneLogger::LineType::Debug);
                break;

            case dbg_DAP::DAPBreakpoint::bptData:
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Found dbg_DAP::DAPBreakpoint::bptData breakpoint type")), dbg_DAP::LogPaneLogger::LineType::Debug);
                return;

            case dbg_DAP::DAPBreakpoint::bptFunction:
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Found dbg_DAP::DAPBreakpoint::bptFunction breakpoint type")), dbg_DAP::LogPaneLogger::LineType::Debug);
#ifdef __MINGW32__

                if (IsDebuggerPresent())
                {
                    DebugBreak();
                }

#endif // __MINGW32__
                return;

            default:
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Unknown breakpoint type: %d",  bpType), dbg_DAP::LogPaneLogger::LineType::Error);
#ifdef __MINGW32__

                if (IsDebuggerPresent())
                {
                    DebugBreak();
                }

#endif // __MINGW32__
                return;
        }

        wxString brkFileName = bp->GetLocation();
        UpdateMapFileBreakPoints(brkFileName, (*it), false);

        if (m_breakpoints.size() == 1)
        {
            m_breakpoints.clear(); // Delete after you have finished using *it
        }
        else
        {
            m_breakpoints.erase(it); // Delete after you have finished using *it
        }

        UpdateDAPSetBreakpointsByFileName(brkFileName);
    }

    cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
    dlg->Reload();
}

void Debugger_DAP::DeleteAllBreakpoints()
{
    if (IsRunning())
    {
        std::vector<dap::SourceBreakpoint> vlines;
        vlines.clear();

        for (std::map<wxString, std::vector<cb::shared_ptr<dbg_DAP::DAPBreakpoint>>>::iterator it = m_map_filebreakpoints.begin(); it != m_map_filebreakpoints.end(); ++it)
        {
            m_dapClient.SetBreakpointsFile(it->first, vlines);
        }
    }

    m_map_filebreakpoints.clear();
    m_breakpoints.clear();
    cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
    dlg->Reload();
}

//cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddBreakpoint(cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp)
//{
//    m_breakpoints.push_back(bp);
//
//    if (IsRunning())
//    {
//        if (!IsStopped())
//        {
//            m_executor.Interupt();
//            m_actions.Add(new dbg_DAP::DAPBreakpointAddAction(bp, m_pLogger));
//            Continue();
//        }
//        else
//        {
//            m_actions.Add(new dbg_DAP::DAPBreakpointAddAction(bp, m_pLogger));
//        }
//    }
//
//    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints.back());
//}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddDataBreakpoint(const wxString & dataExpression)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("dataExpression : %s", dataExpression), dbg_DAP::LogPaneLogger::LineType::Warning);
    //
    //    dbg_DAP::DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), dataExpression, true, 1);
    //    PlaceWindow(&dlg);
    //    if (dlg.ShowModal() == wxID_OK)
    //    {
    //        bool enabled = dlg.IsBreakpointEnabled();
    //        const wxString& newDataExpression = dlg.GetDataExpression();
    //        int sel = dlg.GetSelection();
    //
    //        cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp(new dbg_DAP::DAPBreakpoint(m_pProject, m_pLogger));
    //        bp->SetType(dbg_DAP::DAPBreakpoint::BreakpointType::bptData);
    //        bp->SetIsEnabled(enabled);
    //        bp->SetBreakAddress(newDataExpression);
    //        bp->SetIsBreakOnRead(sel != 1);
    //        bp->SetIsBreakOnWrite(sel != 0);
    //
    //        AddBreakpoint(bp);
    //
    //        return bp;
    //    }
    //    else
    //    {
    return cb::shared_ptr<cbBreakpoint>();
    //    }
}

void Debugger_DAP::UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    dbg_DAP::DAPBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);
    //
    //    if (it == m_breakpoints.end())
    //    {
    //        return;
    //    }
    //
    //    cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp = cb::static_pointer_cast<dbg_DAP::DAPBreakpoint>(breakpoint);
    //    bool reset = false;
    //    switch (bp->GetType())
    //    {
    //        case dbg_DAP::DAPBreakpoint::bptCode:
    //        {
    //            dbg_DAP::EditBreakpointDlg dlg(*bp, Manager::Get()->GetAppWindow());
    //            PlaceWindow(&dlg);
    //            if (dlg.ShowModal() == wxID_OK)
    //            {
    //                *bp = dlg.GetBreakpoint();
    //                reset = true;
    //            }
    //            break;
    //        }
    //        case dbg_DAP::DAPBreakpoint::bptData:
    //        {
    //            int old_sel = 0;
    //            if (bp->GetIsBreakOnRead() && bp->GetIsBreakOnWrite())
    //            {
    //                old_sel = 2;
    //            }
    //            else if (!bp->GetIsBreakOnRead() && bp->GetIsBreakOnWrite())
    //            {
    //                old_sel = 1;
    //            }
    //
    //            dbg_DAP::DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), bp->GetBreakAddress(), bp->GetIsEnabled(), old_sel);
    //            PlaceWindow(&dlg);
    //            if (dlg.ShowModal() == wxID_OK)
    //            {
    //                bp->SetIsEnabled(dlg.IsEnabled());
    //                bp->SetIsBreakOnRead(dlg.GetSelection() != 1);
    //                bp->SetIsBreakOnWrite(dlg.GetSelection() != 0);
    //                bp->SetBreakAddress(dlg.GetDataExpression());
    //                reset = true;
    //            }
    //            break;
    //        }
    //        case dbg_DAP::DAPBreakpoint::bptFunction:
    //            return;
    //
    //        default:
    //            return;
    //    }
    //
    //    if (reset)
    //    {
    //        bool debuggerIsRunning = !IsStopped();
    //        if (debuggerIsRunning)
    //        {
    //            m_executor.Interupt(true);
    //        }
    //
    //        DeleteBreakpoint(bp);
    //        AddBreakpoint(bp);
    //
    //        if (debuggerIsRunning)
    //        {
    //            Continue();
    //        }
    //    }
}

void Debugger_DAP::ShiftBreakpoint(int index, int lines_to_shift)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    if (index < 0 || index >= static_cast<int>(m_breakpoints.size()))
    //    {
    //        return;
    //    }
    //
    //    cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp = m_breakpoints[index];
    //    bp->SetShiftLines(lines_to_shift);
    //
    //    if (IsRunning())
    //    {
    //        // just remove the breakpoints as they will become invalid
    //        if (!IsStopped())
    //        {
    //            m_executor.Interupt();
    //
    //            if (bp->GetIndex() >= 0)
    //            {
    //                AddStringCommand(wxString::Format("-break-delete %d", bp->GetIndex()));
    //                bp->SetIndex(-1);
    //            }
    //
    //            Continue();
    //        }
    //        else
    //        {
    //            if (bp->GetIndex() >= 0)
    //            {
    //                AddStringCommand(wxString::Format("-break-delete %d", bp->GetIndex()));
    //                bp->SetIndex(-1);
    //            }
    //        }
    //    }
}

void Debugger_DAP::EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("%s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()), dbg_DAP::LogPaneLogger::LineType::Debug);
    //    dbg_DAP::DAPBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);
    //
    //    if (it != m_breakpoints.end())
    //    {
    //
    //        int index = (*it)->GetIndex();
    //
    //        if ((*it)->IsEnabled() == enable)
    //        {
    //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
    //                                     __LINE__,
    //                                     wxString::Format(_("breakpoint found but no change needed: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
    //                                     dbg_DAP::LogPaneLogger::LineType::Debug);
    //            // N change required!
    //            return;
    //        }
    //        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
    //                                 __LINE__,
    //                                 wxString::Format(_("breakpoint found and change needed: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
    //                                 dbg_DAP::LogPaneLogger::LineType::Debug);
    //        if (index != -1)
    //        {
    //            wxString wxBreakCommand;
    //            if (enable)
    //            {
    //                wxBreakCommand = wxString::Format("-break-enable %d", index);
    //            }
    //            else
    //            {
    //                wxBreakCommand = wxString::Format("-break-disable %d", index);
    //            }
    //            if (m_executor.IsStopped())
    //            {
    //                AddStringCommand(wxBreakCommand);
    //            }
    //            else
    //            {
    //                m_executor.Interupt();
    //                AddStringCommand(wxBreakCommand);
    //                Continue();
    //            }
    //        }
    //        (*it)->SetEnabled(enable);
    //    }
    //    else
    //    {
    //        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
    //                                 __LINE__,
    //                                 wxString::Format(_("Breakpoint NOT FOUND: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
    //                                 dbg_DAP::LogPaneLogger::LineType::Warning);
    //
    //    }
}

// "=============================================================================================="
// "    __        __          _            _                                                      "
// "    \ \      / /   __ _  | |_    ___  | |__     ___   ___                                     "
// "     \ \ /\ / /   / _` | | __|  / __| | '_ \   / _ \ / __|                                    "
// "      \ V  V /   | (_| | | |_  | (__  | | | | |  __/ \__ \                                    "
// "       \_/\_/     \__,_|  \__|  \___| |_| |_|  \___| |___/                                    "
// "                                                                                              "
// "=============================================================================================="

void Debugger_DAP::CreateStartWatches()
{
    if (m_watches.empty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "No watches", dbg_DAP::LogPaneLogger::LineType::Debug);
    }

    for (dbg_DAP::DAPWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Watch clear for symbol %s", (*it)->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
        (*it)->Reset();
    }

    if (!m_watches.empty())
    {
        CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
        event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
        Manager::Get()->ProcessEvent(event);
    }
}

void Debugger_DAP::UpdateDAPWatches(int updateType)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("updating watches"), dbg_DAP::LogPaneLogger::LineType::Debug);
    //Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->OnDebuggerUpdated();
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(updateType);
    //event.SetPlugin(m_pDriver->GetDebugger());
    Manager::Get()->ProcessEvent(event);
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(const wxString & symbol, cb_unused bool update)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", symbol), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::DAPWatch> watch(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, symbol, false));

    for (const dap::Variable & var : m_stackdapvariables)
    {
        if (symbol.IsSameAs(var.name))
        {
            if (!var.value.empty() && !var.value.IsSameAs(var.type))
            {
                watch->SetValue(var.value);
            }
            else
            {
                watch->SetValue("TBA");
            }

            watch->SetType(var.type);
            break;
        }
    }

    m_watches.push_back(watch);
    UpdateDAPWatches(int(cbDebuggerPlugin::DebugWindows::Watches));
    return watch;
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(dbg_DAP::DAPWatch * watch, cb_unused bool update)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", watch->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::DAPWatch> w(watch);
    m_watches.push_back(w);

    if (IsRunning())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Need to wire up watch.", dbg_DAP::LogPaneLogger::LineType::Error);
        //        m_actions.Add(new dbg_DAP::DAPWatchCreateAction(w, m_watches, m_pLogger, true));
    }

    return w;
}

void Debugger_DAP::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (IsRunning())
    {
        if (IsStopped())
        {
            //            AddStringCommand("-var-delete " + (*it)->GetID());
        }
        else
        {
            //            m_executor.Interupt();
            //            AddStringCommand("-var-delete " + (*it)->GetID());
            Continue();
        }
    }

    if (m_watches.size() == 1)
    {
        m_watches.clear();
    }
    else
    {
        m_watches.erase(it);
    }
}

bool Debugger_DAP::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    if (watch == m_WatchLocalsandArgs)
    {
        return true;
    }

    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);
    return it != m_watches.end();
}

void Debugger_DAP::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
{
    // not supported for child nodes or memory ranges!
    if (watch->GetParent() || IsMemoryRangeWatch(watch))
    {
        return;
    }

    cb::shared_ptr<dbg_DAP::DAPWatch> real_watch = cb::static_pointer_cast<dbg_DAP::DAPWatch>(watch);
    dbg_DAP::EditWatchDlg dlg(real_watch, nullptr);
    PlaceWindow(&dlg);

    if (dlg.ShowModal() == wxID_OK)
    {
        DoWatches();
    }
}

bool Debugger_DAP::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (!IsStopped() || !IsRunning())
    {
        return false;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return false;
    }

    cb::shared_ptr<dbg_DAP::DAPWatch> real_watch = cb::static_pointer_cast<dbg_DAP::DAPWatch>(watch);
    //    AddStringCommand("-var-assign " + real_watch->GetID() + " " + value);
    //    m_actions.Add(new dbg_DAP::DAPWatchSetValueAction(*it, static_cast<dbg_DAP::DAPWatch*>(watch), value, m_pLogger));
    //    dbg_DAP::Action * update_action = new dbg_DAP::DAPWatchesUpdateAction(m_watches, m_pLogger);
    //    update_action->SetWaitPrevious(true);
    //    m_actions.Add(update_action);
    return true;
}

void Debugger_DAP::ExpandWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_DAP::DAPWatch> real_watch = cb::static_pointer_cast<dbg_DAP::DAPWatch>(watch);

        if (!real_watch->HasBeenExpanded())
        {
            //            m_actions.Add(new dbg_DAP::DAPWatchExpandedAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_DAP::CollapseWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_DAP::DAPWatch> real_watch = cb::static_pointer_cast<dbg_DAP::DAPWatch>(watch);

        if (real_watch->HasBeenExpanded() && real_watch->DeleteOnCollapse())
        {
            //            m_actions.Add(new dbg_DAP::DAPWatchCollapseAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_DAP::UpdateWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (IsRunning())
    {
        //        m_actions.Add(new dbg_DAP::DAPWatchCreateAction(*it, m_watches, m_pLogger, false));
    }
}

void Debugger_DAP::DoWatches()
{
    if (!IsRunning())
    {
        return;
    }

    dbg_DAP::DebuggerConfiguration & config = GetActiveConfigEx();
    bool bWatchFuncLocalsArgs = config.GetFlag(dbg_DAP::DebuggerConfiguration::WatchFuncLocalsArgs);

    if (bWatchFuncLocalsArgs)
    {
        if (m_WatchLocalsandArgs == nullptr)
        {
            m_WatchLocalsandArgs = cb::shared_ptr<dbg_DAP::DAPWatch>(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, "Function locals and arguments", false));
            m_WatchLocalsandArgs->Expand(true);
            m_WatchLocalsandArgs->MarkAsChanged(false);
            cbWatchesDlg * watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            watchesDialog->AddSpecialWatch(m_WatchLocalsandArgs, true);
        }
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Need to wire up DoWatches.", dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_actions.Add(new dbg_DAP::DAPStackVariables(m_pLogger, m_WatchLocalsandArgs, bWatchFuncLocalsArgs));
    // Update watches now
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
    Manager::Get()->ProcessEvent(event);
}

// "================================================================================================"
// "     __  __                                               ____                                  "
// "    |  \/  |   ___   _ __ ___     ___    _ __   _   _    |  _ \    __ _   _ __     __ _    ___  "
// "    | |\/| |  / _ \ | '_ ` _ \   / _ \  | '__| | | | |   | |_) |  / _` | | '_ \   / _` |  / _ \ "
// "    | |  | | |  __/ | | | | | | | (_) | | |    | |_| |   |  _ <  | (_| | | | | | | (_| | |  __/ "
// "    |_|  |_|  \___| |_| |_| |_|  \___/  |_|     \__, |   |_| \_\  \__,_| |_| |_|  \__, |  \___| "
// "                                                |___/                             |___/         "
// "================================================================================================"

cb::shared_ptr<cbWatch> Debugger_DAP::AddMemoryRange(uint64_t llAddress, uint64_t llSize, const wxString & symbol, bool update)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    cb::shared_ptr<dbg_DAP::DAPMemoryRangeWatch> watch(new dbg_DAP::DAPMemoryRangeWatch(m_pProject, m_pLogger, llAddress, llSize, symbol));
    //
    //    watch->SetSymbol(symbol);
    //    watch->SetAddress(llAddress);
    //
    //    m_memoryRanges.push_back(watch);
    //    m_mapWatchesToType[watch] = dbg_DAP::DAPWatchType::MemoryRange;
    //
    //    if (IsRunning())
    //    {
    //        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Adding watch for: address: %#018llx  size:%lld", llAddress, llSize), dbg_DAP::LogPaneLogger::LineType::Warning);
    //        m_actions.Add(new dbg_DAP::DAPMemoryRangeWatchCreateAction(watch, m_pLogger));
    //    }
    //
    return watch;
}


bool Debugger_DAP::IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch)
{
    dbg_DAP::DAPMapWatchesToType::const_iterator it = m_mapWatchesToType.find(watch);

    if (it == m_mapWatchesToType.end())
    {
        return false;
    }
    else
    {
        return (it->second == dbg_DAP::DAPWatchType::MemoryRange);
    }
}

// "===================================================================================================="
// "     ____    _____      _       ____   _  __       __    _____   ____       _      __  __   _____   "
// "    / ___|  |_   _|    / \     / ___| | |/ /      / /   |  ___| |  _ \     / \    |  \/  | | ____|  "
// "    \___ \    | |     / _ \   | |     | ' /      / /    | |_    | |_) |   / _ \   | |\/| | |  _|    "
// "     ___) |   | |    / ___ \  | |___  | . \     / /     |  _|   |  _ <   / ___ \  | |  | | | |___   "
// "    |____/    |_|   /_/   \_\  \____| |_|\_\   /_/      |_|     |_| \_\ /_/   \_\ |_|  |_| |_____|  "
// "                                                                                                    "
// "===================================================================================================="

int Debugger_DAP::GetStackFrameCount() const
{
    return m_backtrace.size();
}

cb::shared_ptr<const cbStackFrame> Debugger_DAP::GetStackFrame(int index) const
{
    return m_backtrace[index];
}

void Debugger_DAP::SwitchToFrame(int number)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (IsRunning() && IsStopped())
    {
        if (number < static_cast<int>(m_backtrace.size()))
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("SwitchToFrame"), dbg_DAP::LogPaneLogger::LineType::Debug);
            //int frame = m_backtrace[number]->GetNumber();
            //typedef dbg_DAP::DAPSwitchToFrame<DAPSwitchToFrameNotification> SwitchType;
            //m_actions.Add(new SwitchType(frame, DAPSwitchToFrameNotification(this), true));
        }
    }
}

int Debugger_DAP::GetActiveStackFrame() const
{
    return m_current_frame.GetStackFrame();
}

// "==============================================================================================="
// "     _____   _                                 _                                               "
// "    |_   _| | |__    _ __    ___    __ _    __| |  ___                                         "
// "      | |   | '_ \  | '__|  / _ \  / _` |  / _` | / __|                                        "
// "      | |   | | | | | |    |  __/ | (_| | | (_| | \__ \                                        "
// "      |_|   |_| |_| |_|     \___|  \__,_|  \__,_| |___/                                        "
// "                                                                                               "
// "==============================================================================================="

int Debugger_DAP::GetThreadsCount() const
{
    return m_threads.size();
}

cb::shared_ptr<const cbThread> Debugger_DAP::GetThread(int index) const
{
    return m_threads[index];
}

bool Debugger_DAP::SwitchToThread(int thread_number)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    if (IsStopped())
    //    {
    //        dbg_DAP::DAPSwitchToThread<Notifications> * a;
    //        a = new dbg_DAP::DAPSwitchToThread<Notifications>(thread_number,
    //                                                      m_pLogger,
    //                                                      Notifications(this, m_executor, true)
    //                                                     );
    //        m_actions.Add(a);
    //        return true;
    //    }
    //    else
    {
        return false;
    }
}


// "==============================================================================================="
// "     _____                   _     _____   _                                                   "
// "    |_   _|   ___     ___   | |   |_   _| (_)  _ __    ___                                     "
// "      | |    / _ \   / _ \  | |     | |   | | | '_ \  / __|                                    "
// "      | |   | (_) | | (_) | | |     | |   | | | |_) | \__ \                                    "
// "      |_|    \___/   \___/  |_|     |_|   |_| | .__/  |___/                                    "
// "                                              |_|                                              "
// ================================================================================================"

bool Debugger_DAP::ShowValueTooltip(int style)
{
    //    if (!m_pProcess || !IsStopped())
    return false;
    //    if (!m_State.HasDriver() || !m_State.GetDriver()->IsDebuggingStarted())
    //        return false;
    //
    //    if (!GetActiveConfigEx().GetFlag(DebuggerConfiguration::EvalExpression))
    //        return false;
    //    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER &&
    //        style != wxSCI_C_WORD2 && style != wxSCI_C_GLOBALCLASS  && style != wxSCI_C_WXSMITH &&
    //        style != wxSCI_F_IDENTIFIER)
    //    {
    //        return false;
    //    }
    //    return true;
}

void Debugger_DAP::OnValueTooltip(const wxString & token, const wxRect & evalRect)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_State.GetDriver()->EvaluateSymbol(token, evalRect);
}

void Debugger_DAP::AddTooltipWatch(const wxString & symbol, wxRect const & rect)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    cb::shared_ptr<dbg_DAP::DAPWatch> w(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, symbol, true));
    //    m_watches.push_back(w);
    //
    //    if (IsRunning())
    //    {
    //        m_actions.Add(new dbg_DAP::DAPWatchCreateTooltipAction(w, m_watches, m_pLogger, rect));
    //    }
}

//void Debugger_DAP::OnValueTooltip(const wxString & token, const wxRect & evalRect)
//{
//    AddTooltipWatch(token, evalRect);
//}
//
//bool Debugger_DAP::ShowValueTooltip(int style)
//{
//    if (!IsRunning() || !IsStopped())
//    {
//        return false;
//    }
//
//    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER && style != wxSCI_C_WORD2)
//    {
//        return false;
//    }
//
//    return true;
//}

// "================================================================================================"
// "     ____                                                                                       "
// "    |  _ \   _ __    ___     ___    ___   ___   ___                                             "
// "    | |_) | | '__|  / _ \   / __|  / _ \ / __| / __|                                            "
// "    |  __/  | |    | (_) | | (__  |  __/ \__ \ \__ \                                            "
// "    |_|     |_|     \___/   \___|  \___| |___/ |___/                                            "
// "                                                                                                "
// "================================================================================================"


void Debugger_DAP::AttachToProcess(const wxString & pid)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_pProject = NULL;
    //    long number;
    //
    //    if (!pid.ToLong(&number))
    //    {
    //        return;
    //    }
    //
    //    LaunchDebugger(m_pProject,
    //                   GetActiveConfigEx().GetDebuggerExecutable(),
    //                   wxEmptyString,
    //                   wxEmptyString,
    //                   wxEmptyString,
    //                   number,
    //                   false,
    //                   StartTypeRun);
    //    m_executor.SetAttachedPID(number);
}

void Debugger_DAP::DetachFromProcess()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    AddStringCommand(wxString::Format("-target-detach %ld", m_executor.GetAttachedPID()));
}

bool Debugger_DAP::IsAttachedToProcess() const
{
    // Gets called way too much, so message removed.
    //    return m_pid_attached != 0;
    return false;
}

// "================================================================================================"
// "     __  __   ___   ____     ____                                                               "
// "    |  \/  | |_ _| / ___|   / ___|                                                              "
// "    | |\/| |  | |  \___ \  | |                                                                  "
// "    | |  | |  | |   ___) | | |___                                                               "
// "    |_|  |_| |___| |____/   \____|                                                              "
// "                                                                                                "
// "================================================================================================"

void Debugger_DAP::ConvertDirectory(wxString & str, wxString base, bool relative)
{
    //    dbg_DAP::ConvertDirectory(str, base, relative);
}

void Debugger_DAP::RequestUpdate(DebugWindows window)
{
    if (!IsStopped())
    {
        return;
    }

    //    switch (window)
    //    {
    //        case Backtrace:
    //            {
    //                struct Switcher : dbg_DAP::DAPSwitchToFrameInvoker
    //                {
    //                    Switcher(Debugger_DAP * plugin, dbg_DAP::ActionsMap & actions) :
    //                        m_plugin(plugin),
    //                        m_actions(actions)
    //                    {
    //                    }
    //
    //                    virtual void Invoke(int frame_number)
    //                    {
    //                        typedef dbg_DAP::DAPSwitchToFrame<DAPSwitchToFrameNotification> SwitchType;
    //                        m_actions.Add(new SwitchType(frame_number, DAPSwitchToFrameNotification(m_plugin), false));
    //                    }
    //
    //                    Debugger_DAP * m_plugin;
    //                    dbg_DAP::ActionsMap & m_actions;
    //                };
    //                Switcher * switcher = new Switcher(this, m_actions);
    //                m_actions.Add(new dbg_DAP::DAPGenerateBacktrace(switcher, m_backtrace, m_current_frame, m_pLogger));
    //            }
    //            break;
    //
    //        case Threads:
    //            m_actions.Add(new dbg_DAP::DAPGenerateThreadsList(m_threads, m_current_frame.GetThreadId(), m_pLogger));
    //            break;
    //
    //        case CPURegisters:
    //            {
    //                m_actions.Add(new dbg_DAP::DAPGenerateCPUInfoRegisters(m_pLogger));
    //            }
    //            break;
    //
    //        case Disassembly:
    //            {
    //                wxString flavour = GetActiveConfigEx().GetDisassemblyFlavorCommand();
    //                m_actions.Add(new dbg_DAP::DAPDisassemble(flavour, m_pLogger));
    //            }
    //            break;
    //
    //        case ExamineMemory:
    //            {
    //                cbExamineMemoryDlg * dialog = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();
    //                wxString memaddress = dialog->GetBaseAddress();
    //
    //                // Check for blank memory string
    //                if (!memaddress.IsEmpty())
    //                {
    //                    m_actions.Add(new dbg_DAP::DAPGenerateExamineMemory(m_pLogger));
    //                }
    //            }
    //            break;
    //
    //        case MemoryRange:
    //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("DebugWindows MemoryRange called!!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //#ifdef __MINGW32__
    //            if (IsDebuggerPresent())
    //            {
    //                DebugBreak();
    //            }
    //#endif // __MINGW32__
    //            break;
    //
    //        case Watches:
    //            if (IsWindowReallyShown(Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->GetWindow()))
    //            {
    //                DoWatches();
    //            }
    //            break;
    //
    //        default:
    //            break;
    //    }
}

//void Debugger_DAP::GetCurrentPosition(wxString & filename, int & line)
//{
//    m_current_frame.GetPosition(filename, line);
//}

void Debugger_DAP::StripQuotes(wxString & str)
{
    if ((str.GetChar(0) == '\"') && (str.GetChar(str.Length() - 1) == '\"'))
    {
        str = str.Mid(1, str.Length() - 2);
    }
}

void Debugger_DAP::DAPDebuggerResetData(bool bClearAllData)
{
    DAPDebuggerState = eDAPState::NotConnected;
    m_timer_poll_debugger.Stop();

    if (bClearAllData)
    {
        m_breakpoints.clear();
        m_map_filebreakpoints.clear();
    }

    m_backtrace.clear();
    m_dapClient.Reset();
    m_current_frame.Reset();

    if (m_dapPid != 0)
    {
        wxKill(m_dapPid);
        m_dapPid = 0;
    }

    ClearActiveMarkFromAllEditors();
    MarkAsStopped();
    // Notify debugger plugins for end of debug session
    PluginManager * plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_FINISHED);
    plm->NotifyPlugins(evt);
    SwitchToPreviousLayout();
    cbCPURegistersDlg * pDialogCPURegisters = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

    if (pDialogCPURegisters)
    {
        pDialogCPURegisters->Clear();
    }

    cbDisassemblyDlg * pDialogDisassembly = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

    if (pDialogDisassembly)
    {
        cbStackFrame sf;
        pDialogDisassembly->Clear(sf);
    }
}

void Debugger_DAP::ConvertToDAPFriendly(wxString & str)
{
    if (str.IsEmpty())
    {
        return;
    }

    str = UnixFilename(str);

    while (str.Replace("\\", "/"))
        ;

    while (str.Replace("//", "/"))
        ;

    if ((str.Find(' ') != -1) && (str.GetChar(0) != '"'))
    {
        str = "\"" + str + "\"";
    }
}

void Debugger_DAP::ConvertToDAPDirectory(wxString & str, wxString base, bool relative)
{
    if (str.IsEmpty())
    {
        return;
    }

    ConvertToDAPFriendly(str);
    ConvertToDAPFriendly(base);
    StripQuotes(str);
    StripQuotes(base);

    if (platform::windows)
    {
        int  ColonLocation   = str.Find(':');
        bool convert_path_83 = false;

        if (ColonLocation != wxNOT_FOUND)
        {
            convert_path_83 = true;
        }
        else
            if (!base.IsEmpty() && str.GetChar(0) != '/')
            {
                if (base.GetChar(base.Length()) == '/')
                {
                    base = base.Mid(0, base.Length() - 2);
                }

                while (!str.IsEmpty())
                {
                    base += "/" + str.BeforeFirst('/');

                    if (str.Find('/') != wxNOT_FOUND)
                    {
                        str = str.AfterFirst('/');
                    }
                    else
                    {
                        str.Clear();
                    }
                }

                convert_path_83 = true;
            }

        // If can, get 8.3 name for path (Windows only)
        if (convert_path_83 && str.Contains(' ')) // only if has spaces
        {
            wxFileName fn(str); // might contain a file name, too
            wxString path_83 = fn.GetShortPath();

            if (!path_83.IsEmpty())
            {
                str = path_83; // construct filename again
            }
        }

        if (ColonLocation == wxNOT_FOUND || base.IsEmpty())
        {
            relative = false; // Can't do it
        }
    }
    else
    {
        if ((str.GetChar(0) != '/' && str.GetChar(0) != '~') || base.IsEmpty())
        {
            relative = false;
        }
    }

    if (relative)
    {
        if (platform::windows)
        {
            if (str.Find(':') != wxNOT_FOUND)
            {
                str = str.Mid(str.Find(':') + 2, str.Length());
            }

            if (base.Find(':') != wxNOT_FOUND)
            {
                base = base.Mid(base.Find(':') + 2, base.Length());
            }
        }
        else
        {
            if (str.GetChar(0) == '/')
            {
                str = str.Mid(1, str.Length());
            }
            else
            {
                if (str.GetChar(0) == '~')
                {
                    str = str.Mid(2, str.Length());
                }
            }

            if (base.GetChar(0) == '/')
            {
                base = base.Mid(1, base.Length());
            }
            else
            {
                if (base.GetChar(0) == '~')
                {
                    base = base.Mid(2, base.Length());
                }
            }
        }

        while (!base.IsEmpty() && !str.IsEmpty())
        {
            if (str.BeforeFirst('/') == base.BeforeFirst('/'))
            {
                if (str.Find('/') == wxNOT_FOUND)
                {
                    str.Clear();
                }
                else
                {
                    str = str.AfterFirst('/');
                }

                if (base.Find('/') == wxNOT_FOUND)
                {
                    base.Clear();
                }
                else
                {
                    base = base.AfterFirst('/');
                }
            }
            else
            {
                break;
            }
        }

        while (!base.IsEmpty())
        {
            str = "../" + str;

            if (base.Find('/') == wxNOT_FOUND)
            {
                base.Clear();
            }
            else
            {
                base = base.AfterFirst('/');
            }
        }
    }

    ConvertToDAPFriendly(str);
}


wxArrayString Debugger_DAP::ParseSearchDirs(cbProject * pProject)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
    wxArrayString dirs;
    const TiXmlElement * elem = static_cast<const TiXmlElement *>(pProject->GetExtensionsNode());

    if (elem)
    {
        const TiXmlElement * conf = elem->FirstChildElement("debugger");

        if (conf)
        {
            const TiXmlElement * pathsElem = conf->FirstChildElement("search_path");

            while (pathsElem)
            {
                if (pathsElem->Attribute("add"))
                {
                    wxString dir = pathsElem->Attribute("add");

                    if (dirs.Index(dir) == wxNOT_FOUND)
                    {
                        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dir); // apply env vars
                        ConvertToDAPDirectory(dir, "", false);
                        dirs.Add(dir);
                    }
                }

                pathsElem = pathsElem->NextSiblingElement("search_path");
            }
        }
    }

    return dirs;
}

//TiXmlElement* Debugger_DAP::GetElementForSaving(cbProject &project, const char *elementsToClear)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    TiXmlElement *elem = static_cast<TiXmlElement*>(project.GetExtensionsNode());
//
//    // since rev4332, the project keeps a copy of the <Extensions> element
//    // and re-uses it when saving the project (so to avoid losing entries in it
//    // if plugins that use that element are not loaded atm).
//    // so, instead of blindly inserting the element, we must first check it's
//    // not already there (and if it is, clear its contents)
//    TiXmlElement* node = elem->FirstChildElement("debugger");
//    if (!node)
//        node = elem->InsertEndChild(TiXmlElement("debugger"))->ToElement();
//
//    for (TiXmlElement* child = node->FirstChildElement(elementsToClear);
//         child;
//         child = node->FirstChildElement(elementsToClear))
//    {
//        node->RemoveChild(child);
//    }
//    return node;
//}
//
//
//void Debugger_DAP::SetSearchDirs(cbProject &project, const wxArrayString &dirs)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    TiXmlElement* node = GetElementForSaving(project, "search_path");
//    if (dirs.GetCount() > 0)
//    {
//        for (size_t i = 0; i < dirs.GetCount(); ++i)
//        {
//            TiXmlElement* path = node->InsertEndChild(TiXmlElement("search_path"))->ToElement();
//            path->SetAttribute("add", cbU2C(dirs[i]));
//        }
//    }
//}

//dbg_DAP::RemoteDebuggingMap Debugger_DAP::ParseRemoteDebuggingMap(cbProject &project)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    dbg_DAP::RemoteDebuggingMap map;
//    const TiXmlElement* elem = static_cast<const TiXmlElement*>(project.GetExtensionsNode());
//    if (elem)
//    {
//        const TiXmlElement* conf = elem->FirstChildElement("debugger");
//        if (conf)
//        {
//            const TiXmlElement* rdElem = conf->FirstChildElement("remote_debugging");
//            while (rdElem)
//            {
//                wxString targetName = cbC2U(rdElem->Attribute("target"));
//                ProjectBuildTarget* bt = project.GetBuildTarget(targetName);
//
//                const TiXmlElement* rdOpt = rdElem->FirstChildElement("options");
//                if (rdOpt)
//                {
//                    dbg_DAP::RemoteDebugging rd;
//
//                    if (rdOpt->Attribute("conn_type"))
//                    {
//                        rd.connType = (dbg_DAP::RemoteDebugging::ConnectionType)atol(rdOpt->Attribute("conn_type"));
//                    }
//                    if (rdOpt->Attribute("serial_port"))
//                    {
//                        rd.serialPort = cbC2U(rdOpt->Attribute("serial_port"));
//                    }
//                    if (rdOpt->Attribute("serial_baud"))
//                    {
//                        rd.serialBaud = cbC2U(rdOpt->Attribute("serial_baud"));
//                    }
//                    if (rd.serialBaud.empty())
//                    {
//                        rd.serialBaud = "115200";
//                    }
//                    if (rdOpt->Attribute("ip_address"))
//                    {
//                        rd.ip = cbC2U(rdOpt->Attribute("ip_address"));
//                    }
//                    if (rdOpt->Attribute("ip_port"))
//                    {
//                        rd.ipPort = cbC2U(rdOpt->Attribute("ip_port"));
//                    }
//                    if (rdOpt->Attribute("additional_cmds"))
//                    {
//                        rd.additionalCmds = cbC2U(rdOpt->Attribute("additional_cmds"));
//                    }
//                    if (rdOpt->Attribute("additional_cmds_before"))
//                    {
//                        rd.additionalCmdsBefore = cbC2U(rdOpt->Attribute("additional_cmds_before"));
//                    }
//                    if (rdOpt->Attribute("skip_ld_path"))
//                    {
//                        rd.skipLDpath = cbC2U(rdOpt->Attribute("skip_ld_path")) != "0";
//                    }
//                    if (rdOpt->Attribute("extended_remote"))
//                    {
//                        rd.extendedRemote = cbC2U(rdOpt->Attribute("extended_remote")) != "0";
//                    }
//                    if (rdOpt->Attribute("additional_shell_cmds_after"))
//                    {
//                        rd.additionalShellCmdsAfter = cbC2U(rdOpt->Attribute("additional_shell_cmds_after"));
//                    }
//                    if (rdOpt->Attribute("additional_shell_cmds_before"))
//                    {
//                        rd.additionalShellCmdsBefore = cbC2U(rdOpt->Attribute("additional_shell_cmds_before"));
//                    }
//
//                    map.insert(map.end(), std::make_pair(bt, rd));
//                }
//
//                rdElem = rdElem->NextSiblingElement("remote_debugging");
//            }
//        }
//    }
//    return map;
//}
//
//void Debugger_DAP::SetRemoteDebuggingMap(cbProject &project, const dbg_DAP::RemoteDebuggingMap &rdMap)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    TiXmlElement* node = GetElementForSaving(project, "remote_debugging");
//
//    if (!rdMap.empty())
//    {
//        typedef std::map<wxString, const dbg_DAP::RemoteDebugging*> MapTargetNameToRD;
//        MapTargetNameToRD mapTargetNameToRD;
//
//        for (dbg_DAP::RemoteDebuggingMap::const_iterator it = rdMap.begin(); it != rdMap.end(); ++it)
//        {
//            wxString targetName = (it->first ? it->first->GetTitle() : wxString());
//            const dbg_DAP::RemoteDebugging& rd = it->second;
//            mapTargetNameToRD.emplace(targetName, &rd);
//        }
//
//        for (MapTargetNameToRD::const_iterator it = mapTargetNameToRD.begin();
//             it != mapTargetNameToRD.end();
//             ++it)
//        {
//            const dbg_DAP::RemoteDebugging& rd = *it->second;
//
//            // if no different than defaults, skip it
//            if (rd.serialPort.IsEmpty() &&
//                rd.serialBaud == "115200" &&
//                rd.ip.IsEmpty() &&
//                rd.ipPort.IsEmpty() &&
//                !rd.skipLDpath &&
//                !rd.extendedRemote &&
//                rd.additionalCmds.IsEmpty() &&
//                rd.additionalCmdsBefore.IsEmpty() &&
//                rd.additionalShellCmdsAfter.IsEmpty() &&
//                rd.additionalShellCmdsBefore.IsEmpty())
//            {
//                continue;
//            }
//
//            TiXmlElement* rdnode = node->InsertEndChild(TiXmlElement("remote_debugging"))->ToElement();
//            if (!it->first.empty())
//                rdnode->SetAttribute("target", cbU2C(it->first));
//
//            TiXmlElement* tgtnode = rdnode->InsertEndChild(TiXmlElement("options"))->ToElement();
//            tgtnode->SetAttribute("conn_type", (int)rd.connType);
//            if (!rd.serialPort.IsEmpty())
//            {
//                tgtnode->SetAttribute("serial_port", cbU2C(rd.serialPort));
//            }
//            if (rd.serialBaud != "115200")
//            {
//                tgtnode->SetAttribute("serial_baud", cbU2C(rd.serialBaud));
//            }
//            if (!rd.ip.IsEmpty())
//            {
//                tgtnode->SetAttribute("ip_address", cbU2C(rd.ip));
//            }
//            if (!rd.ipPort.IsEmpty())
//            {
//                tgtnode->SetAttribute("ip_port", cbU2C(rd.ipPort));
//            }
//            if (!rd.additionalCmds.IsEmpty())
//            {
//                tgtnode->SetAttribute("additional_cmds", cbU2C(rd.additionalCmds));
//            }
//            if (!rd.additionalCmdsBefore.IsEmpty())
//            {
//                tgtnode->SetAttribute("additional_cmds_before", cbU2C(rd.additionalCmdsBefore));
//            }
//            if (rd.skipLDpath)
//            {
//                tgtnode->SetAttribute("skip_ld_path", "1");
//            }
//            if (rd.extendedRemote)
//            {
//                tgtnode->SetAttribute("extended_remote", "1");
//            }
//            if (!rd.additionalShellCmdsAfter.IsEmpty())
//            {
//                tgtnode->SetAttribute("additional_shell_cmds_after", cbU2C(rd.additionalShellCmdsAfter));
//            }
//            if (!rd.additionalShellCmdsBefore.IsEmpty())
//            {
//                tgtnode->SetAttribute("additional_shell_cmds_before", cbU2C(rd.additionalShellCmdsBefore));
//            }
//        }
//    }
//}

// "================================================================================================"
// "         ____       _     __     __  _____     ____    _____      _      _____   _____          "
// "        / ___|     / \    \ \   / / | ____|   / ___|  |_   _|    / \    |_   _| | ____|         "
// "        \___ \    / _ \    \ \ / /  |  _|     \___ \    | |     / _ \     | |   |  _|           "
// "         ___) |  / ___ \    \ V /   | |___     ___) |   | |    / ___ \    | |   | |___          "
// "        |____/  /_/   \_\    \_/    |_____|   |____/    |_|   /_/   \_\   |_|   |_____|         "
// "                                                                                                "
// "================================================================================================"

bool Debugger_DAP::SaveStateToFile(cbProject * pProject)
{
    //There are two types of state we should save:
    //1, breakpoints
    //2, watches
    //Create a file according to the m_pProject
    wxString projectFilename = pProject->GetFilename();

    if (projectFilename.IsEmpty())
    {
        return false;
    }

    //saved file name&extention
    wxFileName fname(projectFilename);
    fname.SetExt("bps");
    tinyxml2::XMLDocument doc;
    // doc.InsertEndChild(tinyxml2::XMLDeclaration("1.0", "UTF-8", "yes"));
    tinyxml2::XMLNode * rootnode = doc.InsertEndChild(doc.NewElement(XML_CFG_ROOT_TAG));

    if (!rootnode)
    {
        return false;
    }

    // ********************  Save debugger name ********************
    wxString compilerID = pProject->GetCompilerID();
    int compilerIdx = CompilerFactory::GetCompilerIndex(compilerID);
    Compiler * pCompiler = CompilerFactory::GetCompiler(compilerIdx);
    const CompilerPrograms & pCompilerProgsp = pCompiler->GetPrograms();
    tinyxml2::XMLNode * pCompilerNode = rootnode->InsertEndChild(doc.NewElement("CompilerInfo"));
    dbg_DAP::AddChildNode(pCompilerNode, "CompilerName", pCompiler->GetName());
    // dbg_DAP::AddChildNode(pCompilerNode, "C_Compiler", pCompilerProgsp.C);
    // dbg_DAP::AddChildNode(pCompilerNode, "CPP_Compiler",  pCompilerProgsp.CPP);
    // dbg_DAP::AddChildNode(pCompilerNode, "DynamicLinker_LD",  pCompilerProgsp.LD);
    // dbg_DAP::AddChildNode(pCompilerNode, "StaticLinker_LIB",  pCompilerProgsp.LIB);
    // dbg_DAP::AddChildNode(pCompilerNode, "Make",  pCompilerProgsp.MAKE);
    dbg_DAP::AddChildNode(pCompilerNode, "DBGconfig",  pCompilerProgsp.DBGconfig);
    // ******************** Save breakpoints ********************
    tinyxml2::XMLElement * pElementBreakpointList = doc.NewElement("BreakpointsList");
    pElementBreakpointList->SetAttribute("count", static_cast<int64_t>(m_breakpoints.size()));
    tinyxml2::XMLNode * pBreakpointMasterNode = rootnode->InsertEndChild(pElementBreakpointList);

    for (dbg_DAP::DAPBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        dbg_DAP::DAPBreakpoint & bp = **it;

        if (bp.GetProject() == pProject)
        {
            bp.SaveBreakpointToXML(pBreakpointMasterNode);
        }
    }

    // ********************  Save Watches ********************
    tinyxml2::XMLElement * pElementWatchesList = doc.NewElement("WatchesList");
    pElementWatchesList->SetAttribute("count", static_cast<int64_t>(m_watches.size()));
    tinyxml2::XMLNode * pWatchesMasterNode = rootnode->InsertEndChild(pElementWatchesList);

    for (dbg_DAP::DAPWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        dbg_DAP::DAPWatch & watch = **it;

        if (watch.GetProject() == pProject)
        {
            watch.SaveWatchToXML(pWatchesMasterNode);
        }
    }

    // ********************  Save Memory Range Watches ********************
    //    tinyxml2::XMLElement* pElementMemoryRangeList = doc.NewElement("MemoryRangeList");
    //    pElementMemoryRangeList->SetAttribute("count", m_memoryRanges.size());
    //    tinyxml2::XMLNode* pMemoryRangeMasterNode = rootnode->InsertEndChild(pElementMemoryRangeList);
    //
    //    for (dbg_DAP::DAPMemoryRangeWatchesContainer::iterator it = m_memoryRanges.begin(); it != m_memoryRanges.end(); ++it)
    //    {
    //        dbg_DAP::DAPMemoryRangeWatch& memoryRange = **it;
    //
    //        if (memoryRange.GetProject() == pProject)
    //        {
    //            memoryRange.SaveWatchToXML(pMemoryRangeMasterNode);
    //        }
    //    }
    // ********************  Save XML to disk ********************
    return doc.SaveFile(cbU2C(fname.GetFullPath()), false);
}

bool Debugger_DAP::LoadStateFromFile(cbProject * pProject)
{
    wxString projectFilename = pProject->GetFilename();

    if (projectFilename.IsEmpty())
    {
        return false;
    }

    wxFileName fname(projectFilename);
    fname.SetExt("bps");
    //Open XML file
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(cbU2C(fname.GetFullPath()));

    if (eResult != tinyxml2::XMLError::XML_SUCCESS)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Could not open the file '\%s\" due to the error: %s"), fname.GetFullPath(), doc.ErrorIDToName(eResult)), dbg_DAP::LogPaneLogger::LineType::Error);
        return false;
    }

    tinyxml2::XMLElement * root = doc.FirstChildElement(XML_CFG_ROOT_TAG);

    if (!root)
    {
        return false;
    }

    // ******************** Load breakpoints ********************
    tinyxml2::XMLElement * pBreakpointList = root->FirstChildElement("BreakpointsList");

    if (pBreakpointList)
    {
        for (tinyxml2::XMLElement * pBreakpointElement = pBreakpointList->FirstChildElement("Breakpoint");
                pBreakpointElement;
                pBreakpointElement = pBreakpointElement->NextSiblingElement())
        {
            dbg_DAP::DAPBreakpoint * bpNew = new dbg_DAP::DAPBreakpoint(pProject, m_pLogger);
            bpNew->LoadBreakpointFromXML(pBreakpointElement, this);

            // Find new breakpoint in the m_breakpoints
            for (dbg_DAP::DAPBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
            {
                dbg_DAP::DAPBreakpoint & bpSearch = **it;

                if (
                    (bpSearch.GetProject() == bpNew->GetProject())   &&
                    (bpSearch.GetFilename() == bpNew->GetFilename()) &&
                    (bpSearch.GetLine() == bpNew->GetLine())
                )
                {
                    // Found breakpoint, so update it!!!
                    bpSearch.SetEnabled(bpNew->GetIsEnabled());
                    bpSearch.SetIsUseIgnoreCount(bpNew->GetIsUseIgnoreCount());
                    bpSearch.SetIgnoreCount(bpNew->GetIgnoreCount());
                    bpSearch.SetIsUseCondition(bpNew->GetIsUseCondition());
                    bpSearch.SetCondition(bpNew->GetCondition());
                }
            }
        }

        cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dlg->Reload();
    }

    // ******************** Load watches ********************
    tinyxml2::XMLElement * pElementWatchesList = root->FirstChildElement("WatchesList");

    if (pElementWatchesList)
    {
        for (tinyxml2::XMLElement * pWatchElement = pElementWatchesList->FirstChildElement("Watch");
                pWatchElement;
                pWatchElement = pWatchElement->NextSiblingElement())
        {
            wxString DAPWatchClassName = dbg_DAP::ReadChildNodewxString(pWatchElement, "DAPWatchClassName");

            if (DAPWatchClassName.IsSameAs("DAPWatch"))
            {
                dbg_DAP::DAPWatch * watch = new dbg_DAP::DAPWatch(pProject, m_pLogger, "", false);
                watch->LoadWatchFromXML(pWatchElement, this);
            }
        }
    }

    // ******************** Load Memory Range Watches ********************
    //    tinyxml2::XMLElement* pElementMemoryRangeList = root->FirstChildElement("MemoryRangeList");
    //    if (pElementMemoryRangeList)
    //    {
    //        for(    tinyxml2::XMLElement* pWatchElement = pElementMemoryRangeList->FirstChildElement("MemoryRangeWatch");
    //                pWatchElement;
    //                pWatchElement = pWatchElement->NextSiblingElement())
    //        {
    //            wxString DAPMemoryRangeWatchName = dbg_DAP::ReadChildNodewxString(pWatchElement, "DAPMemoryRangeWatch");
    //            if (DAPMemoryRangeWatchName.IsSameAs("DAPMemoryRangeWatch"))
    //            {
    //                dbg_DAP::DAPMemoryRangeWatch* memoryRangeWatch = new dbg_DAP::DAPMemoryRangeWatch(pProject, m_pLogger, 0, 0, wxEmptyString );
    //                memoryRangeWatch->LoadWatchFromXML(pWatchElement, this);
    //            }
    //        }
    //    }
    // ******************** Finished Load ********************
    return true;
}

// "======================================================================================================================="
// "         ____       _      ____        ____    _   _   ____    ____     ___    ____    _____                           "
// "        |  _ \     / \    |  _ \      / ___|  | | | | |  _ \  |  _ \   / _ \  |  _ \  |_   _|                          "
// "        | | | |   / _ \   | |_) |     \___ \  | | | | | |_) | | |_) | | | | | | |_) |   | |                            "
// "        | |_| |  / ___ \  |  __/       ___) | | |_| | |  __/  |  __/  | |_| | |  _ <    | |                            "
// "        |____/  /_/   \_\ |_|         |____/   \___/  |_|     |_|      \___/  |_| \_\   |_|                            "
// "                                                                                                                       "
// "     _____   _   _   _   _    ____   _____   ___    ___    _   _   ____      ____    _____      _      ____    _____   "
// "    |  ___| | | | | | \ | |  / ___| |_   _| |_ _|  / _ \  | \ | | / ___|    / ___|  |_   _|    / \    |  _ \  |_   _|  "
// "    | |_    | | | | |  \| | | |       | |    | |  | | | | |  \| | \___ \    \___ \    | |     / _ \   | |_) |   | |    "
// "    |  _|   | |_| | | |\  | | |___    | |    | |  | |_| | | |\  |  ___) |    ___) |   | |    / ___ \  |  _ <    | |    "
// "    |_|      \___/  |_| \_|  \____|   |_|   |___|  \___/  |_| \_| |____/    |____/    |_|   /_/   \_\ |_| \_\   |_|    "
// "                                                                                                                       "
// "======================================================================================================================="

void Debugger_DAP::OnProcessBreakpointData(const wxString & brkDescription)
{
    if (!brkDescription.IsEmpty())
    {
        wxString::size_type brkLookupIndexStart = brkDescription.find(' ');
        wxString brkLookup = brkDescription.substr(brkLookupIndexStart);

        if (!brkLookup.IsEmpty())
        {
            wxString::size_type brkLookupIndexEnd = brkLookup.find('.');
            wxString brkID = brkLookup.substr(0, brkLookupIndexEnd);

            if (!brkID.IsEmpty())
            {
                long id;

                if (brkID.ToLong(&id, 10))
                {
                    cb::shared_ptr<cbBreakpoint> breakpoint = Debugger_DAP::GetBreakpointByID(id);

                    if (breakpoint)
                    {
                        cb::shared_ptr<dbg_DAP::DAPBreakpoint> bp = cb::static_pointer_cast<dbg_DAP::DAPBreakpoint>(breakpoint);

                        if (bp && !bp->GetFilename().IsEmpty())
                        {
                            //GetDAPCurrentFrame().SetPosition(bp.GetFilename(), bp.GetLine());
                            SyncEditor(bp->GetFilename(), bp->GetLine() + 1, true);
                        }
                    }
                }
            }
        }
    }

    m_dapClient.GetFrames();
}

// "======================================================================================================================="
// "                     ____       _      ____        _____  __     __  _____   _   _   _____   ____                      "
// "                    |  _ \     / \    |  _ \      | ____| \ \   / / | ____| | \ | | |_   _| / ___|                     "
// "                    | | | |   / _ \   | |_) |     |  _|    \ \ / /  |  _|   |  \| |   | |   \___ \                     "
// "                    | |_| |  / ___ \  |  __/      | |___    \ V /   | |___  | |\  |   | |    ___) |                    "
// "                    |____/  /_/   \_\ |_|         |_____|    \_/    |_____| |_| \_|   |_|   |____/                     "
// "                                                                                                                       "
// "     _____   _   _   _   _    ____   _____   ___    ___    _   _   ____      ____    _____      _      ____    _____   "
// "    |  ___| | | | | | \ | |  / ___| |_   _| |_ _|  / _ \  | \ | | / ___|    / ___|  |_   _|    / \    |  _ \  |_   _|  "
// "    | |_    | | | | |  \| | | |       | |    | |  | | | | |  \| | \___ \    \___ \    | |     / _ \   | |_) |   | |    "
// "    |  _|   | |_| | | |\  | | |___    | |    | |  | |_| | | |\  |  ___) |    ___) |   | |    / ___ \  |  _ <    | |    "
// "    |_|      \___/  |_| \_|  \____|   |_|   |___|  \___/  |_| \_| |____/    |____/    |_|   /_/   \_\ |_| \_\   |_|    "
// "                                                                                                                       "
// "======================================================================================================================="

void Debugger_DAP::OnLaunchResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Check that the debugee was started successfully
    dap::LaunchResponse * resp = event.GetDapResponse()->As<dap::LaunchResponse>();

    if (resp)
    {
        if (resp->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Good Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Bad Response: %s"), resp->message), dbg_DAP::LogPaneLogger::LineType::Error);
            // launch failed!
            wxMessageBox("Failed to launch debuggee: " + resp->message, "DAP", wxICON_ERROR | wxOK | wxOK_DEFAULT | wxCENTRE);
            // Reset plugin back to default state!!!!
            DAPDebuggerResetData(false);
        }
    }
}

/// DAP server responded to our `initialize` request
void Debugger_DAP::OnInitializedEvent(DAPEvent & event)
{
    // got initialized event, place breakpoints and continue
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Setup initial breakpoints
    CreateStartBreakpoints(true);
    // Setup initial data watches
    CreateStartWatches();

    // Set breakpoint on "main"
    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::StopOnMain))
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Placing breakpoint at main..."), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_dapClient.SetFunctionBreakpoints({ { "main" } });
    }

    // Check for exception handling
    bool bExceptionCatch =  GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::ExceptionCatch);
    bool bExceptionThrow =  GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::ExceptionThrow);

    if (bExceptionCatch  || bExceptionThrow)
    {
        std::vector<wxString> vExceptionFilters;

        // Available exception filter options for the 'setExceptionBreakpoints' request.
        for (std::vector<dap::ExceptionBreakpointsFilter>::iterator it = vExceptionBreakpointFilters.begin(); it != vExceptionBreakpointFilters.end(); ++it)
        {
            if (
                ((*it).filter.IsSameAs("cpp_catch", false) && bExceptionCatch) ||
                ((*it).filter.IsSameAs("cpp_throw", false) && bExceptionThrow)
            )
            {
                vExceptionFilters.push_back((*it).filter);
            }
        }

        if (vExceptionFilters.size() > 0)
        {
            m_dapClient.SetExceptionBreakpoints(vExceptionFilters);
        }
    }

    // Let DAP server know that we have completed the configuration required
    m_dapClient.ConfigurationDone();
}

#define SHOW_RESPONSE_DATA(msg, OptType, variable)                          \
    if (OptType.has_value())                                    \
    {                                                           \
        m_pLogger->LogDAPMsgType("", -1,                        \
                                 wxString::Format(msg, OptType.value() ?"True":"False"), \
                                 dbg_DAP::LogPaneLogger::LineType::UserDisplay);         \
        variable =  OptType.value();                            \
    }

/// DAP server sent `initialize` reponse to our `initialize` message
void Debugger_DAP::OnInitializeResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::InitializeResponse * response_data = event.GetDapResponse()->As<dap::InitializeResponse>();

    if (response_data)
    {
        if (response_data->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnInitialize Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            SHOW_RESPONSE_DATA(_("supportsBreakpointLocationsRequest: %s"), response_data->capabilities.supportsBreakpointLocationsRequest, supportsBreakpointLocationsRequest);
            SHOW_RESPONSE_DATA(_("supportsCancelRequest: %s"), response_data->capabilities.supportsCancelRequest, supportsCancelRequest);
            SHOW_RESPONSE_DATA(_("supportsClipboardContext: %s"), response_data->capabilities.supportsClipboardContext, supportsClipboardContext);
            SHOW_RESPONSE_DATA(_("supportsCompletionsRequest: %s"), response_data->capabilities.supportsCompletionsRequest, supportsCompletionsRequest);
            SHOW_RESPONSE_DATA(_("supportsConditionalBreakpoints: %s"), response_data->capabilities.supportsConditionalBreakpoints, supportsConditionalBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsConfigurationDoneRequest: %s"), response_data->capabilities.supportsConfigurationDoneRequest, supportsConfigurationDoneRequest);
            SHOW_RESPONSE_DATA(_("supportsDataBreakpoints: %s"), response_data->capabilities.supportsDataBreakpoints, supportsDataBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsDelayedStackTraceLoading: %s"), response_data->capabilities.supportsDelayedStackTraceLoading, supportsDelayedStackTraceLoading);
            SHOW_RESPONSE_DATA(_("supportsDisassembleRequest: %s"), response_data->capabilities.supportsDisassembleRequest, supportsDisassembleRequest);
            SHOW_RESPONSE_DATA(_("supportsEvaluateForHovers: %s"), response_data->capabilities.supportsEvaluateForHovers, supportsEvaluateForHovers);
            SHOW_RESPONSE_DATA(_("supportsExceptionFilterOptions: %s"), response_data->capabilities.supportsExceptionFilterOptions, supportsExceptionFilterOptions);
            SHOW_RESPONSE_DATA(_("supportsExceptionInfoRequest: %s"), response_data->capabilities.supportsExceptionInfoRequest, supportsExceptionInfoRequest);
            SHOW_RESPONSE_DATA(_("supportsExceptionOptions: %s"), response_data->capabilities.supportsExceptionOptions, supportsExceptionOptions);
            SHOW_RESPONSE_DATA(_("supportsFunctionBreakpoints: %s"), response_data->capabilities.supportsFunctionBreakpoints, supportsFunctionBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsGotoTargetsRequest: %s"), response_data->capabilities.supportsGotoTargetsRequest, supportsGotoTargetsRequest);
            SHOW_RESPONSE_DATA(_("supportsHitConditionalBreakpoints: %s"), response_data->capabilities.supportsHitConditionalBreakpoints, supportsHitConditionalBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsInstructionBreakpoints: %s"), response_data->capabilities.supportsInstructionBreakpoints, supportsInstructionBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsLoadedSourcesRequest: %s"), response_data->capabilities.supportsLoadedSourcesRequest, supportsLoadedSourcesRequest);
            SHOW_RESPONSE_DATA(_("supportsLogPoints: %s"), response_data->capabilities.supportsLogPoints, supportsLogPoints);
            SHOW_RESPONSE_DATA(_("supportsModulesRequest: %s"), response_data->capabilities.supportsModulesRequest, supportsModulesRequest);
            SHOW_RESPONSE_DATA(_("supportsReadMemoryRequest: %s"), response_data->capabilities.supportsReadMemoryRequest, supportsReadMemoryRequest);
            SHOW_RESPONSE_DATA(_("supportsRestartFrame: %s"), response_data->capabilities.supportsRestartFrame, supportsRestartFrame);
            SHOW_RESPONSE_DATA(_("supportsRestartRequest: %s"), response_data->capabilities.supportsRestartRequest, supportsRestartRequest);
            SHOW_RESPONSE_DATA(_("supportsSetExpression: %s"), response_data->capabilities.supportsSetExpression, supportsSetExpression);
            SHOW_RESPONSE_DATA(_("supportsSetVariable: %s"), response_data->capabilities.supportsSetVariable, supportsSetVariable);
            SHOW_RESPONSE_DATA(_("supportsSingleThreadExecutionRequests: %s"), response_data->capabilities.supportsSingleThreadExecutionRequests, supportsSingleThreadExecutionRequests);
            SHOW_RESPONSE_DATA(_("supportsStepBack: %s"), response_data->capabilities.supportsStepBack, supportsStepBack);
            SHOW_RESPONSE_DATA(_("supportsStepInTargetsRequest: %s"), response_data->capabilities.supportsStepInTargetsRequest, supportsStepInTargetsRequest);
            SHOW_RESPONSE_DATA(_("supportsSteppingGranularity: %s"), response_data->capabilities.supportsSteppingGranularity, supportsSteppingGranularity);
            SHOW_RESPONSE_DATA(_("supportsTerminateRequest: %s"), response_data->capabilities.supportsTerminateRequest, supportsTerminateRequest);
            SHOW_RESPONSE_DATA(_("supportsTerminateThreadsRequest: %s"), response_data->capabilities.supportsTerminateThreadsRequest, supportsTerminateThreadsRequest);
            SHOW_RESPONSE_DATA(_("supportSuspendDebuggee: %s"), response_data->capabilities.supportSuspendDebuggee, supportSuspendDebuggee);
            SHOW_RESPONSE_DATA(_("supportsValueFormattingOptions: %s"), response_data->capabilities.supportsValueFormattingOptions, supportsValueFormattingOptions);
            SHOW_RESPONSE_DATA(_("supportsWriteMemoryRequest: %s"), response_data->capabilities.supportsWriteMemoryRequest, supportsWriteMemoryRequest);
            SHOW_RESPONSE_DATA(_("supportTerminateDebuggee: %s"), response_data->capabilities.supportTerminateDebuggee, supportTerminateDebuggee);

            if (response_data->capabilities.exceptionBreakpointFilters.has_value())
            {
                // Available exception filter options for the 'setExceptionBreakpoints' request.
                vExceptionBreakpointFilters = response_data->capabilities.exceptionBreakpointFilters.value();

                for (std::vector<dap::ExceptionBreakpointsFilter>::iterator it = vExceptionBreakpointFilters.begin(); it != vExceptionBreakpointFilters.end(); ++it)
                {
                    if ((*it).default_value.has_value())
                    {
                        m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("exceptionBreakpointFilters - filter:\"%s\"  label:\"%s\" value:%s"), (*it).filter, (*it).label, (*it).default_value.value() ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                    }
                    else
                    {
                        m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("exceptionBreakpointFilters - filter:\"%s\"  label:\"%s\" value:missing"), (*it).filter, (*it).label), dbg_DAP::LogPaneLogger::LineType::Error);
                    }
                }
            }

            if (response_data->capabilities.completionTriggerCharacters.has_value())
            {
                // The set of characters that should trigger completion in a REPL. If not specified, the UI should assume the '.' character.
                vCompletionTriggerCharacters = response_data->capabilities.completionTriggerCharacters.value();

                for (std::vector<wxString>::iterator it = vCompletionTriggerCharacters.begin(); it != vCompletionTriggerCharacters.end(); ++it)
                {
                    m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("completionTriggerCharacters - %s"), *it), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                }
            }

            if (response_data->capabilities.additionalModuleColumns.has_value())
            {
                // The set of additional module information exposed by the debug adapter.
                vAdditionalModuleColumns = response_data->capabilities.additionalModuleColumns.value();
                m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("additionalModuleColumns - future display data ")), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            }

            m_dapClient.Launch(std::move(m_DAP_DebuggeeStartCMD));
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got BAD OnInitialize Response"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }
}


void Debugger_DAP::OnConfigurationDoneResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

/// DAP server stopped. This can happen for multiple reasons:
/// - exception
/// - breakpoint hit
/// - step (user previously issued `Next` command)
void Debugger_DAP::OnStopped(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    DAPDebuggerState = eDAPState::Stopped;
    // got stopped event
    dap::StoppedEvent * stopped_data = event.GetDapEvent()->As<dap::StoppedEvent>();

    if (stopped_data)
    {
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("text: %s"),  stopped_data->text), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("description: %s"),  stopped_data->description), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("All threads stopped: %s"),  stopped_data->allThreadsStopped ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped thread ID: %d (active thread ID: %d)"), stopped_data->threadId, m_dapClient.GetActiveThreadId()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        if (
            (stopped_data->reason.IsSameAs("breakpoint")) ||
            (stopped_data->reason.IsSameAs("step")) ||
            (stopped_data->reason.IsSameAs("goto"))
        )
        {
            /* reason:
            * Values: 'step', 'breakpoint', 'goto',
            */
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            OnProcessBreakpointData(stopped_data->description);
        }
        else
        {
            /* reason:
            * Values: 'exception', 'pause', 'entry', 'function breakpoint', 'data breakpoint', etc
            */
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s not suported"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }

    MarkAsStopped();
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnScopes(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::ScopesResponse * resp = event.GetDapResponse()->As<dap::ScopesResponse>();

    if (resp)
    {
        m_stackdapvariables.clear();

        for (const dap::Scope & scope : resp->scopes)
        {
            if (
                //(scope.name.IsSameAs("Locals", false) /* &&  scope.presentationHint.IsSameAs("Locals", false) */)
                //||
                (scope.name.IsSameAs("Globals", false))
                ||
                (scope.name.IsSameAs("Registers", false) /*&&  scope.presentationHint.IsSameAs("Registers", false)*/)
            )
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Do not request variables for %s."), scope.name), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            }
            else
            {
                m_dapClient.GetChildrenVariables(scope.variablesReference);
            }
        }
    }
}

void Debugger_DAP::OnVariables(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::VariablesResponse * resp = event.GetDapResponse()->As<dap::VariablesResponse>();

    if (resp)
    {
        for (const dap::Variable & var : resp->variables)
        {
            m_stackdapvariables.push_back(var);
#if 1
            wxString button = (var.variablesReference > 0 ? "> " : "  ");
            wxString value = var.value.empty() ? "\"\"" : var.value;
            wxString attributes = wxEmptyString;

            for (const auto & attrib : var.presentationHint.attributes)
            {
                attributes += " " + attrib;
            }

            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("Var: %s  (%d) %s = %s , Type: %s, Hint: kind: %s , attributes %s , visibility: %s "),
                                                      button,
                                                      var.variablesReference,
                                                      var.name,
                                                      value,
                                                      var.type,
                                                      var.presentationHint.kind,
                                                      attributes,
                                                      var.presentationHint.visibility
                                                     ),
                                     dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
        }

        for (dbg_DAP::DAPWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        {
            wxString symbol = (*it)->GetSymbol();

            for (const dap::Variable & var : resp->variables)
            {
                if (symbol.IsSameAs(var.name))
                {
                    wxString value = var.value.empty() ? "\"\"" : var.value;
                    (*it)->SetValue(value);
                    (*it)->SetType(var.type);
                }
            }
        }

        UpdateDAPWatches(int(cbDebuggerPlugin::DebugWindows::Watches));
    }
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnStackTrace(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::StackTraceResponse * stack_trace_data = event.GetDapResponse()->As<dap::StackTraceResponse>();

    if (stack_trace_data)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received stack trace event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        int stackID = 0;
        bool bFileFound = false;
        m_backtrace.clear();

        for (const auto & stack : stack_trace_data->stackFrames)
        {
#if 1
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("Stack: ID 0x%X , Name: %s , File: %s  %d"),
                                                      stack.id,
                                                      stack.name,
                                                      stack.source.path.IsEmpty() ? stack.source.name : stack.source.path,
                                                      stack.line
                                                     ),
                                     dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
            wxString sFileName = stack.source.path;

            if (!sFileName.IsEmpty() && !bFileFound)
            {
                wxFileName fnFileName(sFileName);

                if (fnFileName.Exists())
                {
                    bFileFound = true;
                    int lineNumber = stack.line;
                    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SyncEditor: %s %d"), sFileName, lineNumber), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                    SyncEditor(sFileName, lineNumber, true);
                    // request the scopes for the first stack
                    m_dapClient.GetScopes(stack.id);
                }
            }

            cbStackFrame s;
            s.SetNumber(stackID++);
            s.SetFile(stack.source.path, wxString::Format("%d", stack.line));
            s.SetSymbol(stack.name);
            s.SetAddress(stack.id);
            s.MakeValid(true);
            m_backtrace.push_back(cb::shared_ptr<cbStackFrame>(new cbStackFrame(s)));
        }

        cbBacktraceDlg * pDialogBacktrace = Manager::Get()->GetDebuggerManager()->GetBacktraceDialog();

        if (pDialogBacktrace)
        {
            pDialogBacktrace->Reload();
        }
    }
}

/// Debuggee process exited, print the exit code
void Debugger_DAP::OnExited(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    DAPDebuggerState = eDAPState::NotConnected;
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Debuggee exited. Exit code: %d"), event.GetDapEvent()->As<dap::ExitedEvent>()->exitCode));
}

/// Debug session terminated
void Debugger_DAP::OnTerminated(DAPEvent & event)
{
    wxUnusedVar(event);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event for debugger terminated!"), dbg_DAP::LogPaneLogger::LineType::Warning);
    // Reset the client and data
    DAPDebuggerResetData(false);
}

void Debugger_DAP::OnOutput(DAPEvent & event)
{
    dap::OutputEvent * output_data = event.GetDapEvent()->As<dap::OutputEvent>();

    if (output_data)
    {
        wxString msg(output_data->output);
        msg.Replace("\r\n", "");
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("%s - %s", output_data->category, msg), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
}

void Debugger_DAP::OnBreakpointLocations(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::BreakpointLocationsResponse * d = event.GetDapResponse()->As<dap::BreakpointLocationsResponse>();

    if (d)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("==> Breakpoints:\n"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        for (const auto & bp : d->breakpoints)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _(d->filepath << ":" << bp.line), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
    }
}

void Debugger_DAP::OnConnectionError(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    wxUnusedVar(event);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("OnConnectionError!")), dbg_DAP::LogPaneLogger::LineType::Error);
    wxMessageBox(_("Lost connection to dap server"));
    event.Skip();
    DAPDebuggerResetData(false);
}

void Debugger_DAP::OnBreakpointDataSet(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::SetBreakpointsResponse * resp = event.GetDapResponse()->As<dap::SetBreakpointsResponse>();

    if (resp)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Got reply for setBreakpoint command for file: "), resp->originSource), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        for (const auto & bp : resp->breakpoints)
        {
            dbg_DAP::LogPaneLogger::LineType logType = dbg_DAP::LogPaneLogger::LineType::UserDisplay;

            if ((bp.line == -1) || !bp.verified || bp.source.path.IsEmpty())
            {
                logType = dbg_DAP::LogPaneLogger::LineType::Error;
            }

            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("ID %d , Verified: %s , File: %s , Line: %d, Message: %s"), bp.id, bp.verified ? "True" : "False", bp.source.path, bp.line, bp.message),
                                     logType
                                    );

            if ((bp.line != -1) && bp.verified && !bp.source.path.IsEmpty())
            {
                UpdateOrAddBreakpoint(bp.source.path, bp.line, bp.id);
            }
        }
    }
}

void Debugger_DAP::OnBreakpointFunctionSet(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::SetBreakpointsResponse * resp = event.GetDapResponse()->As<dap::SetBreakpointsResponse>();

    if (resp)
    {
        if (resp->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnBreakpointFunctionSet event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got BAD OnBreakpointFunctionSet Response"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }
}

void Debugger_DAP::OnDapLog(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, event.GetString(), dbg_DAP::LogPaneLogger::LineType::Debug);
}

void Debugger_DAP::OnDapModuleEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::ModuleEvent * event_data = event.GetDapEvent()->As<dap::ModuleEvent>();

    if (event_data)
    {
        //wxString log_entry;
        //log_entry << event_data->module.id << ": " << event_data->module.name << " " << event_data->module.symbolStatus << event_data->module.version << " " << event_data->module.path;
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, log_entry, dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
}

void Debugger_DAP::OnRunInTerminalRequest(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Handling `OnRunInTerminalRequest` event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    auto request = event.GetDapRequest()->As<dap::RunInTerminalRequest>();

    if (!request)
    {
        return;
    }

    wxString command;

    for (const wxString & cmd : request->arguments.args)
    {
        command << cmd << " ";
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Starting process: %s"), command), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    m_DAPTerminalProcess = dap::ExecuteProcess(command);
    dap::RunInTerminalResponse response = m_dapClient.MakeRequest<dap::RunInTerminalResponse>();
    response.request_seq = request->seq;

    if (!m_DAPTerminalProcess)
    {
        response.success = false;
        response.processId = 0;
    }
    else
    {
        response.success = true;
        response.processId = m_DAPTerminalProcess->GetProcessId();
    }

    m_dapClient.SendResponse(response);
}

void Debugger_DAP::OnTreadResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnStopOnEntryEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnProcessEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnBreakpointEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnCcontinuedEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnDebugPYWaitingForServerEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

// "==================================================================================================================="
// "          ____       _      ____      _____  __     __  _____   _   _   _____   ____      _____   _   _   ____     "
// "         |  _ \     / \    |  _ \    | ____| \ \   / / | ____| | \ | | |_   _| / ___|    | ____| | \ | | |  _ \    "
// "         | | | |   / _ \   | |_) |   |  _|    \ \ / /  |  _|   |  \| |   | |   \___ \    |  _|   |  \| | | | | |   "
// "         | |_| |  / ___ \  |  __/    | |___    \ V /   | |___  | |\  |   | |    ___) |   | |___  | |\  | | |_| |   "
// "         |____/  /_/   \_\ |_|       |_____|    \_/    |_____| |_| \_|   |_|   |____/    |_____| |_| \_| |____/    "
// "                                                                                                                   "
// "==================================================================================================================="


// Windows: C:\msys64\mingw64\bin\lldb-vscode.exe -port 12345
// Linux:  /usr/bin/lldb-vscode-14 -port 12345
// MACOS:: /usr/local/opt/llvm/bin/lldb-vscode -port 12345
// --personality=debuging --multiple-instance
