/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/
#define DAP_DEBUG_ENABLE 1

// CB include files (not DAP)
#include "cbdebugger_interfaces.h"
#include "cbplugin.h"
#include "cbproject.h"
#include "compilerfactory.h"

// DAP include files
#include "DAP_CallStack.h"
#include "dlg_SettingsOptions.h"
#include "debugger_logger.h"
#include "DAP_Debugger_State.h"

// constructor
DBG_DAP_CallStack::DBG_DAP_CallStack(cbDebuggerPlugin * plugin, dbg_DAP::LogPaneLogger * logger) :
    m_plugin(plugin),
    m_pLogger(logger)
{
}

// destructor
DBG_DAP_CallStack::~DBG_DAP_CallStack()
{
}

void DBG_DAP_CallStack::OnAttachReal()
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format("%s %d", __PRETTY_FUNCTION__, __LINE__));
}

void DBG_DAP_CallStack::OnReleaseReal(bool appShutDown)
{
    // Do not log anything as we are closing
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_All);
}

dbg_DAP::DebuggerConfiguration & DBG_DAP_CallStack::GetActiveConfigEx()
{
    return static_cast<dbg_DAP::DebuggerConfiguration &>(m_plugin->GetActiveConfig());
}

void DBG_DAP_CallStack::UpdateDebugDialogs(bool bClearAllData)
{
    cbBacktraceDlg * pDialogBacktrace = Manager::Get()->GetDebuggerManager()->GetBacktraceDialog();

    if (pDialogBacktrace)
    {
        pDialogBacktrace->Reload();
    }

    cbThreadsDlg * pDialogThreads = Manager::Get()->GetDebuggerManager()->GetThreadsDialog();

    if (pDialogThreads)
    {
        pDialogThreads->Reload();
    }
}

// "===================================================================================="
// " ____    ____     ___        _   _____    ____   _____       ___      __  _____     "
// " |  _ \  |  _ \   / _ \      | | | ____|  / ___| |_   _|     |_ _|    / / |  ___|   "
// " | |_) | | |_) | | | | |  _  | | |  _|   | |       | |        | |    / /  | |_      "
// " |  __/  |  _ <  | |_| | | |_| | | |___  | |___    | |        | |   / /   |  _|     "
// " |_|     |_| \_\  \___/   \___/  |_____|  \____|   |_|       |___| /_/    |_|       "
// "                                                                                    "
// "===================================================================================="

void DBG_DAP_CallStack::OnProjectOpened(CodeBlocksEvent & event)
{
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_All);
}

void DBG_DAP_CallStack::CleanupWhenProjectClosed(cbProject * project)
{
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_All);
}

// "===================================================================================================="
// "     ____    _____      _       ____   _  __       __    _____   ____       _      __  __   _____   "
// "    / ___|  |_   _|    / \     / ___| | |/ /      / /   |  ___| |  _ \     / \    |  \/  | | ____|  "
// "    \___ \    | |     / _ \   | |     | ' /      / /    | |_    | |_) |   / _ \   | |\/| | |  _|    "
// "     ___) |   | |    / ___ \  | |___  | . \     / /     |  _|   |  _ <   / ___ \  | |  | | | |___   "
// "    |____/    |_|   /_/   \_\  \____| |_|\_\   /_/      |_|     |_| \_\ /_/   \_\ |_|  |_| |_____|  "
// "                                                                                                    "
// "===================================================================================================="

int DBG_DAP_CallStack::GetStackFrameCount() const
{
    return m_backtrace.size();
}

cb::shared_ptr<const cbStackFrame> DBG_DAP_CallStack::GetStackFrame(int index) const
{
    return m_backtrace[index];
}

void DBG_DAP_CallStack::SwitchToFrame(int number)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SwitchToFrame: %d"), number), dbg_DAP::LogPaneLogger::LineType::Debug);

    if (Debugger_State::IsRunning() && Debugger_State::IsStopped() && (number < static_cast<int>(m_backtrace.size())))
    {
        cb::shared_ptr<const cbStackFrame> frameToSwitch = m_backtrace[number];
        wxString sFileName = frameToSwitch->GetFilename();
        long int lineNumber;;
        frameToSwitch->GetLine().ToLong(&lineNumber);
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SyncEditor: %s %ld"), sFileName, lineNumber), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        m_plugin->SyncEditor(sFileName, lineNumber, true);
    }
}

int DBG_DAP_CallStack::GetActiveStackFrame() const
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

int DBG_DAP_CallStack::GetThreadsCount() const
{
    return m_threads.size();
}

cb::shared_ptr<const cbThread> DBG_DAP_CallStack::GetThread(int index) const
{
    return m_threads[index];
}

bool DBG_DAP_CallStack::SwitchToThread(int thread_number)
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

// "================================================================================================"
// "     __  __   ___   ____     ____                                                               "
// "    |  \/  | |_ _| / ___|   / ___|                                                              "
// "    | |\/| |  | |  \___ \  | |                                                                  "
// "    | |  | |  | |   ___) | | |___                                                               "
// "    |_|  |_| |___| |____/   \____|                                                              "
// "                                                                                                "
// "================================================================================================"

void DBG_DAP_CallStack::DAPDebuggerResetData(dbg_DAP::ResetDataType bClearAllData)
{
    m_backtrace.clear();
    m_current_frame.Reset();
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

/// Received a response to `GetFrames()` call
void DBG_DAP_CallStack::OnStackTrace(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::StackTraceResponse * stack_trace_data = event.GetDapResponse()->As<dap::StackTraceResponse>();

    if (stack_trace_data)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received stack trace event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        int stackID = 0;
        m_backtrace.clear();

        for (const auto & stack : stack_trace_data->stackFrames)
        {
#ifdef DAP_DEBUG_ENABLE
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

// "==================================================================================================================="
// "          ____       _      ____      _____  __     __  _____   _   _   _____   ____      _____   _   _   ____     "
// "         |  _ \     / \    |  _ \    | ____| \ \   / / | ____| | \ | | |_   _| / ___|    | ____| | \ | | |  _ \    "
// "         | | | |   / _ \   | |_) |   |  _|    \ \ / /  |  _|   |  \| |   | |   \___ \    |  _|   |  \| | | | | |   "
// "         | |_| |  / ___ \  |  __/    | |___    \ V /   | |___  | |\  |   | |    ___) |   | |___  | |\  | | |_| |   "
// "         |____/  /_/   \_\ |_|       |_____|    \_/    |_____| |_| \_|   |_|   |____/    |_____| |_| \_| |____/    "
// "                                                                                                                   "
// "==================================================================================================================="
