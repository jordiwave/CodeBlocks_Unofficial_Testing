/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

// System include files
#include <algorithm>
#include <tinyxml2.h>
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
#include "sdk.h" // Code::Blocks SDK

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
    DAPDebuggerState(DAPState::NotConnected)
{
    if (!Manager::LoadResource("debugger_dap.zip"))
    {
        NotifyMissingFile("debugger_dap.zip");
    }

    m_pLogger = new dbg_DAP::LogPaneLogger(this);
    // bind the client events
    m_dapClient.Bind(wxEVT_DAP_STOPPED_EVENT,                   &Debugger_DAP::OnStopped,              this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZED_EVENT,               &Debugger_DAP::OnInitializedEvent,     this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZE_RESPONSE,             &Debugger_DAP::OnInitializeResponse,   this);
    m_dapClient.Bind(wxEVT_DAP_EXITED_EVENT,                    &Debugger_DAP::OnExited,               this);
    m_dapClient.Bind(wxEVT_DAP_TERMINATED_EVENT,                &Debugger_DAP::OnTerminated,           this);
    m_dapClient.Bind(wxEVT_DAP_STACKTRACE_RESPONSE,             &Debugger_DAP::OnStackTrace,           this);
    m_dapClient.Bind(wxEVT_DAP_SCOPES_RESPONSE,                 &Debugger_DAP::OnScopes,               this);
    m_dapClient.Bind(wxEVT_DAP_VARIABLES_RESPONSE,              &Debugger_DAP::OnVariables,            this);
    m_dapClient.Bind(wxEVT_DAP_OUTPUT_EVENT,                    &Debugger_DAP::OnOutput,               this);
    m_dapClient.Bind(wxEVT_DAP_BREAKPOINT_LOCATIONS_RESPONSE,   &Debugger_DAP::OnBreakpointLocations,  this);
    m_dapClient.Bind(wxEVT_DAP_LOST_CONNECTION,                 &Debugger_DAP::OnConnectionError,      this);
    m_dapClient.Bind(wxEVT_DAP_SET_SOURCE_BREAKPOINT_RESPONSE,  &Debugger_DAP::OnBreakpointSet,        this);
    m_dapClient.Bind(wxEVT_DAP_SET_FUNCTION_BREAKPOINT_RESPONSE, &Debugger_DAP::OnBreakpointSet,        this);
    m_dapClient.Bind(wxEVT_DAP_LAUNCH_RESPONSE,                 &Debugger_DAP::OnLaunchResponse,       this);
    m_dapClient.Bind(wxEVT_DAP_RUN_IN_TERMINAL_REQUEST,         &Debugger_DAP::OnRunInTerminalRequest, this);
    m_dapClient.Bind(wxEVT_DAP_LOG_EVENT,                       &Debugger_DAP::OnDapLog,               this);
}

// destructor
Debugger_DAP::~Debugger_DAP()
{
    if (m_dapPid >= 0)
    {
        wxKill(m_dapPid);
        m_dapPid = -1;
    }
}

void Debugger_DAP::OnAttachReal()
{
    m_timer_poll_debugger.SetOwner(this, id_gdb_poll_timer);
    DebuggerManager & dbg_manager = *Manager::Get()->GetDebuggerManager();
    dbg_manager.RegisterDebugger(this);
    // Do no use cbEVT_PROJECT_OPEN as the project may not be active!!!!
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,  new cbEventFunctor<Debugger_DAP, CodeBlocksEvent>(this, &Debugger_DAP::OnProjectOpened));
}

void Debugger_DAP::OnReleaseReal(bool appShutDown)
{
    Manager::Get()->GetDebuggerManager()->UnregisterDebugger(this);
    KillDAPDebugger();
    //    if (m_command_stream_dialog)
    //    {
    //        m_command_stream_dialog->Destroy();
    //        m_command_stream_dialog = nullptr;
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
        case cbDebuggerFeature::CPURegisters:
        case cbDebuggerFeature::Disassembly:
        case cbDebuggerFeature::ExamineMemory:
        case cbDebuggerFeature::Threads:
        case cbDebuggerFeature::Watches:
        case cbDebuggerFeature::RunToCursor:
        case cbDebuggerFeature::SetNextStatement:
        case cbDebuggerFeature::ValueTooltips:
            return true;

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
                m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Selecting target cancelled"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
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
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("The selected target is only running pre/post build step commands,Can't debug such a target... ")), dbg_DAP::LogPaneLogger::LineType::Error);
            return false;
        }

        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Selecting target: %s"), target->GetTitle()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        // find the target's compiler (to see which debugger to use)
        compiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : project.GetCompilerID());
    }
    else
    {
        compiler = CompilerFactory::GetDefaultCompiler();
    }

    return true;
}

void Debugger_DAP::OnIdle(wxIdleEvent & event)
{
    //    if (m_executor.IsStopped() && m_executor.IsRunning())
    //    {
    //        m_actions.Run(m_executor);
    //    }
    //
    //    if (m_executor.ProcessHasInput())
    //    {
    //        event.RequestMore();
    //    }
    //    else
    //    {
    //        event.Skip();
    //    }
}

void Debugger_DAP::OnTimer(wxTimerEvent & /*event*/)
{
    wxWakeUpIdle();
}

void Debugger_DAP::OnMenuInfoCommandStream(wxCommandEvent & /*event*/)
{
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
    //        m_command_stream_dialog = new dbg_DAP::GDBTextInfoWindow(Manager::Get()->GetAppWindow(), _T("Command stream"), full);
    //        m_command_stream_dialog->Show();
    //    }
}

void Debugger_DAP::UpdateOnFrameChanged(bool wait)
{
    //    if (wait)
    //    {
    //        m_actions.Add(new dbg_DAP::GDBBarrierAction);
    //    }
    //    DebuggerManager * dbg_manager = Manager::Get()->GetDebuggerManager();
    //
    //    if (IsWindowReallyShown(dbg_manager->GetWatchesDialog()->GetWindow()) && !m_watches.empty())
    //    {
    //        for (dbg_DAP::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    //        {
    //            if ((*it)->GetID().empty() && !(*it)->ForTooltip())
    //            {
    //                m_actions.Add(new dbg_DAP::GDBWatchCreateAction(*it, m_watches, m_pLogger, true));
    //            }
    //        }
    //
    //        m_actions.Add(new dbg_DAP::GDBWatchesUpdateAction(m_watches, m_pLogger));
    //    }
}

void Debugger_DAP::UpdateWhenStopped()
{
    DebuggerManager * dbg_manager = Manager::Get()->GetDebuggerManager();

    if (dbg_manager->UpdateBacktrace())
    {
        RequestUpdate(Backtrace);
    }

    if (dbg_manager->UpdateThreads())
    {
        RequestUpdate(Threads);
    }

    if (dbg_manager->UpdateCPURegisters())
    {
        RequestUpdate(CPURegisters);
    }

    if (dbg_manager->UpdateExamineMemory())
    {
        RequestUpdate(ExamineMemory);
    }

    if (dbg_manager->UpdateDisassembly())
    {
        RequestUpdate(Disassembly);
    }

    if (IsWindowReallyShown(dbg_manager->GetWatchesDialog()->GetWindow()))
    {
        RequestUpdate(Watches);
    }

    UpdateOnFrameChanged(false);
}

bool Debugger_DAP::Debug(bool breakOnEntry)
{
    m_hasStartUpError = false;
    ProjectManager & project_manager = *Manager::Get()->GetProjectManager();
    cbProject * project = project_manager.GetActiveProject();

    if (!project)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_DAP::LogPaneLogger::LineType::Error);
        return false;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("starting debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
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
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }

    return false;
}

void Debugger_DAP::ConvertDirectory(wxString & str, wxString base, bool relative)
{
    //    dbg_DAP::ConvertDirectory(str, base, relative);
}

struct BreakpointMatchProject
{
    BreakpointMatchProject(cbProject * project) : project(project) {}
    bool operator()(cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp) const
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

    // the same for remote debugging
    // GetRemoteDebuggingMap(event.GetProject()).clear();
    dbg_DAP::GDBBreakpointsContainer::iterator bpIT = std::remove_if(m_breakpoints.begin(), m_breakpoints.end(), BreakpointMatchProject(project));

    if (bpIT != m_breakpoints.end())
    {
        m_breakpoints.erase(bpIT, m_breakpoints.end());
        cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dlg->Reload();
    }

    m_map_filebreakpoints.clear();

    for (dbg_DAP::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end();)
    {
        cb::shared_ptr<dbg_DAP::GDBWatch> watch = *it;

        if (watch->GetProject() == project)
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Remove watch for \"%s\"", watch->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
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
        cbBacktraceDlg * pDialogBacktrace = Manager::Get()->GetDebuggerManager()->GetBacktraceDialog();

        if (pDialogBacktrace)
        {
            pDialogBacktrace->Reload();
        }

        cbBreakpointsDlg * pDialogBreakpoint = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();

        if (pDialogBreakpoint)
        {
            pDialogBreakpoint->RemoveAllBreakpoints();
        }

        cbExamineMemoryDlg * pDialogExamineMemory = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();

        if (pDialogExamineMemory)
        {
            pDialogExamineMemory->SetBaseAddress("");
            pDialogExamineMemory->Clear();
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
    }
}

int Debugger_DAP::StartDebugger(cbProject * project, StartType start_type)
{
    //    ShowLog(true);
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);

    if (!compiler)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no compiler found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 2;
    }

    if (!target)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no target found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 3;
    }

    // is gdb accessible, i.e. can we find it?
    wxString dap_debugger = GetActiveConfigEx().GetDAPExecutable(true);
    wxString dap_port_number = GetActiveConfigEx().GetDAPPortNumber();

    if (dap_port_number.IsEmpty())
    {
        dap_port_number = "12345";
    }

    wxString debuggee, working_dir;

    if (!GetDebuggee(debuggee, working_dir, target))
    {
        m_hasStartUpError = true;
        return 6;
    }

    bool console = target->GetTargetType() == ttConsoleOnly;
    wxString oldLibPath;
    wxGetEnv(CB_LIBRARY_ENVVAR, &oldLibPath);
    wxString newLibPath = GetLibraryPath(oldLibPath, compiler, target, project);

    if (oldLibPath != newLibPath)
    {
        wxSetEnv(CB_LIBRARY_ENVVAR, newLibPath);
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("wxSetEnv(%s , %s"), CB_LIBRARY_ENVVAR, newLibPath),
                                 dbg_DAP::LogPaneLogger::LineType::Debug);
    }

    int res = LaunchDebugger(project, dap_debugger, debuggee, dap_port_number, working_dir, 0, console, start_type);

    if (res != 0)
    {
        m_hasStartUpError = true;
        return res;
    }

    m_pProject = project;
    m_hasStartUpError = false;

    if (oldLibPath != newLibPath)
    {
        wxSetEnv(CB_LIBRARY_ENVVAR, oldLibPath);
    }

    return 0;
}

int Debugger_DAP::LaunchDebugger(cbProject * project,
                                 wxString const & dap_debugger,
                                 wxString const & debuggee,
                                 wxString const & dap_port_number,
                                 wxString const & working_dir,
                                 int pid,
                                 bool console,
                                 StartType start_type)
{
    m_current_frame.Reset();

    if (dap_debugger.IsEmpty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no debugger executable found (full path)!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 5;
    }

    wxBusyCursor cursor;
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_debugger: %s"), dap_debugger), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

    if (!dap_port_number.IsEmpty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_port_number: %s"), dap_port_number), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }

    if (!debuggee.IsEmpty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE: %s"), debuggee), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }

    const wxString shell(Manager::Get()->GetConfigManager("app")->Read("/console_shell", DEFAULT_CONSOLE_SHELL));
    wxString dapStartCmd;

    if (platform::windows)
    {
        dapStartCmd.Format("%s /k \'%s\'", shell, dap_debugger);
    }
    else
    {
        dapStartCmd.Format("%s \'%s\'", shell, dap_debugger);
    }

    if (!dap_port_number.IsEmpty())
    {
        dapStartCmd << " -port " << dap_port_number;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dapStartCmd: %s"), dapStartCmd), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // start the dap_debugger process
    m_dapPid = wxExecute(dapStartCmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
    // Reset the client
    m_dapClient.Reset();
    // For this demo, we use socket transport. But you may choose
    // to write your own transport that implements the dap::Transport interface
    // This is useful when the user wishes to use stdin/out for communicating with
    // the dap and not over socket
    dap::SocketTransport * transport = new dap::SocketTransport();
    wxString connection = wxString::Format("tcp://127.0.0.1:%s", dap_port_number);

    if (!transport->Connect(connection, 10))
        //if(!transport->Connect("tcp://127.0.0.1:12345", 10))
    {
        if (m_dapPid >= 0)
        {
            wxKill(m_dapPid);
            m_dapPid = -1;
        }

        wxMessageBox("Failed to connect to DAP server", "DAP Demo", wxICON_ERROR | wxOK | wxCENTRE);
        return 1;
    }

    DAPDebuggerState = DAPState::Connected;
    // construct new client with the transport
    m_dapClient.SetTransport(transport);
    // This part is done in mode **sync**
    DAPDebuggerState = DAPState::Running;
    m_dap_debuggee = debuggee;
    wxFileName fndebugee(debuggee);
    m_dap_debuggeepath = fndebugee.GetPath();
    // The protocol starts by us sending an initialize request
    dap::InitializeRequestArguments args;
    args.linesStartAt1 = true;
    args.clientID = "CB_DAP_Plugin";
    args.clientName = "CB_DAP_Plugin";
    m_dapClient.Initialize(&args);
    //    // Set program arguments
    //    m_actions.Add(new dbg_DAP::GDBSimpleAction("-exec-arguments " + args));
    //
    //    wxArrayString comandLines = GetArrayFromString(active_config.GetInitialCommands(), '\n');
    //    size_t CommandLineCount = comandLines.GetCount();
    //
    //    for (unsigned int i = 0; i < CommandLineCount; ++i)
    //    {
    //        DoSendCommand(comandLines[i]);
    //    }
    //
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
    //    SwitchToDebuggingLayout();
    return 0;
}

void Debugger_DAP::CreateStartBreakpoints(bool force)
{
    long line;
    std::map<wxString, dbg_DAP::GDBBreakpointsContainer>::iterator mapit;

    for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        // FIXME (obfuscated#): pointers inside the vector can be dangerous!!!
        if ((*it)->GetIndex() == -1 || force)
        {
            if ((*it)->GetLineString().ToLong(&line))
            {
                wxFileName absfnfilename((*it)->GetLocation());
                mapit = m_map_filebreakpoints.find(absfnfilename.GetFullName());

                if (mapit != m_map_filebreakpoints.end())
                {
                    mapit->second.push_back(*it);
                }
                else
                {
                    std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>> filebreakpoint;
                    filebreakpoint.push_back(*it);
                    m_map_filebreakpoints.insert(std::pair<wxString, std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>>>(absfnfilename.GetFullName(), filebreakpoint));
                }

                wxFileName relfnfilename(absfnfilename);
                relfnfilename.MakeRelativeTo(m_dap_debuggeepath);
                wxString relfilename = relfnfilename.GetFullName();
                m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("CreateStartBreakpoints: %s %d", (*it)->GetLocation(), line), dbg_DAP::LogPaneLogger::LineType::Debug);
                m_dapClient.SetBreakpointsFile(relfilename, { { static_cast<int>(line), wxEmptyString } });
            }
        }
    }

    for (dbg_DAP::GDBBreakpointsContainer::const_iterator it = m_temporary_breakpoints.begin(); it != m_temporary_breakpoints.end(); ++it)
    {
        if ((*it)->GetLineString().ToLong(&line))
        {
            wxFileName absfnfilename((*it)->GetLocation());
            mapit = m_map_filebreakpoints.find(absfnfilename.GetFullName());

            if (mapit != m_map_filebreakpoints.end())
            {
                mapit->second.push_back(*it);
            }
            else
            {
                std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>> filebreakpoint;
                filebreakpoint.push_back(*it);
                m_map_filebreakpoints.insert(std::pair<wxString, std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>>>(absfnfilename.GetFullName(), filebreakpoint));
            }

            wxFileName relfnfilename(absfnfilename);
            relfnfilename.MakeRelativeTo(m_dap_debuggeepath);
            wxString relfilename = relfnfilename.GetFullName();
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("CreateStartBreakpoints temp: %s %d)", (*it)->GetLocation(), line), dbg_DAP::LogPaneLogger::LineType::Debug);
            m_dapClient.SetBreakpointsFile(relfilename, { { static_cast<int>(line), wxEmptyString } });
        }
    }

    m_temporary_breakpoints.clear();
}

void Debugger_DAP::CreateStartWatches()
{
    if (m_watches.empty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "No watches", dbg_DAP::LogPaneLogger::LineType::Debug);
    }

    for (dbg_DAP::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Watch clear for symbol %s", (*it)->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
        (*it)->Reset();
    }

    if (!m_watches.empty())
    {
        CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
        event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
        Manager::Get()->ProcessEvent(event);
    }
}

bool Debugger_DAP::RunToCursor(const wxString & filename, int line, const wxString & /*line_text*/)
{
    if (IsRunning())
    {
        //        if (IsStopped())
        //        {
        //            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("=>-exec-until %s:%d<=", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        //            CommitRunCommand(wxString::Format("-exec-until %s:%d", filename.c_str(), line));
        //            return true;
        //        }
        //        else
        //        {
        //            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("filename:%s line:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
        //        }
        //
        return false;
    }
    else
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("RunToCursor %s:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);
        cb::shared_ptr<dbg_DAP::GDBBreakpoint> ptr(new dbg_DAP::GDBBreakpoint(project, m_pLogger, filename, line, -1));
        m_temporary_breakpoints.push_back(ptr);
        return Debug(false);
    }
}

void Debugger_DAP::SetNextStatement(const wxString & filename, int line)
{
    if (IsStopped())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("-break-insert -t & -exec-jump for filename:=>%s<= line:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        //        AddStringCommand(wxString::Format("-break-insert -t %s:%d", filename.c_str(), line));
    }
}

void Debugger_DAP::Continue()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Continue", dbg_DAP::LogPaneLogger::LineType::Debug);
    m_dapClient.Continue();
}

void Debugger_DAP::Next()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Next", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Next();
}

void Debugger_DAP::NextInstruction()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "NextInstruction", dbg_DAP::LogPaneLogger::LineType::Command);
    // m_dapClient.Next();
}

void Debugger_DAP::StepIntoInstruction()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::StepIn", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.StepIn();
}

void Debugger_DAP::Step()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Step", dbg_DAP::LogPaneLogger::LineType::Command);
    // m_dapClient.Next();
}

void Debugger_DAP::StepOut()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::StepOut", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.StepOut();
}

void Debugger_DAP::Break()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Break", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Pause();
}

void Debugger_DAP::Stop()
{
    if (!IsRunning())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger failed as not running!!!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    ClearActiveMarkFromAllEditors();
    MarkAsStopped();
    m_dapClient.Reset();
    DAPDebuggerState = DAPState::NotConnected;
}

bool Debugger_DAP::IsRunning() const
{
    return (DAPDebuggerState != DAPState::NotConnected);
}

bool Debugger_DAP::IsStopped() const
{
    return (DAPDebuggerState == DAPState::Stopped);
}

bool Debugger_DAP::IsBusy() const
{
    return (DAPDebuggerState != DAPState::Stopped);
}

int Debugger_DAP::GetExitCode() const
{
    return m_exit_code;
}

int Debugger_DAP::GetStackFrameCount() const
{
    return m_backtrace.size();
}

cb::shared_ptr<const cbStackFrame> Debugger_DAP::GetStackFrame(int index) const
{
    return m_backtrace[index];
}

//struct GDBSwitchToFrameNotification
//{
//    GDBSwitchToFrameNotification(Debugger_DAP * plugin) :
//        m_plugin(plugin)
//    {
//    }
//
//    void operator()(dbg_DAP::ResultParser const & result, int frame_number, bool user_action)
//    {
//        if (m_frame_number < m_plugin->GetStackFrameCount())
//        {
//            dbg_DAP::GDBCurrentFrame & current_frame = m_plugin->GetGDBCurrentFrame();
//
//            if (user_action)
//            {
//                current_frame.GDBSwitchToFrame(frame_number);
//            }
//            else
//            {
//                current_frame.Reset();
//                current_frame.SetFrame(frame_number);
//            }
//
//            cb::shared_ptr<const cbStackFrame> frame = m_plugin->GetStackFrame(frame_number);
//            wxString const & filename = frame->GetFilename();
//            long line;
//
//            if (frame->GetLine().ToLong(&line))
//            {
//                current_frame.SetPosition(filename, line);
//                m_plugin->SyncEditor(filename, line, true);
//            }
//
//            m_plugin->UpdateOnFrameChanged(true);
//            Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
//        }
//    }
//
//    Debugger_DAP * m_plugin;
//    int m_frame_number;
//};

void Debugger_DAP::SwitchToFrame(int number)
{
    if (IsRunning() && IsStopped())
    {
        if (number < static_cast<int>(m_backtrace.size()))
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("SwitchToFrame"), dbg_DAP::LogPaneLogger::LineType::Debug);
            //int frame = m_backtrace[number]->GetNumber();
            //typedef dbg_DAP::GDBSwitchToFrame<GDBSwitchToFrameNotification> SwitchType;
            //m_actions.Add(new SwitchType(frame, GDBSwitchToFrameNotification(this), true));
        }
    }
}

int Debugger_DAP::GetActiveStackFrame() const
{
    return m_current_frame.GetStackFrame();
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddBreakpoint(const wxString & filename, int line)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("AddBreakpoint %s:%d"), filename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
    return UpdateOrAddBreakpoint(filename, line, -1);
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::UpdateOrAddBreakpoint(const wxString & filename, const int line, const int id)
{
    wxFileName absfnfilename(filename);
    wxString absfilename;

    if (absfnfilename.IsAbsolute())
    {
        absfilename = filename;
    }
    else
    {
        absfnfilename.MakeAbsolute(m_dap_debuggeepath);
        absfilename = absfnfilename.GetFullPath();
    }

    cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);

    if (id != -1)
    {
        for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
        {
            if (
                ((*it)->GetProject() == project)
                &&
                ((*it)->GetFilename() == absfilename)
                &&
                ((*it)->GetLine() == line)
            )
            {
                (*it)->SetID(id);
                m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("UpdateOrAddBreakpoint update %s:%d set ID %d"), absfilename, line, id), dbg_DAP::LogPaneLogger::LineType::Debug);
                return *it;
            }
        }
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("UpdateOrAddBreakpoint add %s:%d"), absfilename, line), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::GDBBreakpoint> ptr(new dbg_DAP::GDBBreakpoint(project, m_pLogger, absfilename, line, id));
    m_breakpoints.push_back(ptr);
    std::map<wxString, dbg_DAP::GDBBreakpointsContainer>::iterator mapit;
    mapit = m_map_filebreakpoints.find(absfilename);

    if (mapit != m_map_filebreakpoints.end())
    {
        mapit->second.push_back(ptr);
    }
    else
    {
        std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>> filebreakpoint;
        filebreakpoint.push_back(ptr);
        m_map_filebreakpoints.insert(std::pair<wxString, std::vector<cb::shared_ptr<dbg_DAP::GDBBreakpoint>>>(absfilename, filebreakpoint));
    }

    if (IsRunning())
    {
        mapit = m_map_filebreakpoints.find(absfilename);
        std::vector<dap::SourceBreakpoint> vlines;
        dbg_DAP::GDBBreakpointsContainer filebreakpoints = mapit->second;

        for (dbg_DAP::GDBBreakpointsContainer::iterator it = filebreakpoints.begin(); it != filebreakpoints.end(); ++it)
        {
            vlines.push_back({ static_cast<int>((*it)->GetLine()), wxEmptyString });
        }

        wxFileName relfnfilename(absfnfilename);
        relfnfilename.MakeRelativeTo(m_dap_debuggeepath);
        wxString relfilename = relfnfilename.GetFullPath();
        m_dapClient.SetBreakpointsFile(relfilename, vlines);
    }

    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints.back());
}


//cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddBreakpoint(cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp)
//{
//    m_breakpoints.push_back(bp);
//
//    if (IsRunning())
//    {
//        if (!IsStopped())
//        {
//            m_executor.Interupt();
//            m_actions.Add(new dbg_DAP::GDBBreakpointAddAction(bp, m_pLogger));
//            Continue();
//        }
//        else
//        {
//            m_actions.Add(new dbg_DAP::GDBBreakpointAddAction(bp, m_pLogger));
//        }
//    }
//
//    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints.back());
//}
//
//cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddDataBreakpoint(const wxString& dataExpression)
//{
//    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("dataExpression : %s", dataExpression), dbg_DAP::LogPaneLogger::LineType::Warning);
//
//    dbg_DAP::DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), dataExpression, true, 1);
//    PlaceWindow(&dlg);
//    if (dlg.ShowModal() == wxID_OK)
//    {
//        bool enabled = dlg.IsBreakpointEnabled();
//        const wxString& newDataExpression = dlg.GetDataExpression();
//        int sel = dlg.GetSelection();
//
//        cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp(new dbg_DAP::GDBBreakpoint(m_pProject, m_pLogger));
//        bp->SetType(dbg_DAP::GDBBreakpoint::BreakpointType::bptData);
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
//        return cb::shared_ptr<cbBreakpoint>();
//    }
//}
//
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
    for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        if ((*it)->GetID() == id)
        {
            return *it;
        }
    }

    return cb::shared_ptr<cbBreakpoint>();
}

//void Debugger_DAP::UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
//{
//    dbg_DAP::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);
//
//    if (it == m_breakpoints.end())
//    {
//        return;
//    }
//
//    cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp = cb::static_pointer_cast<dbg_DAP::GDBBreakpoint>(breakpoint);
//    bool reset = false;
//    switch (bp->GetType())
//    {
//        case dbg_DAP::GDBBreakpoint::bptCode:
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
//        case dbg_DAP::GDBBreakpoint::bptData:
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
//        case dbg_DAP::GDBBreakpoint::bptFunction:
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
//}
//
//void Debugger_DAP::DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
//{
//    dbg_DAP::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);
//
//    if (it != m_breakpoints.end())
//    {
//
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
//                                 __LINE__,
//                                 wxString::Format(_("%s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
//                                 dbg_DAP::LogPaneLogger::LineType::Debug);
//        cb::shared_ptr<dbg_DAP::GDBBreakpoint> pBrkPt = *it;
//
//        int index = pBrkPt->GetIndex();
//
//        if (index != -1)
//        {
//            dbg_DAP::GDBBreakpoint::BreakpointType bpType = pBrkPt->GetType();
//            switch (bpType)
//            {
//                case dbg_DAP::GDBBreakpoint::bptCode:
//                case dbg_DAP::GDBBreakpoint::bptData:
//                {
//                    if (!IsStopped())
//                    {
//                        m_executor.Interupt();
//                        AddStringCommand(wxString::Format("-break-delete %d", index));
//                        Continue();
//                    }
//                    else
//                    {
//                        AddStringCommand(wxString::Format("-break-delete %d", index));
//                    }
//                    break;
//                }
//
//                case dbg_DAP::GDBBreakpoint::bptFunction:
//// #warning dbg_DAP::GDBBreakpoint::BreakpointType::bptFunction not supported yet!!
//                    #ifdef __MINGW32__
//                        if (IsDebuggerPresent())
//                        {
//                            DebugBreak();
//                        }
//                    #endif // __MINGW32__
//                    break;
//
//                default:
//                    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Unknown breakpoint type: %d",  bpType), dbg_DAP::LogPaneLogger::LineType::Error);
//                    break;
//            }
//        }
//
//        m_breakpoints.erase(it);
//    }
//}
//
//void Debugger_DAP::DeleteAllBreakpoints()
//{
//    if (IsRunning())
//    {
//        wxString breaklist;
//
//        for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
//        {
//            dbg_DAP::GDBBreakpoint & current = **it;
//
//            if (current.GetIndex() != -1)
//            {
//                breaklist += wxString::Format(" %d", current.GetIndex());
//            }
//        }
//
//        if (!breaklist.empty())
//        {
//            if (!IsStopped())
//            {
//                m_executor.Interupt();
//                AddStringCommand("-break-delete" + breaklist);
//                Continue();
//            }
//            else
//            {
//                AddStringCommand("-break-delete" + breaklist);
//            }
//        }
//    }
//
//    m_breakpoints.clear();
//}
//
//void Debugger_DAP::ShiftBreakpoint(int index, int lines_to_shift)
//{
//    if (index < 0 || index >= static_cast<int>(m_breakpoints.size()))
//    {
//        return;
//    }
//
//    cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp = m_breakpoints[index];
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
//}
//
//void Debugger_DAP::EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable)
//{
//    dbg_DAP::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);
//
//    if (it != m_breakpoints.end())
//    {
//
//        int index = (*it)->GetIndex();
//
//        if ((*it)->IsEnabled() == enable)
//        {
//            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
//                                     __LINE__,
//                                     wxString::Format(_("breakpoint found but no change needed: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
//                                     dbg_DAP::LogPaneLogger::LineType::Debug);
//            // N change required!
//            return;
//        }
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
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
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
//                                 __LINE__,
//                                 wxString::Format(_("Breakpoint NOT FOUND: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
//                                 dbg_DAP::LogPaneLogger::LineType::Warning);
//
//    }
//}
//
//int Debugger_DAP::GetThreadsCount() const
//{
//    return m_threads.size();
//}
//
//cb::shared_ptr<const cbThread> Debugger_DAP::GetThread(int index) const
//{
//    return m_threads[index];
//}
//
//bool Debugger_DAP::SwitchToThread(int thread_number)
//{
//    if (IsStopped())
//    {
//        dbg_DAP::GDBSwitchToThread<Notifications> * a;
//        a = new dbg_DAP::GDBSwitchToThread<Notifications>(thread_number,
//                                                      m_pLogger,
//                                                      Notifications(this, m_executor, true)
//                                                     );
//        m_actions.Add(a);
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
void Debugger_DAP::UpdateWatches(int updateType)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("updating watches"), dbg_DAP::LogPaneLogger::LineType::Debug);
    //Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->OnDebuggerUpdated();
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(updateType);
    //event.SetPlugin(m_pDriver->GetDebugger());
    Manager::Get()->ProcessEvent(event);
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(const wxString & symbol, cb_unused bool update)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", symbol), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::GDBWatch> watch(new dbg_DAP::GDBWatch(m_pProject, m_pLogger, symbol, false));

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
    UpdateWatches(int(cbDebuggerPlugin::DebugWindows::Watches));
    return watch;
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(dbg_DAP::GDBWatch * watch, cb_unused bool update)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", watch->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::GDBWatch> w(watch);
    m_watches.push_back(w);

    if (IsRunning())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Need to wire up watch.", dbg_DAP::LogPaneLogger::LineType::Error);
        //        m_actions.Add(new dbg_DAP::GDBWatchCreateAction(w, m_watches, m_pLogger, true));
    }

    return w;
}

//cb::shared_ptr<cbWatch> Debugger_DAP::AddMemoryRange(uint64_t llAddress, uint64_t llSize, const wxString &symbol, bool update)
//{
//    cb::shared_ptr<dbg_DAP::GDBMemoryRangeWatch> watch(new dbg_DAP::GDBMemoryRangeWatch(m_pProject, m_pLogger, llAddress, llSize, symbol));
//
//    watch->SetSymbol(symbol);
//    watch->SetAddress(llAddress);
//
//    m_memoryRanges.push_back(watch);
//    m_mapWatchesToType[watch] = dbg_DAP::GDBWatchType::MemoryRange;
//
//    if (IsRunning())
//    {
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Adding watch for: address: %#018llx  size:%lld", llAddress, llSize), dbg_DAP::LogPaneLogger::LineType::Warning);
//        m_actions.Add(new dbg_DAP::GDBMemoryRangeWatchCreateAction(watch, m_pLogger));
//    }
//
//    return watch;
//}
//
//void Debugger_DAP::AddTooltipWatch(const wxString & symbol, wxRect const & rect)
//{
//    cb::shared_ptr<dbg_DAP::GDBWatch> w(new dbg_DAP::GDBWatch(m_pProject, m_pLogger, symbol, true));
//    m_watches.push_back(w);
//
//    if (IsRunning())
//    {
//        m_actions.Add(new dbg_DAP::GDBWatchCreateTooltipAction(w, m_watches, m_pLogger, rect));
//    }
//}
//
void Debugger_DAP::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

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

    m_watches.erase(it);
}

bool Debugger_DAP::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    if (watch == m_WatchLocalsandArgs)
    {
        return true;
    }

    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);
    return it != m_watches.end();
}

bool Debugger_DAP::IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch)
{
    dbg_DAP::GDBMapWatchesToType::const_iterator it = m_mapWatchesToType.find(watch);

    if (it == m_mapWatchesToType.end())
    {
        return false;
    }
    else
    {
        return (it->second == dbg_DAP::GDBWatchType::MemoryRange);
    }
}

void Debugger_DAP::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
{
    // not supported for child nodes or memory ranges!
    if (watch->GetParent() || IsMemoryRangeWatch(watch))
    {
        return;
    }

    cb::shared_ptr<dbg_DAP::GDBWatch> real_watch = cb::static_pointer_cast<dbg_DAP::GDBWatch>(watch);
    dbg_DAP::EditWatchDlg dlg(real_watch, nullptr);
    PlaceWindow(&dlg);

    if (dlg.ShowModal() == wxID_OK)
    {
        DoWatches();
    }
}

bool Debugger_DAP::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value)
{
    if (!IsStopped() || !IsRunning())
    {
        return false;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return false;
    }

    cb::shared_ptr<dbg_DAP::GDBWatch> real_watch = cb::static_pointer_cast<dbg_DAP::GDBWatch>(watch);
    //    AddStringCommand("-var-assign " + real_watch->GetID() + " " + value);
    //    m_actions.Add(new dbg_DAP::GDBWatchSetValueAction(*it, static_cast<dbg_DAP::GDBWatch*>(watch), value, m_pLogger));
    //    dbg_DAP::Action * update_action = new dbg_DAP::GDBWatchesUpdateAction(m_watches, m_pLogger);
    //    update_action->SetWaitPrevious(true);
    //    m_actions.Add(update_action);
    return true;
}

void Debugger_DAP::ExpandWatch(cb::shared_ptr<cbWatch> watch)
{
    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_DAP::GDBWatch> real_watch = cb::static_pointer_cast<dbg_DAP::GDBWatch>(watch);

        if (!real_watch->HasBeenExpanded())
        {
            //            m_actions.Add(new dbg_DAP::GDBWatchExpandedAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_DAP::CollapseWatch(cb::shared_ptr<cbWatch> watch)
{
    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_DAP::GDBWatch> real_watch = cb::static_pointer_cast<dbg_DAP::GDBWatch>(watch);

        if (real_watch->HasBeenExpanded() && real_watch->DeleteOnCollapse())
        {
            //            m_actions.Add(new dbg_DAP::GDBWatchCollapseAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_DAP::UpdateWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
    dbg_DAP::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (IsRunning())
    {
        //        m_actions.Add(new dbg_DAP::GDBWatchCreateAction(*it, m_watches, m_pLogger, false));
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
            m_WatchLocalsandArgs = cb::shared_ptr<dbg_DAP::GDBWatch>(new dbg_DAP::GDBWatch(m_pProject, m_pLogger, "Function locals and arguments", false));
            m_WatchLocalsandArgs->Expand(true);
            m_WatchLocalsandArgs->MarkAsChanged(false);
            cbWatchesDlg * watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            watchesDialog->AddSpecialWatch(m_WatchLocalsandArgs, true);
        }
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Need to wire up DoWatches.", dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_actions.Add(new dbg_DAP::GDBStackVariables(m_pLogger, m_WatchLocalsandArgs, bWatchFuncLocalsArgs));
    // Update watches now
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
    Manager::Get()->ProcessEvent(event);
}


//void Debugger_DAP::SendCommand(const wxString & cmd, bool debugLog)
//{
//    if (!IsRunning())
//    {
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Command will not be executed because the debugger is not running!"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
//        return;
//    }
//
//    if (!IsStopped())
//    {
//        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Command will not be executed because the debugger/debuggee is not paused/interupted!"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
//        return;
//    }
//
//    if (cmd.empty())
//    {
//        return;
//    }
//
//    DoSendCommand(cmd);
//}


//void Debugger_DAP::AttachToProcess(const wxString & pid)
//{
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
//}
//
//void Debugger_DAP::DetachFromProcess()
//{
//    AddStringCommand(wxString::Format("-target-detach %ld", m_executor.GetAttachedPID()));
//}
//
//bool Debugger_DAP::IsAttachedToProcess() const
//{
//    return m_pid_attached != 0;
//}
//
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
    //                struct Switcher : dbg_DAP::GDBSwitchToFrameInvoker
    //                {
    //                    Switcher(Debugger_DAP * plugin, dbg_DAP::ActionsMap & actions) :
    //                        m_plugin(plugin),
    //                        m_actions(actions)
    //                    {
    //                    }
    //
    //                    virtual void Invoke(int frame_number)
    //                    {
    //                        typedef dbg_DAP::GDBSwitchToFrame<GDBSwitchToFrameNotification> SwitchType;
    //                        m_actions.Add(new SwitchType(frame_number, GDBSwitchToFrameNotification(m_plugin), false));
    //                    }
    //
    //                    Debugger_DAP * m_plugin;
    //                    dbg_DAP::ActionsMap & m_actions;
    //                };
    //                Switcher * switcher = new Switcher(this, m_actions);
    //                m_actions.Add(new dbg_DAP::GDBGenerateBacktrace(switcher, m_backtrace, m_current_frame, m_pLogger));
    //            }
    //            break;
    //
    //        case Threads:
    //            m_actions.Add(new dbg_DAP::GDBGenerateThreadsList(m_threads, m_current_frame.GetThreadId(), m_pLogger));
    //            break;
    //
    //        case CPURegisters:
    //            {
    //                m_actions.Add(new dbg_DAP::GDBGenerateCPUInfoRegisters(m_pLogger));
    //            }
    //            break;
    //
    //        case Disassembly:
    //            {
    //                wxString flavour = GetActiveConfigEx().GetDisassemblyFlavorCommand();
    //                m_actions.Add(new dbg_DAP::GDBDisassemble(flavour, m_pLogger));
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
    //                    m_actions.Add(new dbg_DAP::GDBGenerateExamineMemory(m_pLogger));
    //                }
    //            }
    //            break;
    //
    //        case MemoryRange:
    //            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("DebugWindows MemoryRange called!!"), dbg_DAP::LogPaneLogger::LineType::Error);
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

void Debugger_DAP::KillDAPDebugger()
{
    if (m_dapPid >= 0)
    {
        wxKill(m_dapPid);
        m_dapPid = -1;
    }
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

void Debugger_DAP::StripQuotes(wxString & str)
{
    if ((str.GetChar(0) == '\"') && (str.GetChar(str.Length() - 1) == '\"'))
    {
        str = str.Mid(1, str.Length() - 2);
    }
}

void Debugger_DAP::ConvertToGDBFriendly(wxString & str)
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

void Debugger_DAP::ConvertToGDBDirectory(wxString & str, wxString base, bool relative)
{
    if (str.IsEmpty())
    {
        return;
    }

    ConvertToGDBFriendly(str);
    ConvertToGDBFriendly(base);
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

    ConvertToGDBFriendly(str);
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
                        ConvertToGDBDirectory(dir, "", false);
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

    for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        dbg_DAP::GDBBreakpoint & bp = **it;

        if (bp.GetProject() == pProject)
        {
            bp.SaveBreakpointToXML(pBreakpointMasterNode);
        }
    }

    // ********************  Save Watches ********************
    tinyxml2::XMLElement * pElementWatchesList = doc.NewElement("WatchesList");
    pElementWatchesList->SetAttribute("count", static_cast<int64_t>(m_watches.size()));
    tinyxml2::XMLNode * pWatchesMasterNode = rootnode->InsertEndChild(pElementWatchesList);

    for (dbg_DAP::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        dbg_DAP::GDBWatch & watch = **it;

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
    //    for (dbg_DAP::GDBMemoryRangeWatchesContainer::iterator it = m_memoryRanges.begin(); it != m_memoryRanges.end(); ++it)
    //    {
    //        dbg_DAP::GDBMemoryRangeWatch& memoryRange = **it;
    //
    //        if (memoryRange.GetProject() == pProject)
    //        {
    //            memoryRange.SaveWatchToXML(pMemoryRangeMasterNode);
    //        }
    //    }
    // ********************  Save XML to disk ********************
    return doc.SaveFile(fname.GetFullPath(), false);
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
    tinyxml2::XMLError eResult = doc.LoadFile(fname.GetFullPath());

    if (eResult != tinyxml2::XMLError::XML_SUCCESS)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Could not open the file '\%s\" due to the error: %s"), fname.GetFullPath(), doc.ErrorIDToName(eResult)), dbg_DAP::LogPaneLogger::LineType::Error);
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
            dbg_DAP::GDBBreakpoint * bpNew = new dbg_DAP::GDBBreakpoint(pProject, m_pLogger);
            bpNew->LoadBreakpointFromXML(pBreakpointElement, this);

            // Find new breakpoint in the m_breakpoints
            for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
            {
                dbg_DAP::GDBBreakpoint & bpSearch = **it;

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
            wxString GDBWatchClassName = dbg_DAP::ReadChildNodewxString(pWatchElement, "GDBWatchClassName");

            if (GDBWatchClassName.IsSameAs("GDBWatch"))
            {
                dbg_DAP::GDBWatch * watch = new dbg_DAP::GDBWatch(pProject, m_pLogger, "", false);
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
    //            wxString GDBMemoryRangeWatchName = dbg_DAP::ReadChildNodewxString(pWatchElement, "GDBMemoryRangeWatch");
    //            if (GDBMemoryRangeWatchName.IsSameAs("GDBMemoryRangeWatch"))
    //            {
    //                dbg_DAP::GDBMemoryRangeWatch* memoryRangeWatch = new dbg_DAP::GDBMemoryRangeWatch(pProject, m_pLogger, 0, 0, wxEmptyString );
    //                memoryRangeWatch->LoadWatchFromXML(pWatchElement, this);
    //            }
    //        }
    //    }
    // ******************** Finished Load ********************
    return true;
}

void Debugger_DAP::OnProcessBreakpointData(const wxString & brkDescription)
{
    wxString::size_type brkLookupIndexStart = brkDescription.find(' ');
    wxString brkLookup = brkDescription.substr(brkLookupIndexStart);

    if (brkLookup.IsEmpty())
    {
        return;
    }

    wxString::size_type brkLookupIndexEnd = brkLookup.find('.');
    wxString brkID = brkLookup.substr(0, brkLookupIndexEnd);

    if (brkID.IsEmpty())
    {
        return;
    }

    long id;

    if (brkID.ToLong(&id, 10))
    {
        cb::shared_ptr<cbBreakpoint> breakpoint = Debugger_DAP::GetBreakpointByID(id);

        if (breakpoint)
        {
            cb::shared_ptr<dbg_DAP::GDBBreakpoint> bp = cb::static_pointer_cast<dbg_DAP::GDBBreakpoint>(breakpoint);

            if (bp && !bp->GetFilename().IsEmpty())
            {
                //GetGDBCurrentFrame().SetPosition(bp.GetFilename(), bp.GetLine());
                SyncEditor(bp->GetFilename(), bp->GetLine() + 1, true);
            }
        }
    }
}

/// ----------------------------------
/// -- DAP EVENTS START --
/// ----------------------------------

void Debugger_DAP::OnLaunchResponse(DAPEvent & event)
{
    // Check that the debugee was started successfully
    dap::LaunchResponse * resp = event.GetDapResponse()->As<dap::LaunchResponse>();

    if (resp && !resp->success)
    {
        // launch failed!
        wxMessageBox("Failed to launch debuggee: " + resp->message, "DAP", wxICON_ERROR | wxOK | wxOK_DEFAULT | wxCENTRE);
        m_dapClient.CallAfter(&dap::Client::Reset);
    }
}

/// DAP server responded to our `initialize` request
void Debugger_DAP::OnInitializeResponse(DAPEvent & event)
{
    wxUnusedVar(event);
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnInitialize Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    m_dapClient.Launch({m_dap_debuggee});
}

/// DAP server responded to our `initialize` request
void Debugger_DAP::OnInitializedEvent(DAPEvent & event)
{
    // got initialized event, place breakpoints and continue
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got Initialized event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Setup initial breakpoints
    CreateStartBreakpoints(true);
    // Setup initial data watches
    CreateStartWatches();
    // Set breakpoint on "main"
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Placing breakpoint at main..."), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    m_dapClient.SetFunctionBreakpoints({ { "main" } });
    m_dapClient.ConfigurationDone();
}

/// DAP server stopped. This can happen for multiple reasons:
/// - exception
/// - breakpoint hit
/// - step (user previously issued `Next` command)
void Debugger_DAP::OnStopped(DAPEvent & event)
{
    DAPDebuggerState = DAPState::Stopped;
    // got stopped event
    dap::StoppedEvent * stopped_data = event.GetDapEvent()->As<dap::StoppedEvent>();

    if (stopped_data)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("text: %s"),  stopped_data->text), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("description: %s"),  stopped_data->description), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("All threads stopped: %s"),  stopped_data->allThreadsStopped ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped thread ID: %d (active thread ID: %d)"), stopped_data->threadId, m_dapClient.GetActiveThreadId()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        if (stopped_data->reason.IsSameAs("breakpoint"))
        {
            /* reason:
            * Values: 'step', 'breakpoint', 'exception', 'pause', 'entry', 'goto',
            * 'function breakpoint', 'data breakpoint', etc.
            */
            OnProcessBreakpointData(stopped_data->description);
        }

        m_dapClient.GetFrames();
    }

    MarkAsStopped();
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnScopes(DAPEvent & event)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnScopes Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::ScopesResponse * resp = event.GetDapResponse()->As<dap::ScopesResponse>();

    if (resp)
    {
        m_stackdapvariables.clear();

        for (const auto & scope : resp->scopes)
        {
            m_dapClient.GetChildrenVariables(scope.variablesReference);
        }
    }
}

void Debugger_DAP::OnVariables(DAPEvent & event)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("OnVariables event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
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

            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
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

        for (dbg_DAP::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
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

        UpdateWatches(int(cbDebuggerPlugin::DebugWindows::Watches));
    }
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnStackTrace(DAPEvent & event)
{
    dap::StackTraceResponse * stack_trace_data = event.GetDapResponse()->As<dap::StackTraceResponse>();

    if (stack_trace_data)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received stack trace event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        if (!stack_trace_data->stackFrames.empty())
        {
            wxString fileName = stack_trace_data->stackFrames[0].source.path;

            if (!fileName.IsEmpty())
            {
                int lineNumber = stack_trace_data->stackFrames[0].line;
                m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SyncEditor: %s %d"), fileName, lineNumber), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                SyncEditor(fileName, lineNumber, true);
            }

            // request the scopes for the first stack
            m_dapClient.GetScopes(stack_trace_data->stackFrames[0].id);
        }

        m_backtrace.clear();
        int stackID = 0;

        for (const auto & stack : stack_trace_data->stackFrames)
        {
#if 1
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("Stack: ID 0x%X , Name: %s , File: %s  %d"),
                                                      stack.id,
                                                      stack.name,
                                                      stack.source.path.IsEmpty() ? stack.source.name : stack.source.path,
                                                      stack.line
                                                     ),
                                     dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
            cbStackFrame s;
            s.SetNumber(stackID++);
            s.SetFile(stack.source.path, wxString::Format("%d", stack.line));
            s.SetSymbol(stack.name);
            s.SetAddress(stack.id);
            s.MakeValid(true);
            m_backtrace.push_back(cb::shared_ptr<cbStackFrame>(new cbStackFrame(s)));
        }

        Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
    }
}

/// Debuggee process exited, print the exit code
void Debugger_DAP::OnExited(DAPEvent & event)
{
    DAPDebuggerState = DAPState::NotConnected;
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Debuggee exited. Exit code: %d"), event.GetDapEvent()->As<dap::ExitedEvent>()->exitCode));
}

/// Debug session terminated
void Debugger_DAP::OnTerminated(DAPEvent & event)
{
    DAPDebuggerState = DAPState::NotConnected;
    wxUnusedVar(event);
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Session terminated!"));
    m_dapClient.Reset();
    ClearActiveMarkFromAllEditors();
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("debugger terminated!")), dbg_DAP::LogPaneLogger::LineType::Warning);
    m_timer_poll_debugger.Stop();
    // Notify debugger plugins for end of debug session
    PluginManager * plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_FINISHED);
    plm->NotifyPlugins(evt);
    SwitchToPreviousLayout();
    KillDAPDebugger();
    MarkAsStopped();
    //    for (dbg_DAP::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    //    {
    //        (*it)->SetIndex(-1);
    //    }
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


void Debugger_DAP::OnOutput(DAPEvent & event)
{
    dap::OutputEvent * output_data = event.GetDapEvent()->As<dap::OutputEvent>();

    if (output_data)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _(output_data->category << ":" << output_data->output), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
}

void Debugger_DAP::OnBreakpointLocations(DAPEvent & event)
{
    dap::BreakpointLocationsResponse * d = event.GetDapResponse()->As<dap::BreakpointLocationsResponse>();

    if (d)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("==> Breakpoints:\n"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        for (const auto & bp : d->breakpoints)
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _(d->filepath << ":" << bp.line), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
    }
}

void Debugger_DAP::OnConnectionError(DAPEvent & event)
{
    DAPDebuggerState = DAPState::NotConnected;
    wxUnusedVar(event);
    wxMessageBox(_("Lost connection to dap server"));
}

void Debugger_DAP::OnBreakpointSet(DAPEvent & event)
{
    dap::SetBreakpointsResponse * resp = event.GetDapResponse()->As<dap::SetBreakpointsResponse>();

    if (resp)
    {
        for (const auto & bp : resp->breakpoints)
        {
            wxString message;
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("OnBreakpointSet: ID %d , Verified: %s , File: %s , Line: %d"), bp.id, bp.verified ? "True" : "False", bp.source.path, bp.line)
                                     , dbg_DAP::LogPaneLogger::LineType::UserDisplay
                                    );

            if (bp.line != -1)
            {
                UpdateOrAddBreakpoint(bp.source.path, bp.line, bp.id);
            }
        }
    }
}

void Debugger_DAP::OnDapLog(DAPEvent & event)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, event.GetString());
}

void Debugger_DAP::OnRunInTerminalRequest(DAPEvent & event)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Handling `OnRunInTerminalRequest` event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
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

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Starting process: %s"), command), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    m_process = dap::ExecuteProcess(command);
    dap::RunInTerminalResponse response = m_dapClient.MakeRequest<dap::RunInTerminalResponse>();
    response.request_seq = request->seq;

    if (!m_process)
    {
        response.success = false;
        response.processId = 0;
    }
    else
    {
        response.success = true;
        response.processId = m_process->GetProcessId();
    }

    m_dapClient.SendResponse(response);
}

/// ----------------------------------
/// -- DAP EVENTS END --
/// ----------------------------------



// Windows: C:\msys64\mingw64\bin\lldb-vscode.exe -port 12345
// Linux: /usr/bin/lldb-vscode-14 -port 12345
// MACOS:: /usr/local/Cellar/llvm/14.0.6/bin/lldb-vscode -port 12345 -port 12345
