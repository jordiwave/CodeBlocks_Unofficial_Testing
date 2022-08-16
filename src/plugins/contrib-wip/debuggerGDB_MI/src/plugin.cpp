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

// CB include files (not GDB)
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

// GDB include files
#include "actions.h"
#include "cmd_result_parser.h"
#include "escape.h"
#include "frame.h"
#include "debuggeroptionsdlg.h"
#include "gdb_logger.h"
#include "databreakpointdlg.h"
#include "editbreakpointdlg.h"
#include "editwatchdlg.h"
#include "debuggeroptionsprjdlg.h"
#include "plugin.h"

//XML file root tag for data
static const char* XML_CFG_ROOT_TAG = "Debugger_layout_file";

namespace
{
    int const id_gdb_process = wxNewId();
    int const id_gdb_poll_timer = wxNewId();
    int const id_menu_info_command_stream = wxNewId();

    // Register the plugin with Code::Blocks.
    // We are using an anonymous namespace so we don't litter the global one.
    // this auto-registers the plugin
    PluginRegistrant<Debugger_GDB_MI> reg("debugger_gdbmi");
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
BEGIN_EVENT_TABLE(Debugger_GDB_MI, cbDebuggerPlugin)

    EVT_PIPEDPROCESS_STDOUT(id_gdb_process, Debugger_GDB_MI::OnGDBOutput)
    EVT_PIPEDPROCESS_STDERR(id_gdb_process, Debugger_GDB_MI::OnGDBError)
    EVT_PIPEDPROCESS_TERMINATED(id_gdb_process, Debugger_GDB_MI::OnGDBTerminated)

    EVT_IDLE(Debugger_GDB_MI::OnIdle)
    EVT_TIMER(id_gdb_poll_timer, Debugger_GDB_MI::OnTimer)

    EVT_MENU(id_menu_info_command_stream, Debugger_GDB_MI::OnMenuInfoCommandStream)
END_EVENT_TABLE()

// constructor
Debugger_GDB_MI::Debugger_GDB_MI() :
    cbDebuggerPlugin("GDB/MI", "gdbmi_debugger"),
    m_pProject(nullptr),
    m_command_stream_dialog(nullptr),
    m_console_pid(-1),
    m_pid_attached(0)
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource("debugger_gdbmi.zip"))
    {
        NotifyMissingFile("debugger_gdbmi.zip");
    }

    m_pLogger = new dbg_mi::LogPaneLogger(this);
    m_executor.SetLogger(m_pLogger);
}

// destructor
Debugger_GDB_MI::~Debugger_GDB_MI()
{
}

void Debugger_GDB_MI::OnAttachReal()
{
    m_timer_poll_debugger.SetOwner(this, id_gdb_poll_timer);
    DebuggerManager & dbg_manager = *Manager::Get()->GetDebuggerManager();
    dbg_manager.RegisterDebugger(this);

    // Do no use cbEVT_PROJECT_OPEN as the project may not be active!!!!
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,  new cbEventFunctor<Debugger_GDB_MI, CodeBlocksEvent>(this, &Debugger_GDB_MI::OnProjectOpened));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE,     new cbEventFunctor<Debugger_GDB_MI, CodeBlocksEvent>(this, &Debugger_GDB_MI::OnProjectClosed));
}

void Debugger_GDB_MI::OnReleaseReal(bool appShutDown)
{
    Manager::Get()->GetDebuggerManager()->UnregisterDebugger(this);
    KillConsole();
    m_executor.ForceStop();

    if (m_command_stream_dialog)
    {
        m_command_stream_dialog->Destroy();
        m_command_stream_dialog = nullptr;
    }
}

void Debugger_GDB_MI::SetupToolsMenu(wxMenu & menu)
{
    menu.Append(id_menu_info_command_stream, _("Show command stream"));
}

bool Debugger_GDB_MI::SupportsFeature(cbDebuggerFeature::Flags flag)
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

cbDebuggerConfiguration * Debugger_GDB_MI::LoadConfig(const ConfigManagerWrapper & config)
{
    return new dbg_mi::DebuggerConfiguration(config);
}

dbg_mi::DebuggerConfiguration & Debugger_GDB_MI::GetActiveConfigEx()
{
    return static_cast<dbg_mi::DebuggerConfiguration &>(GetActiveConfig());
}

cbConfigurationPanel* Debugger_GDB_MI::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    dbg_mi::DebuggerOptionsProjectDlg* dlg = new dbg_mi::DebuggerOptionsProjectDlg(parent, this, project);
    return dlg;
}


bool Debugger_GDB_MI::SelectCompiler(cbProject & project, Compiler *& compiler,
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
                m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Selecting target canceled"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
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
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("The selected target is only running pre/post build step commands,Can't debug such a target... ")), dbg_mi::LogPaneLogger::LineType::Error);
            return false;
        }

        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Selecting target: %s"), target->GetTitle()), dbg_mi::LogPaneLogger::LineType::UserDisplay);
        // find the target's compiler (to see which debugger to use)
        compiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : project.GetCompilerID());
    }
    else
    {
        compiler = CompilerFactory::GetDefaultCompiler();
    }

    return true;
}

void Debugger_GDB_MI::OnGDBOutput(wxCommandEvent & event)
{
    wxString const & msg = event.GetString();

    if (!msg.IsEmpty() &&
            !msg.IsSameAs("(gdb) ") &&
            !msg.IsSameAs("\\n") &&
            !msg.IsSameAs("~\"\\n\"") &&
            (
                // Ignore lines like ~"Catchpoint 3 (catch)\n"
                !msg.StartsWith("~\"Catchpoint ") &&
                !msg.EndsWith(" (catch)\n\"")
            ) &&
            !msg.StartsWith("~\"Reading symbols from ") &&
            (
                // Ignore lines like ~"Thread 1 hit Breakpoint 1, main () at D:\\Andrew_Development\\Z_Testing_Apps\\Printf_I64\\main.cpp:13\n"
                !msg.StartsWith("~\"Thread ") &&
                !msg.Contains(" hit Breakpoint ")
            ) &&
            (
                // Ignore lines like ~"\032\032D:\\Andrew_Development\\Z_Testing_Apps\\Printf_I64\\main.cpp:13:220:beg:0x7ff6af41155a\n"
                !msg.StartsWith("~\"\\032\\032") &&
                !msg.Contains(":beg:")
            ) &&
            !msg.StartsWith("~\"Source directories searched: ")
       )
    {
        ParseOutput(msg);
    }
    else
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Ignore =>%s<=", msg), dbg_mi::LogPaneLogger::LineType::Receive_Info);
    }
}

void Debugger_GDB_MI::OnGDBError(wxCommandEvent & event)
{
    wxString const & msg = event.GetString();

    if (!msg.IsEmpty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Receive: =>%s<=", msg), dbg_mi::LogPaneLogger::LineType::Error);
        ParseOutput(msg);
    }
    else
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Receive Ignore =>%s<=", msg), dbg_mi::LogPaneLogger::LineType::Error);
    }
}

void Debugger_GDB_MI::OnGDBTerminated(wxCommandEvent & /*event*/)
{
    ClearActiveMarkFromAllEditors();
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("debugger terminated!")), dbg_mi::LogPaneLogger::LineType::Warning);
    m_timer_poll_debugger.Stop();
    m_actions.Clear();
    m_executor.Clear();

    // Notify debugger plugins for end of debug session
    PluginManager * plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_FINISHED);
    plm->NotifyPlugins(evt);
    SwitchToPreviousLayout();
    KillConsole();
    MarkAsStopped();

    for (dbg_mi::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        (*it)->SetIndex(-1);
    }

    cbCPURegistersDlg* pDialogCPURegisters = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();
    if (pDialogCPURegisters)
    {
        pDialogCPURegisters->Clear();
    }

    cbDisassemblyDlg* pDialogDisassembly = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();
    if (pDialogDisassembly)
    {
        cbStackFrame sf;
        pDialogDisassembly->Clear(sf);
    }
}

void Debugger_GDB_MI::OnIdle(wxIdleEvent & event)
{
    if (m_executor.IsStopped() && m_executor.IsRunning())
    {
        m_actions.Run(m_executor);
    }

    if (m_executor.ProcessHasInput())
    {
        event.RequestMore();
    }
    else
    {
        event.Skip();
    }
}

void Debugger_GDB_MI::OnTimer(wxTimerEvent & /*event*/)
{
    RunQueue();
    wxWakeUpIdle();
}

void Debugger_GDB_MI::OnMenuInfoCommandStream(wxCommandEvent & /*event*/)
{
    wxString full;

    for (int ii = 0; ii < m_executor.GetCommandQueueCount(); ++ii)
    {
        full += m_executor.GetQueueCommand(ii) + "\n";
    }

    if (m_command_stream_dialog)
    {
        m_command_stream_dialog->SetText(full);
        m_command_stream_dialog->Show();
    }
    else
    {
        m_command_stream_dialog = new dbg_mi::GDBTextInfoWindow(Manager::Get()->GetAppWindow(), _T("Command stream"), full);
        m_command_stream_dialog->Show();
    }
}

void Debugger_GDB_MI::AddStringCommand(wxString const & command)
{
    //-    dbg_mi::Command *cmd = new dbg_mi::Command();
    //-    cmd->SetString(command);
    //-    m_command_queue.AddCommand(cmd, true);
    //    m_executor.Execute(command);
    if (IsRunning())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("Queue command:: %s"), command),
                                 dbg_mi::LogPaneLogger::LineType::Debug);
        m_actions.Add(new dbg_mi::GDBSimpleAction(command));
    }
}

struct Notifications
{
        Notifications(Debugger_GDB_MI * plugin, dbg_mi::GDBExecutor & executor, bool simple_mode) :
            m_plugin(plugin),
            m_executor(executor),
            m_simple_mode(simple_mode)
        {
        }

        void operator()(dbg_mi::ResultParser const & parser)
        {
            dbg_mi::ResultValue const & result_value = parser.GetResultValue();

            if (m_simple_mode)
            {
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                ParseStateInfo(result_value);
                m_plugin->UpdateWhenStopped();
            }
            else
            {
                if (parser.GetResultType() == dbg_mi::ResultParser::NotifyAsyncOutput)
                {
                    ParseNotifyAsyncOutput(parser);
                }
                else
                {
                    if (parser.GetResultClass() == dbg_mi::ResultParser::ClassStopped)
                    {
                        dbg_mi::StoppedReason reason = dbg_mi::StoppedReason::Parse(result_value);
                        dbg_mi::StoppedReason::Type stopType = reason.GetType();

                        switch (stopType)
                        {
                            case dbg_mi::StoppedReason::SignalReceived:
                            {
                                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                                wxString signal_name, signal_meaning;
                                dbg_mi::Lookup(result_value, "signal-name", signal_name);

                                if ((signal_name != "SIGTRAP") && (signal_name != "SIGINT"))
                                {
                                    dbg_mi::Lookup(result_value, "signal-meaning", signal_meaning);
                                    InfoWindow::Display(_("Signal received"),
                                                        wxString::Format(_("\nProgram received signal: %s (%s)\n\n"),
                                                                         signal_meaning,
                                                                         signal_name));
                                }

                                Manager::Get()->GetDebuggerManager()->ShowBacktraceDialog();
                                UpdateCursor(result_value, true);
                            }
                            break;

                            case dbg_mi::StoppedReason::ExitedNormally:
                            case dbg_mi::StoppedReason::ExitedSignalled:
                                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                                m_executor.Execute("-gdb-exit");
                                break;

                            case dbg_mi::StoppedReason::Exited:
                            {
                                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                                int code = -1;

                                if (!dbg_mi::Lookup(result_value, "exit-code", code))
                                {
                                    code = -1;
                                }

                                m_plugin->SetExitCode(code);
                                m_executor.Execute("-gdb-exit");
                            }
                            break;

                            default:
                                if (stopType != dbg_mi::StoppedReason::BreakpointHit)
                                {
                                    m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                                }

                                UpdateCursor(result_value, !m_executor.IsTemporaryInterupt());
                        }

                        if (!m_executor.IsTemporaryInterupt())
                        {
                            m_plugin->BringCBToFront();
                        }
                    }
                    else
                    {
                        m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("notification event received: ==>%s<=="), parser.MakeDebugString()), dbg_mi::LogPaneLogger::LineType::Receive);
                    }
                }
            }
        }

    private:
        void UpdateCursor(dbg_mi::ResultValue const & result_value, bool parse_state_info)
        {
            if (parse_state_info)
            {
                ParseStateInfo(result_value);
            }

            m_executor.Stopped(true);
            // Notify debugger plugins for end of debug session
            PluginManager * plm = Manager::Get()->GetPluginManager();
            CodeBlocksEvent evt(cbEVT_DEBUGGER_PAUSED);
            plm->NotifyPlugins(evt);
            m_plugin->UpdateWhenStopped();
        }
        void ParseStateInfo(dbg_mi::ResultValue const & result_value)
        {
            dbg_mi::Frame frame;

            if (frame.ParseOutput(result_value))
            {
                dbg_mi::ResultValue const * thread_id_value;
                thread_id_value = result_value.GetTupleValue(m_simple_mode ? "new-thread-id" : "thread-id");

                if (thread_id_value)
                {
                    long id;

                    if (thread_id_value->GetSimpleValue().ToLong(&id, 10))
                    {
                        m_plugin->GetGDBCurrentFrame().SetThreadId(id);
                    }
                    else
                    {
                        m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                                __LINE__,
                                                                wxString::Format(_("Thread_id parsing failed (%s)"), result_value.MakeDebugString()),
                                                                dbg_mi::LogPaneLogger::LineType::Error);
                    }
                }

                if (frame.HasValidSource())
                {
                    dbg_mi::StoppedReason reason = dbg_mi::StoppedReason::Parse(result_value);

                    if (reason.GetType() == dbg_mi::StoppedReason::BreakpointHit)
                    {
                        m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                                __LINE__,
                                                                wxString::Format(_("Breakpoint hit on line#: %d in file: %s"), frame.GetLine(), frame.GetFilename()),
                                                                dbg_mi::LogPaneLogger::LineType::UserDisplay);
                    }
                    else
                    {
                        m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                                __LINE__,
                                                                wxString::Format(_("File line#: %d in %s ==>%s<=="), frame.GetLine(), frame.GetFilename(), result_value.MakeDebugString()),
                                                                dbg_mi::LogPaneLogger::LineType::Debug);
                    }

                    m_plugin->GetGDBCurrentFrame().SetPosition(frame.GetFilename(), frame.GetLine());
                    m_plugin->SyncEditor(frame.GetFilename(), frame.GetLine(), true);
                }
                else
                {
                    m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                            __LINE__,
                                                            wxString::Format(_("ParseStateInfo frame does not have valid source (%s)"), result_value.MakeDebugString()),
                                                            dbg_mi::LogPaneLogger::LineType::Error);
                }
            }
            else
            {
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Can't find/parse frame value: ==>%s<=="), result_value.MakeDebugString()),
                                                        dbg_mi::LogPaneLogger::LineType::Error);
            }
        }

        void ParseNotifyAsyncOutput(dbg_mi::ResultParser const & parser)
        {
            wxString notifyType = parser.GetAsyncNotifyType();

            if (notifyType.IsSameAs("thread-group-started"))
            {
                int pid;
                dbg_mi::Lookup(parser.GetResultValue(), "pid", pid);
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Found child pid: %d"), pid),
                                                        dbg_mi::LogPaneLogger::LineType::Receive_NoLine);
                dbg_mi::GDBExecutor & exec = m_plugin->GetGDBExecutor();

                if (!exec.HasChildPID())
                {
                    exec.SetChildPID(pid);
                }
            }
            else if (notifyType.IsSameAs("library-loaded"))
            {
                wxString  targetName;
                dbg_mi::Lookup(parser.GetResultValue(), "target-name", targetName);
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Notification: %s for %s"), parser.GetAsyncNotifyType(), targetName),
                                                        dbg_mi::LogPaneLogger::LineType::Receive_Info);
            }
            else if (notifyType.IsSameAs("breakpoint-modified"))
            {
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Notification for breakpoint-modified: %s"), parser.MakeDebugString()),
                                                        dbg_mi::LogPaneLogger::LineType::Receive_Info);
            }
            else if (notifyType.IsSameAs("breakpoint-deleted"))
            {
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Notification for breakpoint-deleted: %s"), parser.MakeDebugString()),
                                                        dbg_mi::LogPaneLogger::LineType::Receive_Info);
            }
            else
            {
                m_plugin->GetGDBLogger()->LogGDBMsgType(__PRETTY_FUNCTION__,
                                                        __LINE__,
                                                        wxString::Format(_("Notification: %s"), parser.MakeDebugString()),
                                                        dbg_mi::LogPaneLogger::LineType::Receive);
            }
        }

    private:
        Debugger_GDB_MI * m_plugin;
        int m_page_index;
        dbg_mi::GDBExecutor & m_executor;
        bool m_simple_mode;
};

void Debugger_GDB_MI::UpdateOnFrameChanged(bool wait)
{
    if (wait)
    {
        m_actions.Add(new dbg_mi::GDBBarrierAction);
    }

    DebuggerManager * dbg_manager = Manager::Get()->GetDebuggerManager();

    if (IsWindowReallyShown(dbg_manager->GetWatchesDialog()->GetWindow()) && !m_watches.empty())
    {
        for (dbg_mi::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        {
            if ((*it)->GetID().empty() && !(*it)->ForTooltip())
            {
                m_actions.Add(new dbg_mi::GDBWatchCreateAction(*it, m_watches, m_pLogger, true));
            }
        }

        m_actions.Add(new dbg_mi::GDBWatchesUpdateAction(m_watches, m_pLogger));
    }
}

void Debugger_GDB_MI::UpdateWhenStopped()
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

void Debugger_GDB_MI::RunQueue()
{
    if (m_executor.IsRunning())
    {
        Notifications notifications(this, m_executor, false);
        dbg_mi::DispatchResults(m_executor, m_actions, notifications);

        if (m_executor.IsStopped())
        {
            m_actions.Run(m_executor);
        }
    }
}

void Debugger_GDB_MI::ParseOutput(wxString const & str)
{
    if (!str.IsEmpty())
    {
        bool bProcessedOutput = false;
        // See CodeLite file Debugger\debuggergdb.cpp function DbgGdb::OnDataRead(..)
        wxArrayString const & lines = GetArrayFromString(str, '\n');

        if (lines.IsEmpty())
        {
            return;
        }

        //        // Prepend the partially saved line from previous iteration to the first line
        //        // of this iteration
        //        if (!m_gdbOutputIncompleteLine.empty())
        //        {
        //            lines.Item(0).Prepend(m_gdbOutputIncompleteLine);
        //            m_gdbOutputIncompleteLine.Clear();
        //        }

        //        // If the last line is in-complete, remove it from the array and keep it for next iteration
        //        wxChar lastChar = lines[lines.GetCount()-1].Last();
        //        if ((lastChar != '\n') &&  (lastChar != '\"'))
        //        {
        //            m_gdbOutputIncompleteLine = lines.Last();
        //            lines.RemoveAt(lines.GetCount() - 1);
        //        }

        for (size_t i = 0; i < lines.GetCount(); ++i)
        {
            wxString processLine = lines.Item(i);
            // Codelite!!!!            GetDebugeePID(processLine);
            processLine.Replace("(gdb)", "");
            processLine.Trim().Trim(false);

            // ">" - Shell line, probably user command line
            if (!processLine.empty() && !processLine.StartsWith(">"))
            {
                // m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Calling m_executor.ProcessOutput with:=>%s<=",processLine), dbg_mi::LogPaneLogger::LineType::Debug);
                m_executor.ProcessOutput(processLine);
                bProcessedOutput = true;
            }
            else
            {
                if (lines.Item(i).Contains("(gdb)"))
                {
                    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Line thrown away, was:=>%s<=", lines.Item(i)), dbg_mi::LogPaneLogger::LineType::Debug);
                }
                else
                {
                    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Line empty, was:=>%s<=", lines.Item(i)), dbg_mi::LogPaneLogger::LineType::Error);
                }
            }
        }

        if (bProcessedOutput)
        {
            m_actions.Run(m_executor);
        }
    }
}

struct StopNotification
{
    StopNotification(cbDebuggerPlugin * plugin, dbg_mi::GDBExecutor & executor) :
        m_plugin(plugin),
        m_executor(executor)
    {
    }

    void operator()(bool stopped)
    {
        m_executor.Stopped(stopped);

        if (!stopped)
        {
            m_plugin->ClearActiveMarkFromAllEditors();
        }
    }

    cbDebuggerPlugin * m_plugin;
    dbg_mi::GDBExecutor & m_executor;
};

bool Debugger_GDB_MI::Debug(bool breakOnEntry)
{
    m_hasStartUpError = false;
    ProjectManager & project_manager = *Manager::Get()->GetProjectManager();
    cbProject * project = project_manager.GetActiveProject();

    if (!project)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_mi::LogPaneLogger::LineType::Error);
        return false;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("starting debugger"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
    StartType start_type = breakOnEntry ? StartTypeStepInto : StartTypeRun;

    if (!EnsureBuildUpToDate(start_type))
    {
        return false;
    }

    if (!WaitingCompilerToFinish() && !m_executor.IsRunning() && !m_hasStartUpError)
    {
        return StartDebugger(project, start_type) == 0;
    }
    else
    {
        return true;
    }
}

bool Debugger_GDB_MI::CompilerFinished(bool compilerFailed, StartType startType)
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
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_mi::LogPaneLogger::LineType::Error);
        }
    }

    return false;
}

void Debugger_GDB_MI::ConvertDirectory(wxString & str, wxString base, bool relative)
{
    dbg_mi::ConvertDirectory(str, base, relative);
}

struct BreakpointMatchProject
{
    BreakpointMatchProject(cbProject * project) : project(project) {}
    bool operator()(cb::shared_ptr<dbg_mi::GDBBreakpoint> bp) const
    {
        return bp->GetProject() == project;
    }
    cbProject * project;
};


void Debugger_GDB_MI::CleanupWhenProjectClosed(cbProject * project)
{
    dbg_mi::GDBBreakpointsContainer::iterator bpIT = std::remove_if(m_breakpoints.begin(), m_breakpoints.end(), BreakpointMatchProject(project));
    if (bpIT != m_breakpoints.end())
    {
        m_breakpoints.erase(bpIT, m_breakpoints.end());
        cbBreakpointsDlg * dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dlg->Reload();
    }

    for (dbg_mi::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); )
    {
        cb::shared_ptr<dbg_mi::GDBWatch> watch = *it;
        if (watch->GetProject() == project)
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Remove watch for \"%s\"", watch->GetSymbol()), dbg_mi::LogPaneLogger::LineType::Debug);
            cbWatchesDlg *dialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            dialog->RemoveWatch(watch);  // This call removed the watch from the GUI and debugger
        }
        else
        {
            it++;
        }
    }

    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!prj)
    {
        cbBacktraceDlg* pDialogBacktrace = Manager::Get()->GetDebuggerManager()->GetBacktraceDialog();
        if (pDialogBacktrace)
        {
            pDialogBacktrace->Reload();
        }

        cbBreakpointsDlg* pDialogBreakpoint = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
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

        cbThreadsDlg* pDialogThreads = Manager::Get()->GetDebuggerManager()->GetThreadsDialog();
        if (pDialogThreads)
        {
            pDialogThreads->Reload();
        }

        cbWatchesDlg* pDialogWatches = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
        if (pDialogWatches)
        {
            pDialogWatches->RefreshUI();
        }
    }
}

int Debugger_GDB_MI::StartDebugger(cbProject * project, StartType start_type)
{
    //    ShowLog(true);
    m_executor.ClearQueueCommand();
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);

    if (!compiler)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no compiler found!"), dbg_mi::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 2;
    }

    if (!target)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no target found!"), dbg_mi::LogPaneLogger::LineType::Error);
        m_hasStartUpError = true;
        return 3;
    }

    // is gdb accessible, i.e. can we find it?
    wxString debugger = GetActiveConfigEx().GetDebuggerExecutable();
    wxString args = target->GetExecutionParameters();
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
                                 dbg_mi::LogPaneLogger::LineType::Debug);
    }
    int res = LaunchDebugger(project, debugger, debuggee, args, working_dir, 0, console, start_type);

    if (res != 0)
    {
        m_hasStartUpError = true;
        return res;
    }

    m_executor.SetAttachedPID(-1);
    m_pProject = project;
    m_hasStartUpError = false;

    if (oldLibPath != newLibPath)
    {
        wxSetEnv(CB_LIBRARY_ENVVAR, oldLibPath);
    }

    return 0;
}

int Debugger_GDB_MI::LaunchDebugger(cbProject * project,
                                    wxString const & debugger,
                                    wxString const & debuggee,
                                    wxString const & args,
                                    wxString const & working_dir,
                                    int pid,
                                    bool console,
                                    StartType start_type)
{
    m_current_frame.Reset();

    if (debugger.IsEmpty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no debugger executable found (full path)!"), dbg_mi::LogPaneLogger::LineType::Error);
        return 5;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("GDB path: %s"), debugger), dbg_mi::LogPaneLogger::LineType::UserDisplay);

    if (pid == 0)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE path: %s"), debuggee), dbg_mi::LogPaneLogger::LineType::UserDisplay);
    }

    wxString cmd;
    cmd << debugger;
    //    cmd << " -nx";          // don't run .gdbinit
    cmd << " -fullname ";   // report full-path filenames when breaking
    cmd << " -quiet";       // don't display version on startup
    cmd << " --interpreter=mi";

    if (pid == 0)
    {
        cmd << " -args " << debuggee;
    }
    else
    {
        cmd << " -pid=" << pid;
    }

    // start the gdb process
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Command-line: %s"), cmd), dbg_mi::LogPaneLogger::LineType::UserDisplay);

    if (pid == 0)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Working dir: %s"), working_dir), dbg_mi::LogPaneLogger::LineType::UserDisplay);
    }

    int ret = m_executor.LaunchProcess(cmd, working_dir, id_gdb_process, this, m_pLogger);

    if (ret != 0)
    {
        return ret;
    }

    m_executor.Stopped(true);
    //    m_executor.Execute("-enable-timings");
    CommitBreakpoints(true);
    CommitWatches();
    // Set program arguments
    m_actions.Add(new dbg_mi::GDBSimpleAction("-exec-arguments " + args));

    if (console)
    {
        wxString console_tty;
        m_console_pid = RunNixConsole(console_tty);

        if (m_console_pid >= 0)
        {
            m_actions.Add(new dbg_mi::GDBSimpleAction("-inferior-tty-set " + console_tty));
        }
    }

    dbg_mi::DebuggerConfiguration & active_config = GetActiveConfigEx();

    if (active_config.GetFlag(dbg_mi::DebuggerConfiguration::CheckPrettyPrinters))
    {
        m_actions.Add(new dbg_mi::GDBSimpleAction("-enable-pretty-printing"));
    }

#ifdef __WXMSW__
    if (console)
    {
        DoSendCommand("set new-console on");
    }
#endif

    wxArrayString comandLines = GetArrayFromString(active_config.GetInitialCommands(), '\n');
    size_t CommandLineCount = comandLines.GetCount();

    for (unsigned int i = 0; i < CommandLineCount; ++i)
    {
        DoSendCommand(comandLines[i]);
    }

    if (active_config.GetFlag(dbg_mi::DebuggerConfiguration::CatchExceptions))
    {
        DoSendCommand("catch throw");
        DoSendCommand("catch catch");
    }

    wxString directorySearchPaths = wxEmptyString;
    const wxArrayString& pdirs = ParseSearchDirs(project);
    for (size_t i = 0; i < pdirs.GetCount(); ++i)
    {
        directorySearchPaths.Append(pdirs[i]);
        directorySearchPaths.Append(wxPATH_SEP);
    }

    if (!directorySearchPaths.IsEmpty())
    {
        DoSendCommand(wxString::Format("directory %s", directorySearchPaths));
    }

    if (pid == 0)
    {
        switch (start_type)
        {
            case StartTypeRun:
                CommitRunCommand("-exec-run");
                break;

            case StartTypeStepInto:
                CommitRunCommand("-exec-step");
                break;

            case StartTypeUnknown:
                // Keep compiler happy with this case
                break;
        }
    }

    m_actions.Run(m_executor);
    m_timer_poll_debugger.Start(20);
    SwitchToDebuggingLayout();
    m_pid_attached = pid;
    return 0;
}

void Debugger_GDB_MI::CommitBreakpoints(bool force)
{
    for (dbg_mi::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        // FIXME (obfuscated#): pointers inside the vector can be dangerous!!!
        if ((*it)->GetIndex() == -1 || force)
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("m_actions.Add(GDBBreakpointAddAction: Filename:%s Line:%s)",
                                     (*it)->GetLocation(), (*it)->GetLineString()), dbg_mi::LogPaneLogger::LineType::Debug);
            m_actions.Add(new dbg_mi::GDBBreakpointAddAction(*it, m_pLogger));
        }
    }

    for (dbg_mi::GDBBreakpointsContainer::const_iterator it = m_temporary_breakpoints.begin(); it != m_temporary_breakpoints.end(); ++it)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("AddStringCommand: =>-break-insert -t %s:%d<=", (*it)->GetLocation(), (*it)->GetLine()), dbg_mi::LogPaneLogger::LineType::Command);
        wxString sLocation = (*it)->GetLocation();
        AddStringCommand(wxString::Format("-break-insert -t %s:%d", sLocation.c_str(), (*it)->GetLine()));
    }

    m_temporary_breakpoints.clear();
}

void Debugger_GDB_MI::CommitWatches()
{
    if (m_watches.empty())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "No watches", dbg_mi::LogPaneLogger::LineType::Debug);
    }

    for (dbg_mi::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Watch clear for symbol %s", (*it)->GetSymbol()), dbg_mi::LogPaneLogger::LineType::Debug);
        (*it)->Reset();
    }

    if (!m_watches.empty())
    {
        CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
        event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
        Manager::Get()->ProcessEvent(event);
    }
}

void Debugger_GDB_MI::CommitRunCommand(wxString const & command)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("=>%s<=", command), dbg_mi::LogPaneLogger::LineType::Command);
    m_current_frame.Reset();
    m_actions.Add(new dbg_mi::GDBRunAction<StopNotification>(this,
                                                             command,
                                                             StopNotification(this, m_executor),
                                                             m_pLogger)
                 );
}

bool Debugger_GDB_MI::RunToCursor(const wxString & filename, int line, const wxString & /*line_text*/)
{
    if (IsRunning())
    {
        if (IsStopped())
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("=>-exec-until %s:%d<=", filename, line), dbg_mi::LogPaneLogger::LineType::Command);
            CommitRunCommand(wxString::Format("-exec-until %s:%d", filename.c_str(), line));
            return true;
        }
        else
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("filename:%s line:%d", filename, line), dbg_mi::LogPaneLogger::LineType::Debug);
        }

        return false;
    }
    else
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("push_back %s:%d", filename, line), dbg_mi::LogPaneLogger::LineType::Command);
        cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);
        cb::shared_ptr<dbg_mi::GDBBreakpoint> ptr(new dbg_mi::GDBBreakpoint(project, m_pLogger, filename, line));
        m_temporary_breakpoints.push_back(ptr);
        return Debug(false);
    }
}

void Debugger_GDB_MI::SetNextStatement(const wxString & filename, int line)
{
    if (IsStopped())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("-break-insert -t & -exec-jump for filename:=>%s<= line:%d", filename, line), dbg_mi::LogPaneLogger::LineType::Command);
        AddStringCommand(wxString::Format("-break-insert -t %s:%d", filename.c_str(), line));
        CommitRunCommand(wxString::Format("-exec-jump %s:%d", filename.c_str(), line));
    }
}

void Debugger_GDB_MI::Continue()
{
    if (!IsStopped() && !m_executor.Interupting())
    {
        dbg_mi::LogPaneLogger * logger = m_executor.GetLogger();

        if (logger)
        {
            logger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Continue failed -> debugger is not interupted!"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
        }

        return;
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_GDB_MI::Continue", dbg_mi::LogPaneLogger::LineType::Debug);
    CommitRunCommand("-exec-continue");
}

void Debugger_GDB_MI::Next()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "=>-exec-next<=", dbg_mi::LogPaneLogger::LineType::Command);
    CommitRunCommand("-exec-next");
}

void Debugger_GDB_MI::NextInstruction()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "=>-exec-next-instruction<=", dbg_mi::LogPaneLogger::LineType::Command);
    CommitRunCommand("-exec-next-instruction");
}

void Debugger_GDB_MI::StepIntoInstruction()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "=>-exec-step-instruction<=", dbg_mi::LogPaneLogger::LineType::Command);
    CommitRunCommand("-exec-step-instruction");
}

void Debugger_GDB_MI::Step()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "=>-exec-step<=", dbg_mi::LogPaneLogger::LineType::Command);
    CommitRunCommand("-exec-step");
}

void Debugger_GDB_MI::StepOut()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "=>-exec-finish<=", dbg_mi::LogPaneLogger::LineType::Command);
    CommitRunCommand("-exec-finish");
}

void Debugger_GDB_MI::Break()
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, "future?", dbg_mi::LogPaneLogger::LineType::Command);
    m_executor.Interupt(false);
    // cbEVT_DEBUGGER_PAUSED will be sent, when the debugger has pause for real
}

void Debugger_GDB_MI::Stop()
{
    if (!IsRunning())
    {
        return;
    }

    ClearActiveMarkFromAllEditors();
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
    m_executor.ForceStop();
    MarkAsStopped();
}

bool Debugger_GDB_MI::IsRunning() const
{
    return m_executor.IsRunning();
}

bool Debugger_GDB_MI::IsStopped() const
{
    return m_executor.IsStopped();
}

bool Debugger_GDB_MI::IsBusy() const
{
    return !m_executor.IsStopped();
}

int Debugger_GDB_MI::GetExitCode() const
{
    return m_exit_code;
}

int Debugger_GDB_MI::GetStackFrameCount() const
{
    return m_backtrace.size();
}

cb::shared_ptr<const cbStackFrame> Debugger_GDB_MI::GetStackFrame(int index) const
{
    return m_backtrace[index];
}

struct GDBSwitchToFrameNotification
{
    GDBSwitchToFrameNotification(Debugger_GDB_MI * plugin) :
        m_plugin(plugin)
    {
    }

    void operator()(dbg_mi::ResultParser const & result, int frame_number, bool user_action)
    {
        if (m_frame_number < m_plugin->GetStackFrameCount())
        {
            dbg_mi::GDBCurrentFrame & current_frame = m_plugin->GetGDBCurrentFrame();

            if (user_action)
            {
                current_frame.GDBSwitchToFrame(frame_number);
            }
            else
            {
                current_frame.Reset();
                current_frame.SetFrame(frame_number);
            }

            cb::shared_ptr<const cbStackFrame> frame = m_plugin->GetStackFrame(frame_number);
            wxString const & filename = frame->GetFilename();
            long line;

            if (frame->GetLine().ToLong(&line))
            {
                current_frame.SetPosition(filename, line);
                m_plugin->SyncEditor(filename, line, true);
            }

            m_plugin->UpdateOnFrameChanged(true);
            Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
        }
    }

    Debugger_GDB_MI * m_plugin;
    int m_frame_number;
};

void Debugger_GDB_MI::SwitchToFrame(int number)
{
    if (IsRunning() && IsStopped())
    {
        if (number < static_cast<int>(m_backtrace.size()))
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("adding commnad"), dbg_mi::LogPaneLogger::LineType::Debug);
            int frame = m_backtrace[number]->GetNumber();
            typedef dbg_mi::GDBSwitchToFrame<GDBSwitchToFrameNotification> SwitchType;
            m_actions.Add(new SwitchType(frame, GDBSwitchToFrameNotification(this), true));
        }
    }
}

int Debugger_GDB_MI::GetActiveStackFrame() const
{
    return m_current_frame.GetStackFrame();
}

cb::shared_ptr<cbBreakpoint> Debugger_GDB_MI::AddBreakpoint(const wxString & filename, int line)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("%s:%d"), filename, line), dbg_mi::LogPaneLogger::LineType::Debug);
    cbProject * project = Manager::Get()->GetProjectManager()->FindProjectForFile(filename, nullptr, false, false);
    cb::shared_ptr<dbg_mi::GDBBreakpoint> ptr(new dbg_mi::GDBBreakpoint(project, m_pLogger, filename, line));
    m_breakpoints.push_back(ptr);

    if (IsRunning())
    {
        if (!IsStopped())
        {
            m_executor.Interupt();
            m_actions.Add(new dbg_mi::GDBBreakpointAddAction(ptr, m_pLogger));
            Continue();
        }
        else
        {
            m_actions.Add(new dbg_mi::GDBBreakpointAddAction(ptr, m_pLogger));
        }
    }

    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints.back());
}

cb::shared_ptr<cbBreakpoint> Debugger_GDB_MI::AddBreakpoint(cb::shared_ptr<dbg_mi::GDBBreakpoint> bp)
{
    m_breakpoints.push_back(bp);

    if (IsRunning())
    {
        if (!IsStopped())
        {
            m_executor.Interupt();
            m_actions.Add(new dbg_mi::GDBBreakpointAddAction(bp, m_pLogger));
            Continue();
        }
        else
        {
            m_actions.Add(new dbg_mi::GDBBreakpointAddAction(bp, m_pLogger));
        }
    }

    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints.back());
}

cb::shared_ptr<cbBreakpoint> Debugger_GDB_MI::AddDataBreakpoint(const wxString& dataExpression)
{
    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("dataExpression : %s", dataExpression), dbg_mi::LogPaneLogger::LineType::Warning);

    dbg_mi::DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), dataExpression, true, 1);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        bool enabled = dlg.IsBreakpointEnabled();
        const wxString& newDataExpression = dlg.GetDataExpression();
        int sel = dlg.GetSelection();

        cb::shared_ptr<dbg_mi::GDBBreakpoint> bp(new dbg_mi::GDBBreakpoint(m_pProject, m_pLogger));
        bp->SetType(dbg_mi::GDBBreakpoint::BreakpointType::bptData);
        bp->SetIsEnabled(enabled);
        bp->SetBreakAddress(newDataExpression);
        bp->SetIsBreakOnRead(sel != 1);
        bp->SetIsBreakOnWrite(sel != 0);

        AddBreakpoint(bp);

        return bp;
    }
    else
    {
        return cb::shared_ptr<cbBreakpoint>();
    }
}

int Debugger_GDB_MI::GetBreakpointsCount() const
{
    return m_breakpoints.size();
}

cb::shared_ptr<cbBreakpoint> Debugger_GDB_MI::GetBreakpoint(int index)
{
    return cb::static_pointer_cast<cbBreakpoint>(m_breakpoints[index]);
}

cb::shared_ptr<const cbBreakpoint> Debugger_GDB_MI::GetBreakpoint(int index) const
{
    return cb::static_pointer_cast<const cbBreakpoint>(m_breakpoints[index]);
}

void Debugger_GDB_MI::UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    dbg_mi::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);

    if (it == m_breakpoints.end())
    {
        return;
    }

    cb::shared_ptr<dbg_mi::GDBBreakpoint> bp = cb::static_pointer_cast<dbg_mi::GDBBreakpoint>(breakpoint);
    bool reset = false;
    switch (bp->GetType())
    {
        case dbg_mi::GDBBreakpoint::bptCode:
        {
            dbg_mi::EditBreakpointDlg dlg(*bp, Manager::Get()->GetAppWindow());
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
            {
                *bp = dlg.GetBreakpoint();
                reset = true;
            }
            break;
        }
        case dbg_mi::GDBBreakpoint::bptData:
        {
            int old_sel = 0;
            if (bp->GetIsBreakOnRead() && bp->GetIsBreakOnWrite())
            {
                old_sel = 2;
            }
            else if (!bp->GetIsBreakOnRead() && bp->GetIsBreakOnWrite())
            {
                old_sel = 1;
            }

            dbg_mi::DataBreakpointDlg dlg(Manager::Get()->GetAppWindow(), bp->GetBreakAddress(), bp->GetIsEnabled(), old_sel);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
            {
                bp->SetIsEnabled(dlg.IsEnabled());
                bp->SetIsBreakOnRead(dlg.GetSelection() != 1);
                bp->SetIsBreakOnWrite(dlg.GetSelection() != 0);
                bp->SetBreakAddress(dlg.GetDataExpression());
                reset = true;
            }
            break;
        }
        case dbg_mi::GDBBreakpoint::bptFunction:
            return;

        default:
            return;
    }

    if (reset)
    {
        bool debuggerIsRunning = !IsStopped();
        if (debuggerIsRunning)
        {
            m_executor.Interupt(true);
        }

        DeleteBreakpoint(bp);
        AddBreakpoint(bp);

        if (debuggerIsRunning)
        {
            Continue();
        }
    }
}

void Debugger_GDB_MI::DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    dbg_mi::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);

    if (it != m_breakpoints.end())
    {

        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("%s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
                                 dbg_mi::LogPaneLogger::LineType::Debug);
        cb::shared_ptr<dbg_mi::GDBBreakpoint> pBrkPt = *it;

        int index = pBrkPt->GetIndex();

        if (index != -1)
        {
            dbg_mi::GDBBreakpoint::BreakpointType bpType = pBrkPt->GetType();
            switch (bpType)
            {
                case dbg_mi::GDBBreakpoint::bptCode:
                case dbg_mi::GDBBreakpoint::bptData:
                {
                    if (!IsStopped())
                    {
                        m_executor.Interupt();
                        AddStringCommand(wxString::Format("-break-delete %d", index));
                        Continue();
                    }
                    else
                    {
                        AddStringCommand(wxString::Format("-break-delete %d", index));
                    }
                    break;
                }

                case dbg_mi::GDBBreakpoint::bptFunction:
// #warning dbg_mi::GDBBreakpoint::BreakpointType::bptFunction not supported yet!!
                    #ifdef __MINGW32__
                        if (IsDebuggerPresent())
                        {
                            DebugBreak();
                        }
                    #endif // __MINGW32__
                    break;

                default:
                    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Unknown breakpoint type: %d",  bpType), dbg_mi::LogPaneLogger::LineType::Error);
                    break;
            }
        }

        m_breakpoints.erase(it);
    }
}

void Debugger_GDB_MI::DeleteAllBreakpoints()
{
    if (IsRunning())
    {
        wxString breaklist;

        for (dbg_mi::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
        {
            dbg_mi::GDBBreakpoint & current = **it;

            if (current.GetIndex() != -1)
            {
                breaklist += wxString::Format(" %d", current.GetIndex());
            }
        }

        if (!breaklist.empty())
        {
            if (!IsStopped())
            {
                m_executor.Interupt();
                AddStringCommand("-break-delete" + breaklist);
                Continue();
            }
            else
            {
                AddStringCommand("-break-delete" + breaklist);
            }
        }
    }

    m_breakpoints.clear();
}

void Debugger_GDB_MI::ShiftBreakpoint(int index, int lines_to_shift)
{
    if (index < 0 || index >= static_cast<int>(m_breakpoints.size()))
    {
        return;
    }

    cb::shared_ptr<dbg_mi::GDBBreakpoint> bp = m_breakpoints[index];
    bp->SetShiftLines(lines_to_shift);

    if (IsRunning())
    {
        // just remove the breakpoints as they will become invalid
        if (!IsStopped())
        {
            m_executor.Interupt();

            if (bp->GetIndex() >= 0)
            {
                AddStringCommand(wxString::Format("-break-delete %d", bp->GetIndex()));
                bp->SetIndex(-1);
            }

            Continue();
        }
        else
        {
            if (bp->GetIndex() >= 0)
            {
                AddStringCommand(wxString::Format("-break-delete %d", bp->GetIndex()));
                bp->SetIndex(-1);
            }
        }
    }
}

void Debugger_GDB_MI::EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable)
{
    dbg_mi::GDBBreakpointsContainer::iterator it = std::find(m_breakpoints.begin(), m_breakpoints.end(), breakpoint);

    if (it != m_breakpoints.end())
    {

        int index = (*it)->GetIndex();

        if ((*it)->IsEnabled() == enable)
        {
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("breakpoint found but no change needed: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
                                     dbg_mi::LogPaneLogger::LineType::Debug);
            // N change required!
            return;
        }
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("breakpoint found and change needed: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
                                 dbg_mi::LogPaneLogger::LineType::Debug);
        if (index != -1)
        {
            wxString wxBreakCommand;
            if (enable)
            {
                wxBreakCommand = wxString::Format("-break-enable %d", index);
            }
            else
            {
                wxBreakCommand = wxString::Format("-break-disable %d", index);
            }
            if (m_executor.IsStopped())
            {
                AddStringCommand(wxBreakCommand);
            }
            else
            {
                m_executor.Interupt();
                AddStringCommand(wxBreakCommand);
                Continue();
            }
        }
        (*it)->SetEnabled(enable);
    }
    else
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("Breakpoint NOT FOUND: %s:%d"), breakpoint->GetLocation(), breakpoint->GetLine()),
                                 dbg_mi::LogPaneLogger::LineType::Warning);

    }
}

int Debugger_GDB_MI::GetThreadsCount() const
{
    return m_threads.size();
}

cb::shared_ptr<const cbThread> Debugger_GDB_MI::GetThread(int index) const
{
    return m_threads[index];
}

bool Debugger_GDB_MI::SwitchToThread(int thread_number)
{
    if (IsStopped())
    {
        dbg_mi::GDBSwitchToThread<Notifications> * a;
        a = new dbg_mi::GDBSwitchToThread<Notifications>(thread_number,
                                                      m_pLogger,
                                                      Notifications(this, m_executor, true)
                                                     );
        m_actions.Add(a);
        return true;
    }
    else
    {
        return false;
    }
}

cb::shared_ptr<cbWatch> Debugger_GDB_MI::AddWatch(const wxString & symbol, cb_unused bool update)
{
    cb::shared_ptr<dbg_mi::GDBWatch> watch(new dbg_mi::GDBWatch(m_pProject, m_pLogger, symbol, false));
    m_watches.push_back(watch);

    if (IsRunning())
    {
        m_actions.Add(new dbg_mi::GDBWatchCreateAction(watch, m_watches, m_pLogger, true));
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", watch->GetSymbol()), dbg_mi::LogPaneLogger::LineType::Debug);
    return watch;
}

cb::shared_ptr<cbWatch> Debugger_GDB_MI::AddWatch(dbg_mi::GDBWatch * watch, cb_unused bool update)
{
    cb::shared_ptr<dbg_mi::GDBWatch> w(watch);
    m_watches.push_back(w);

    if (IsRunning())
    {
        m_actions.Add(new dbg_mi::GDBWatchCreateAction(w, m_watches, m_pLogger, true));
    }

    m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", w->GetSymbol()), dbg_mi::LogPaneLogger::LineType::Debug);
    return w;
}

cb::shared_ptr<cbWatch> Debugger_GDB_MI::AddMemoryRange(uint64_t llAddress, uint64_t llSize, const wxString &symbol, bool update)
{
    cb::shared_ptr<dbg_mi::GDBMemoryRangeWatch> watch(new dbg_mi::GDBMemoryRangeWatch(m_pProject, m_pLogger, llAddress, llSize, symbol));

    watch->SetSymbol(symbol);
    watch->SetAddress(llAddress);

    m_memoryRanges.push_back(watch);
    m_mapWatchesToType[watch] = dbg_mi::GDBWatchType::MemoryRange;

    if (IsRunning())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Adding watch for: address: %#018llx  size:%lld", llAddress, llSize), dbg_mi::LogPaneLogger::LineType::Warning);
        m_actions.Add(new dbg_mi::GDBMemoryRangeWatchCreateAction(watch, m_pLogger));
    }

    return watch;
}

void Debugger_GDB_MI::AddTooltipWatch(const wxString & symbol, wxRect const & rect)
{
    cb::shared_ptr<dbg_mi::GDBWatch> w(new dbg_mi::GDBWatch(m_pProject, m_pLogger, symbol, true));
    m_watches.push_back(w);

    if (IsRunning())
    {
        m_actions.Add(new dbg_mi::GDBWatchCreateTooltipAction(w, m_watches, m_pLogger, rect));
    }
}

void Debugger_GDB_MI::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (IsRunning())
    {
        if (IsStopped())
        {
            AddStringCommand("-var-delete " + (*it)->GetID());
        }
        else
        {
            m_executor.Interupt();
            AddStringCommand("-var-delete " + (*it)->GetID());
            Continue();
        }
    }

    m_watches.erase(it);
}

bool Debugger_GDB_MI::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    if (watch == m_WatchLocalsandArgs)
    {
        return true;
    }
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);
    return it != m_watches.end();
}

bool Debugger_GDB_MI::IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> &watch)
{
    dbg_mi::GDBMapWatchesToType::const_iterator it = m_mapWatchesToType.find(watch);
    if (it == m_mapWatchesToType.end())
    {
        return false;
    }
    else
    {
        return (it->second == dbg_mi::GDBWatchType::MemoryRange);
    }
}

void Debugger_GDB_MI::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
{
    // not supported for child nodes or memory ranges!
    if (watch->GetParent() || IsMemoryRangeWatch(watch))
    {
        return;
    }

    cb::shared_ptr<dbg_mi::GDBWatch> real_watch = cb::static_pointer_cast<dbg_mi::GDBWatch>(watch);
    dbg_mi::EditWatchDlg dlg(real_watch, nullptr);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        DoWatches();
    }
}

bool Debugger_GDB_MI::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value)
{
    if (!IsStopped() || !IsRunning())
    {
        return false;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return false;
    }

    cb::shared_ptr<dbg_mi::GDBWatch> real_watch = cb::static_pointer_cast<dbg_mi::GDBWatch>(watch);
    AddStringCommand("-var-assign " + real_watch->GetID() + " " + value);
    //    m_actions.Add(new dbg_mi::GDBWatchSetValueAction(*it, static_cast<dbg_mi::GDBWatch*>(watch), value, m_pLogger));
    dbg_mi::Action * update_action = new dbg_mi::GDBWatchesUpdateAction(m_watches, m_pLogger);
    update_action->SetWaitPrevious(true);
    m_actions.Add(update_action);
    return true;
}

void Debugger_GDB_MI::ExpandWatch(cb::shared_ptr<cbWatch> watch)
{
    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_mi::GDBWatch> real_watch = cb::static_pointer_cast<dbg_mi::GDBWatch>(watch);

        if (!real_watch->HasBeenExpanded())
        {
            m_actions.Add(new dbg_mi::GDBWatchExpandedAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_GDB_MI::CollapseWatch(cb::shared_ptr<cbWatch> watch)
{
    if (!IsStopped() || !IsRunning())
    {
        return;
    }

    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it != m_watches.end())
    {
        cb::shared_ptr<dbg_mi::GDBWatch> real_watch = cb::static_pointer_cast<dbg_mi::GDBWatch>(watch);

        if (real_watch->HasBeenExpanded() && real_watch->DeleteOnCollapse())
        {
            m_actions.Add(new dbg_mi::GDBWatchCollapseAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void Debugger_GDB_MI::UpdateWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
    dbg_mi::GDBWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (IsRunning())
    {
        m_actions.Add(new dbg_mi::GDBWatchCreateAction(*it, m_watches, m_pLogger, false));
    }
}

void Debugger_GDB_MI::DoWatches()
{
    if (!IsRunning())
    {
        return;
    }

    dbg_mi::DebuggerConfiguration &config = GetActiveConfigEx();

    bool bWatchFuncLocalsArgs = config.GetFlag(dbg_mi::DebuggerConfiguration::WatchFuncLocalsArgs);
    if (bWatchFuncLocalsArgs)
    {
        if (m_WatchLocalsandArgs == nullptr)
        {
            m_WatchLocalsandArgs = cb::shared_ptr<dbg_mi::GDBWatch>(new dbg_mi::GDBWatch(m_pProject, m_pLogger, "Function locals and arguments", false));
            m_WatchLocalsandArgs->Expand(true);
            m_WatchLocalsandArgs->MarkAsChanged(false);
            cbWatchesDlg *watchesDialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            watchesDialog->AddSpecialWatch(m_WatchLocalsandArgs, true);
        }
    }

    m_actions.Add(new dbg_mi::GDBStackVariables(m_pLogger, m_WatchLocalsandArgs, bWatchFuncLocalsArgs));

    // Update watches now
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(int(cbDebuggerPlugin::DebugWindows::Watches));
    Manager::Get()->ProcessEvent(event);
}


void Debugger_GDB_MI::SendCommand(const wxString & cmd, bool debugLog)
{
    if (!IsRunning())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Command will not be executed because the debugger is not running!"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
        return;
    }

    if (!IsStopped())
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("Command will not be executed because the debugger/debuggee is not paused/interupted!"), dbg_mi::LogPaneLogger::LineType::UserDisplay);
        return;
    }

    if (cmd.empty())
    {
        return;
    }

    DoSendCommand(cmd);
}

void Debugger_GDB_MI::DoSendCommand(const wxString & cmd)
{
    wxString escaped_cmd = cmd;
    escaped_cmd.Replace("\n", "\\n", true);

    if (escaped_cmd[0] == '-')
    {
        AddStringCommand(escaped_cmd);
    }
    else
    {
        escaped_cmd.Replace("\\", "\\\\", true);
        AddStringCommand("-interpreter-exec console \"" + escaped_cmd + "\"");
    }
}

void Debugger_GDB_MI::AttachToProcess(const wxString & pid)
{
    m_pProject = NULL;
    long number;

    if (!pid.ToLong(&number))
    {
        return;
    }

    LaunchDebugger(m_pProject,
                   GetActiveConfigEx().GetDebuggerExecutable(),
                   wxEmptyString,
                   wxEmptyString,
                   wxEmptyString,
                   number,
                   false,
                   StartTypeRun);
    m_executor.SetAttachedPID(number);
}

void Debugger_GDB_MI::DetachFromProcess()
{
    AddStringCommand(wxString::Format("-target-detach %ld", m_executor.GetAttachedPID()));
}

bool Debugger_GDB_MI::IsAttachedToProcess() const
{
    return m_pid_attached != 0;
}

void Debugger_GDB_MI::RequestUpdate(DebugWindows window)
{
    if (!IsStopped())
    {
        return;
    }

    switch (window)
    {
        case Backtrace:
            {
                struct Switcher : dbg_mi::GDBSwitchToFrameInvoker
                {
                    Switcher(Debugger_GDB_MI * plugin, dbg_mi::ActionsMap & actions) :
                        m_plugin(plugin),
                        m_actions(actions)
                    {
                    }

                    virtual void Invoke(int frame_number)
                    {
                        typedef dbg_mi::GDBSwitchToFrame<GDBSwitchToFrameNotification> SwitchType;
                        m_actions.Add(new SwitchType(frame_number, GDBSwitchToFrameNotification(m_plugin), false));
                    }

                    Debugger_GDB_MI * m_plugin;
                    dbg_mi::ActionsMap & m_actions;
                };
                Switcher * switcher = new Switcher(this, m_actions);
                m_actions.Add(new dbg_mi::GDBGenerateBacktrace(switcher, m_backtrace, m_current_frame, m_pLogger));
            }
            break;

        case Threads:
            m_actions.Add(new dbg_mi::GDBGenerateThreadsList(m_threads, m_current_frame.GetThreadId(), m_pLogger));
            break;

        case CPURegisters:
            {
                m_actions.Add(new dbg_mi::GDBGenerateCPUInfoRegisters(m_pLogger));
            }
            break;

        case Disassembly:
            {
                wxString flavour = GetActiveConfigEx().GetDisassemblyFlavorCommand();
                m_actions.Add(new dbg_mi::GDBDisassemble(flavour, m_pLogger));
            }
            break;

        case ExamineMemory:
            {
                cbExamineMemoryDlg * dialog = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();
                wxString memaddress = dialog->GetBaseAddress();

                // Check for blank memory string
                if (!memaddress.IsEmpty())
                {
                    m_actions.Add(new dbg_mi::GDBGenerateExamineMemory(m_pLogger));
                }
            }
            break;

        case MemoryRange:
            m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, _("DebugWindows MemoryRange called!!"), dbg_mi::LogPaneLogger::LineType::Error);
#ifdef __MINGW32__
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
#endif // __MINGW32__
            break;

        case Watches:
            if (IsWindowReallyShown(Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->GetWindow()))
            {
                DoWatches();
            }
            break;

        default:
            break;
    }
}

void Debugger_GDB_MI::GetCurrentPosition(wxString & filename, int & line)
{
    m_current_frame.GetPosition(filename, line);
}

void Debugger_GDB_MI::KillConsole()
{
    if (m_console_pid >= 0)
    {
        wxKill(m_console_pid);
        m_console_pid = -1;
    }
}

void Debugger_GDB_MI::OnValueTooltip(const wxString & token, const wxRect & evalRect)
{
    AddTooltipWatch(token, evalRect);
}

bool Debugger_GDB_MI::ShowValueTooltip(int style)
{
    if (!IsRunning() || !IsStopped())
    {
        return false;
    }

    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER && style != wxSCI_C_WORD2)
    {
        return false;
    }

    return true;
}

void Debugger_GDB_MI::StripQuotes(wxString& str)
{
    if ((str.GetChar(0) == '\"') && (str.GetChar(str.Length() - 1) == '\"'))
    {
        str = str.Mid(1, str.Length() - 2);
    }
}

void Debugger_GDB_MI::ConvertToGDBFriendly(wxString& str)
{
    if (str.IsEmpty())
        return;

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

void Debugger_GDB_MI::ConvertToGDBDirectory(wxString& str, wxString base, bool relative)
{
    if (str.IsEmpty())
        return;

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
        else if (!base.IsEmpty() && str.GetChar(0) != '/')
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


wxArrayString Debugger_GDB_MI::ParseSearchDirs(cbProject * pProject)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!

    wxArrayString dirs;
    const TiXmlElement* elem = static_cast<const TiXmlElement*>(pProject->GetExtensionsNode());
    if (elem)
    {
        const TiXmlElement* conf = elem->FirstChildElement("debugger");
        if (conf)
        {
            const TiXmlElement* pathsElem = conf->FirstChildElement("search_path");
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

TiXmlElement* Debugger_GDB_MI::GetElementForSaving(cbProject &project, const char *elementsToClear)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!

    TiXmlElement *elem = static_cast<TiXmlElement*>(project.GetExtensionsNode());

    // since rev4332, the project keeps a copy of the <Extensions> element
    // and re-uses it when saving the project (so to avoid losing entries in it
    // if plugins that use that element are not loaded atm).
    // so, instead of blindly inserting the element, we must first check it's
    // not already there (and if it is, clear its contents)
    TiXmlElement* node = elem->FirstChildElement("debugger");
    if (!node)
        node = elem->InsertEndChild(TiXmlElement("debugger"))->ToElement();

    for (TiXmlElement* child = node->FirstChildElement(elementsToClear);
         child;
         child = node->FirstChildElement(elementsToClear))
    {
        node->RemoveChild(child);
    }
    return node;
}


void Debugger_GDB_MI::SetSearchDirs(cbProject &project, const wxArrayString &dirs)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!

    TiXmlElement* node = GetElementForSaving(project, "search_path");
    if (dirs.GetCount() > 0)
    {
        for (size_t i = 0; i < dirs.GetCount(); ++i)
        {
            TiXmlElement* path = node->InsertEndChild(TiXmlElement("search_path"))->ToElement();
            path->SetAttribute("add", cbU2C(dirs[i]));
        }
    }
}

dbg_mi::RemoteDebuggingMap Debugger_GDB_MI::ParseRemoteDebuggingMap(cbProject &project)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!

    dbg_mi::RemoteDebuggingMap map;
    const TiXmlElement* elem = static_cast<const TiXmlElement*>(project.GetExtensionsNode());
    if (elem)
    {
        const TiXmlElement* conf = elem->FirstChildElement("debugger");
        if (conf)
        {
            const TiXmlElement* rdElem = conf->FirstChildElement("remote_debugging");
            while (rdElem)
            {
                wxString targetName = cbC2U(rdElem->Attribute("target"));
                ProjectBuildTarget* bt = project.GetBuildTarget(targetName);

                const TiXmlElement* rdOpt = rdElem->FirstChildElement("options");
                if (rdOpt)
                {
                    dbg_mi::RemoteDebugging rd;

                    if (rdOpt->Attribute("conn_type"))
                    {
                        rd.connType = (dbg_mi::RemoteDebugging::ConnectionType)atol(rdOpt->Attribute("conn_type"));
                    }
                    if (rdOpt->Attribute("serial_port"))
                    {
                        rd.serialPort = cbC2U(rdOpt->Attribute("serial_port"));
                    }
                    if (rdOpt->Attribute("serial_baud"))
                    {
                        rd.serialBaud = cbC2U(rdOpt->Attribute("serial_baud"));
                    }
                    if (rd.serialBaud.empty())
                    {
                        rd.serialBaud = "115200";
                    }
                    if (rdOpt->Attribute("ip_address"))
                    {
                        rd.ip = cbC2U(rdOpt->Attribute("ip_address"));
                    }
                    if (rdOpt->Attribute("ip_port"))
                    {
                        rd.ipPort = cbC2U(rdOpt->Attribute("ip_port"));
                    }
                    if (rdOpt->Attribute("additional_cmds"))
                    {
                        rd.additionalCmds = cbC2U(rdOpt->Attribute("additional_cmds"));
                    }
                    if (rdOpt->Attribute("additional_cmds_before"))
                    {
                        rd.additionalCmdsBefore = cbC2U(rdOpt->Attribute("additional_cmds_before"));
                    }
                    if (rdOpt->Attribute("skip_ld_path"))
                    {
                        rd.skipLDpath = cbC2U(rdOpt->Attribute("skip_ld_path")) != "0";
                    }
                    if (rdOpt->Attribute("extended_remote"))
                    {
                        rd.extendedRemote = cbC2U(rdOpt->Attribute("extended_remote")) != "0";
                    }
                    if (rdOpt->Attribute("additional_shell_cmds_after"))
                    {
                        rd.additionalShellCmdsAfter = cbC2U(rdOpt->Attribute("additional_shell_cmds_after"));
                    }
                    if (rdOpt->Attribute("additional_shell_cmds_before"))
                    {
                        rd.additionalShellCmdsBefore = cbC2U(rdOpt->Attribute("additional_shell_cmds_before"));
                    }

                    map.insert(map.end(), std::make_pair(bt, rd));
                }

                rdElem = rdElem->NextSiblingElement("remote_debugging");
            }
        }
    }
    return map;
}

void Debugger_GDB_MI::SetRemoteDebuggingMap(cbProject &project, const dbg_mi::RemoteDebuggingMap &rdMap)
{
    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!

    TiXmlElement* node = GetElementForSaving(project, "remote_debugging");

    if (!rdMap.empty())
    {
        typedef std::map<wxString, const dbg_mi::RemoteDebugging*> MapTargetNameToRD;
        MapTargetNameToRD mapTargetNameToRD;

        for (dbg_mi::RemoteDebuggingMap::const_iterator it = rdMap.begin(); it != rdMap.end(); ++it)
        {
            wxString targetName = (it->first ? it->first->GetTitle() : wxString());
            const dbg_mi::RemoteDebugging& rd = it->second;
            mapTargetNameToRD.emplace(targetName, &rd);
        }

        for (MapTargetNameToRD::const_iterator it = mapTargetNameToRD.begin();
             it != mapTargetNameToRD.end();
             ++it)
        {
            const dbg_mi::RemoteDebugging& rd = *it->second;

            // if no different than defaults, skip it
            if (rd.serialPort.IsEmpty() &&
                rd.serialBaud == "115200" &&
                rd.ip.IsEmpty() &&
                rd.ipPort.IsEmpty() &&
                !rd.skipLDpath &&
                !rd.extendedRemote &&
                rd.additionalCmds.IsEmpty() &&
                rd.additionalCmdsBefore.IsEmpty() &&
                rd.additionalShellCmdsAfter.IsEmpty() &&
                rd.additionalShellCmdsBefore.IsEmpty())
            {
                continue;
            }

            TiXmlElement* rdnode = node->InsertEndChild(TiXmlElement("remote_debugging"))->ToElement();
            if (!it->first.empty())
                rdnode->SetAttribute("target", cbU2C(it->first));

            TiXmlElement* tgtnode = rdnode->InsertEndChild(TiXmlElement("options"))->ToElement();
            tgtnode->SetAttribute("conn_type", (int)rd.connType);
            if (!rd.serialPort.IsEmpty())
            {
                tgtnode->SetAttribute("serial_port", cbU2C(rd.serialPort));
            }
            if (rd.serialBaud != "115200")
            {
                tgtnode->SetAttribute("serial_baud", cbU2C(rd.serialBaud));
            }
            if (!rd.ip.IsEmpty())
            {
                tgtnode->SetAttribute("ip_address", cbU2C(rd.ip));
            }
            if (!rd.ipPort.IsEmpty())
            {
                tgtnode->SetAttribute("ip_port", cbU2C(rd.ipPort));
            }
            if (!rd.additionalCmds.IsEmpty())
            {
                tgtnode->SetAttribute("additional_cmds", cbU2C(rd.additionalCmds));
            }
            if (!rd.additionalCmdsBefore.IsEmpty())
            {
                tgtnode->SetAttribute("additional_cmds_before", cbU2C(rd.additionalCmdsBefore));
            }
            if (rd.skipLDpath)
            {
                tgtnode->SetAttribute("skip_ld_path", "1");
            }
            if (rd.extendedRemote)
            {
                tgtnode->SetAttribute("extended_remote", "1");
            }
            if (!rd.additionalShellCmdsAfter.IsEmpty())
            {
                tgtnode->SetAttribute("additional_shell_cmds_after", cbU2C(rd.additionalShellCmdsAfter));
            }
            if (!rd.additionalShellCmdsBefore.IsEmpty())
            {
                tgtnode->SetAttribute("additional_shell_cmds_before", cbU2C(rd.additionalShellCmdsBefore));
            }
        }
    }
}

void Debugger_GDB_MI::OnProjectOpened(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    if (GetActiveConfigEx().GetFlag(dbg_mi::DebuggerConfiguration::PersistDebugElements))
    {
        LoadStateFromFile(event.GetProject());
    }
}

void Debugger_GDB_MI::OnProjectClosed(CodeBlocksEvent& event)
{
    // allow others to catch this
    event.Skip();

    if (GetActiveConfigEx().GetFlag(dbg_mi::DebuggerConfiguration::PersistDebugElements))
    {
        SaveStateToFile(event.GetProject());
    }

    // the same for remote debugging
    // GetRemoteDebuggingMap(event.GetProject()).clear();
}

bool Debugger_GDB_MI::SaveStateToFile(cbProject* pProject)
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
    tinyxml2::XMLNode* rootnode = doc.InsertEndChild(doc.NewElement(XML_CFG_ROOT_TAG));
    if (!rootnode)
    {
        return false;
    }

    // ********************  Save debugger name ********************
    wxString compilerID = pProject->GetCompilerID();
    int compilerIdx = CompilerFactory::GetCompilerIndex(compilerID );
    Compiler* pCompiler = CompilerFactory::GetCompiler(compilerIdx);
    const CompilerPrograms & pCompilerProgsp = pCompiler->GetPrograms();

    tinyxml2::XMLNode* pCompilerNode = rootnode->InsertEndChild(doc.NewElement("CompilerInfo"));
    dbg_mi::AddChildNode(pCompilerNode, "CompilerName", pCompiler->GetName());
    // dbg_mi::AddChildNode(pCompilerNode, "C_Compiler", pCompilerProgsp.C);
    // dbg_mi::AddChildNode(pCompilerNode, "CPP_Compiler",  pCompilerProgsp.CPP);
    // dbg_mi::AddChildNode(pCompilerNode, "DynamicLinker_LD",  pCompilerProgsp.LD);
    // dbg_mi::AddChildNode(pCompilerNode, "StaticLinker_LIB",  pCompilerProgsp.LIB);
    // dbg_mi::AddChildNode(pCompilerNode, "Make",  pCompilerProgsp.MAKE);
    dbg_mi::AddChildNode(pCompilerNode, "DBGconfig",  pCompilerProgsp.DBGconfig);

    // ******************** Save breakpoints ********************
    tinyxml2::XMLElement* pElementBreakpointList = doc.NewElement("BreakpointsList");
    pElementBreakpointList->SetAttribute("count", m_breakpoints.size());
    tinyxml2::XMLNode* pBreakpointMasterNode = rootnode->InsertEndChild(pElementBreakpointList);

    for (dbg_mi::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        dbg_mi::GDBBreakpoint& bp = **it;
        if (bp.GetProject() == pProject)
        {
            bp.SaveBreakpointToXML(pBreakpointMasterNode);
        }
    }

    // ********************  Save Watches ********************
    tinyxml2::XMLElement* pElementWatchesList = doc.NewElement("WatchesList");
    pElementWatchesList->SetAttribute("count", m_watches.size());
    tinyxml2::XMLNode* pWatchesMasterNode = rootnode->InsertEndChild(pElementWatchesList);

    for (dbg_mi::GDBWatchesContainer::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        dbg_mi::GDBWatch& watch = **it;

        if (watch.GetProject() == pProject)
        {
            watch.SaveWatchToXML(pWatchesMasterNode);
        }
    }

    // ********************  Save Memory Range Watches ********************
    tinyxml2::XMLElement* pElementMemoryRangeList = doc.NewElement("MemoryRangeList");
    pElementMemoryRangeList->SetAttribute("count", m_memoryRanges.size());
    tinyxml2::XMLNode* pMemoryRangeMasterNode = rootnode->InsertEndChild(pElementMemoryRangeList);

    for (dbg_mi::GDBMemoryRangeWatchesContainer::iterator it = m_memoryRanges.begin(); it != m_memoryRanges.end(); ++it)
    {
        dbg_mi::GDBMemoryRangeWatch& memoryRange = **it;

        if (memoryRange.GetProject() == pProject)
        {
            memoryRange.SaveWatchToXML(pMemoryRangeMasterNode);
        }
    }

    // ********************  Save XML to disk ********************
    return doc.SaveFile(cbU2C(fname.GetFullPath()), false);
}

bool Debugger_GDB_MI::LoadStateFromFile(cbProject* pProject)
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
    if(eResult != tinyxml2::XMLError::XML_SUCCESS)
    {
        m_pLogger->LogGDBMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Could not open the file '\%s\" due to the error: %s"),fname.GetFullPath(), doc.ErrorIDToName(eResult) ), dbg_mi::LogPaneLogger::LineType::Error);

        return false;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement(XML_CFG_ROOT_TAG);
    if (!root)
    {
        return false;
    }

    // ******************** Load breakpoints ********************
    tinyxml2::XMLElement* pBreakpointList = root->FirstChildElement("BreakpointsList");
    if (pBreakpointList)
    {
        for(    tinyxml2::XMLElement* pBreakpointElement = pBreakpointList->FirstChildElement("Breakpoint");
                pBreakpointElement;
                pBreakpointElement = pBreakpointElement->NextSiblingElement())
        {
            dbg_mi::GDBBreakpoint * bpNew = new dbg_mi::GDBBreakpoint(pProject, m_pLogger);
            bpNew->LoadBreakpointFromXML(pBreakpointElement, this);

            // Find new breakpoint in the m_breakpoints
            for (dbg_mi::GDBBreakpointsContainer::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
            {
                dbg_mi::GDBBreakpoint& bpSearch = **it;
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
    tinyxml2::XMLElement* pElementWatchesList = root->FirstChildElement("WatchesList");
    if (pElementWatchesList)
    {
        for(    tinyxml2::XMLElement* pWatchElement = pElementWatchesList->FirstChildElement("Watch");
                pWatchElement;
                pWatchElement = pWatchElement->NextSiblingElement())
        {
            wxString GDBWatchClassName = dbg_mi::ReadChildNodewxString(pWatchElement, "GDBWatchClassName");
            if (GDBWatchClassName.IsSameAs("GDBWatch"))
            {
                dbg_mi::GDBWatch* watch = new dbg_mi::GDBWatch(pProject, m_pLogger, "", false);
                watch->LoadWatchFromXML(pWatchElement, this);
            }
        }
    }

    // ******************** Load Memory Range Watches ********************
    tinyxml2::XMLElement* pElementMemoryRangeList = root->FirstChildElement("MemoryRangeList");
    if (pElementMemoryRangeList)
    {
        for(    tinyxml2::XMLElement* pWatchElement = pElementMemoryRangeList->FirstChildElement("MemoryRangeWatch");
                pWatchElement;
                pWatchElement = pWatchElement->NextSiblingElement())
        {
            wxString GDBMemoryRangeWatchName = dbg_mi::ReadChildNodewxString(pWatchElement, "GDBMemoryRangeWatch");
            if (GDBMemoryRangeWatchName.IsSameAs("GDBMemoryRangeWatch"))
            {
                dbg_mi::GDBMemoryRangeWatch* memoryRangeWatch = new dbg_mi::GDBMemoryRangeWatch(pProject, m_pLogger, 0, 0, wxEmptyString );
                memoryRangeWatch->LoadWatchFromXML(pWatchElement, this);
            }
        }
    }

    // ******************** Finished Load ********************
    return true;
}
