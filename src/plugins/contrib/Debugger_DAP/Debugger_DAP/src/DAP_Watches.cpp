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
#include "DAP_Watches.h"
#include "dlg_SettingsOptions.h"
#include "debugger_logger.h"
#include "DAP_Debugger_State.h"
#include "dlg_WatchEdit.h"

//XML file root tag for data
static const char * XML_CFG_ROOT_TAG = "Debugger_layout_file";

// constructor
DBG_DAP_Watches::DBG_DAP_Watches(cbDebuggerPlugin * plugin, dbg_DAP::LogPaneLogger * logger, dap::Client * pDAPClient) :
    m_plugin(plugin),
    m_pProject(nullptr),
    m_pLogger(logger),
    m_pDAPClient(pDAPClient)
{
}

// destructor
DBG_DAP_Watches::~DBG_DAP_Watches()
{
}

void DBG_DAP_Watches::OnAttachReal()
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format("%s %d", __PRETTY_FUNCTION__, __LINE__));
}

void DBG_DAP_Watches::OnReleaseReal(bool appShutDown)
{
    // Do not log anything as we are closing
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_All);
}

dbg_DAP::DebuggerConfiguration & DBG_DAP_Watches::GetActiveConfigEx()
{
    return static_cast<dbg_DAP::DebuggerConfiguration &>(m_plugin->GetActiveConfig());
}

void DBG_DAP_Watches::UpdateDebugDialogs(bool bClearAllData)
{
    cbWatchesDlg * pDialogWatches = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();

    if (pDialogWatches)
    {
        pDialogWatches->RefreshUI();
    }
}
void DBG_DAP_Watches::SetProject(cbProject * pProject)
{
    m_pProject = pProject;
}

// "===================================================================================="
// " ____    ____     ___        _   _____    ____   _____       ___      __  _____     "
// " |  _ \  |  _ \   / _ \      | | | ____|  / ___| |_   _|     |_ _|    / / |  ___|   "
// " | |_) | | |_) | | | | |  _  | | |  _|   | |       | |        | |    / /  | |_      "
// " |  __/  |  _ <  | |_| | | |_| | | |___  | |___    | |        | |   / /   |  _|     "
// " |_|     |_| \_\  \___/   \___/  |_____|  \____|   |_|       |___| /_/    |_|       "
// "                                                                                    "
// "===================================================================================="

void DBG_DAP_Watches::OnProjectOpened(CodeBlocksEvent & event)
{
    // allow others to catch this
    event.Skip();

    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        LoadStateFromFile(event.GetProject());
    }
}

void DBG_DAP_Watches::CleanupWhenProjectClosed(cbProject * project)
{
    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        SaveStateToFile(project);
    }

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
}

// "=============================================================================================="
// "    __        __          _            _                                                      "
// "    \ \      / /   __ _  | |_    ___  | |__     ___   ___                                     "
// "     \ \ /\ / /   / _` | | __|  / __| | '_ \   / _ \ / __|                                    "
// "      \ V  V /   | (_| | | |_  | (__  | | | | |  __/ \__ \                                    "
// "       \_/\_/     \__,_|  \__|  \___| |_| |_|  \___| |___/                                    "
// "                                                                                              "
// "=============================================================================================="

void DBG_DAP_Watches::CreateStartWatches()
{
    if (m_watches.empty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("No watches"), dbg_DAP::LogPaneLogger::LineType::Debug);
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

void DBG_DAP_Watches::UpdateDAPWatches(int updateType)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("updating watches"), dbg_DAP::LogPaneLogger::LineType::Debug);
    CodeBlocksEvent event(cbEVT_DEBUGGER_UPDATED);
    event.SetInt(updateType);
    Manager::Get()->ProcessEvent(event);
}

cb::shared_ptr<cbWatch> DBG_DAP_Watches::AddWatch(const wxString & symbol, cb_unused bool update)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", symbol), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::DAPWatch> watch(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, symbol, false));

    for (const dap::Variable & var : m_stackdapvariables)
    {
        if (symbol.IsSameAs(var.name))
        {
#ifdef DAP_DEBUG_ENABLE
            wxString value = var.value.empty() ? "\"\"" : var.value;
            wxString attributes = wxEmptyString;

            for (const auto & attrib : var.presentationHint.attributes)
            {
                attributes += " " + attrib;
            }

            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("Var: %s = %s , variablesReference: %d  Type: %s, Hint: kind: %s , attributes %s , visibility: %s "),
                                                      var.name,
                                                      value,
                                                      var.variablesReference,
                                                      var.type,
                                                      var.presentationHint.kind,
                                                      attributes,
                                                      var.presentationHint.visibility
                                                     ),
                                     dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
            watch->SetValue(var.value);
            watch->SetDAPVariableReference(var.variablesReference);
            watch->SetType(var.type);

            if (var.variablesReference > 0)
            {
                watch->SetHasBeenExpanded(false);
                watch->SetRangeArray(0, var.variablesReference);
                cbWatch::AddChild(watch, cb::shared_ptr<cbWatch>(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, "updating...", false)));
            }

            break;
        }
    }

    m_watches.push_back(watch);
    return watch;
}

cb::shared_ptr<cbWatch> DBG_DAP_Watches::AddWatch(dbg_DAP::DAPWatch * watch, cb_unused bool update)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("Add watch for \"%s\"", watch->GetSymbol()), dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<dbg_DAP::DAPWatch> w(watch);
    m_watches.push_back(w);

    if (Debugger_State::IsRunning())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Need to wire up watch.", dbg_DAP::LogPaneLogger::LineType::Error);
    }

    return w;
}

void DBG_DAP_Watches::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Debug);
    cb::shared_ptr<cbWatch> root_watch = cbGetRootWatch(watch);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), root_watch);

    if (it == m_watches.end())
    {
        return;
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

bool DBG_DAP_Watches::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    if (watch == m_WatchLocalsandArgs)
    {
        return true;
    }

    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);
    return it != m_watches.end();
}

void DBG_DAP_Watches::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
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

bool DBG_DAP_Watches::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (!Debugger_State::IsStopped() || !Debugger_State::IsRunning())
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

void DBG_DAP_Watches::ExpandWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Debug);

    if (!Debugger_State::IsStopped() || !Debugger_State::IsRunning())
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
            real_watch->RemoveChildren();
            real_watch->SetDAPChildVariableRequestSequence(m_pDAPClient->GetChildrenVariables(real_watch->GetDAPVariableReference(), dap::EvaluateContext::VARIABLES, 0));
        }
    }
}

void DBG_DAP_Watches::CollapseWatch(cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not completed yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (!Debugger_State::IsStopped() || !Debugger_State::IsRunning())
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
            // m_actions.Add(new dbg_DAP::DAPWatchCollapseAction(*it, real_watch, m_watches, m_pLogger));
        }
    }
}

void DBG_DAP_Watches::UpdateWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    dbg_DAP::DAPWatchesContainer::iterator it = std::find(m_watches.begin(), m_watches.end(), watch);

    if (it == m_watches.end())
    {
        return;
    }

    if (Debugger_State::IsRunning())
    {
        //        m_actions.Add(new dbg_DAP::DAPWatchCreateAction(*it, m_watches, m_pLogger, false));
    }
}

void DBG_DAP_Watches::DoWatches()
{
    if (!Debugger_State::IsRunning())
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

cb::shared_ptr<cbWatch> DBG_DAP_Watches::AddMemoryRange(uint64_t llAddress, uint64_t llSize, const wxString & symbol, bool update)
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


bool DBG_DAP_Watches::IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch)
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

// "================================================================================================"
// "     __  __   ___   ____     ____                                                               "
// "    |  \/  | |_ _| / ___|   / ___|                                                              "
// "    | |\/| |  | |  \___ \  | |                                                                  "
// "    | |  | |  | |   ___) | | |___                                                               "
// "    |_|  |_| |___| |____/   \____|                                                              "
// "                                                                                                "
// "================================================================================================"

void DBG_DAP_Watches::DAPDebuggerResetData(dbg_DAP::ResetDataType bClearAllData)
{
}

// "================================================================================================"
// "         ____       _     __     __  _____     ____    _____      _      _____   _____          "
// "        / ___|     / \    \ \   / / | ____|   / ___|  |_   _|    / \    |_   _| | ____|         "
// "        \___ \    / _ \    \ \ / /  |  _|     \___ \    | |     / _ \     | |   |  _|           "
// "         ___) |  / ___ \    \ V /   | |___     ___) |   | |    / ___ \    | |   | |___          "
// "        |____/  /_/   \_\    \_/    |_____|   |____/    |_|   /_/   \_\   |_|   |_____|         "
// "                                                                                                "
// "================================================================================================"

bool DBG_DAP_Watches::SaveStateToFile(cbProject * pProject)
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

bool DBG_DAP_Watches::LoadStateFromFile(cbProject * pProject)
{
    wxString projectFilename = pProject->GetFilename();

    if (projectFilename.IsEmpty())
    {
        return false;
    }

    wxFileName fname(projectFilename);
    fname.SetExt("bps");

    if (!fname.FileExists())
    {
        return false;
    }

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
                watch->LoadWatchFromXML(pWatchElement);

                if (!watch->GetSymbol().IsEmpty())
                {
                    // See debuggermenu.cpp DebuggerMenuHandler::OnAddWatch(...) function
                    cb::shared_ptr<cbWatch> watchAdded = AddWatch(watch, true);
                    cbWatchesDlg * dialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
                    dialog->AddWatch(watchAdded);   // This call adds the watch to the debugger and GUI
                }
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

/// DAP server responded to our `initialize` request
void DBG_DAP_Watches::OnInitializedEvent(DAPEvent & event)
{
    // got initialized event, place breakpoints and continue
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::InitializeResponse * response_data = event.GetDapResponse()->As<dap::InitializeResponse>();

    if (response_data)
    {
        if (response_data->success)
        {
            // Setup initial data watches
            CreateStartWatches();
        }
    }
}

/// Received a response to `GetFrames()` call
void DBG_DAP_Watches::OnScopes(DAPEvent & event)
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
                m_pDAPClient->GetChildrenVariables(scope.variablesReference, dap::EvaluateContext::VARIABLES, 0);
            }
        }
    }
}

bool DBG_DAP_Watches::AddWatchChildByReqestSequence(cb::shared_ptr<dbg_DAP::DAPWatch> pWatch, int requestSeq, const dap::Variable & var)
{
    if (pWatch->GetDAPChildVariableRequestSequence() == requestSeq)
    {
#ifdef DAP_DEBUG_ENABLE
        wxString attributes = wxEmptyString;

        for (const auto & attrib : var.presentationHint.attributes)
        {
            attributes += " " + attrib;
        }

        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("Child found for request #%d   Var: %s  (%d) = %s , Type: %s, Hint: kind: %s , attributes %s , visibility: %s "),
                                                  requestSeq,
                                                  var.name,
                                                  var.variablesReference,
                                                  var.value,
                                                  var.type,
                                                  var.presentationHint.kind,
                                                  attributes,
                                                  var.presentationHint.visibility
                                                 ),
                                 dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
        cb::shared_ptr<dbg_DAP::DAPWatch> childWatch(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, var.name, false));
        childWatch->SetValue(var.value);
        childWatch->SetDAPVariableReference(var.variablesReference);
        childWatch->SetType(var.type);

        if (var.variablesReference > 0)
        {
            childWatch->SetHasBeenExpanded(false);
            childWatch->SetRangeArray(0, var.variablesReference);
            cbWatch::AddChild(childWatch, cb::shared_ptr<cbWatch>(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, "updating...", false)));
        }

        cbWatch::AddChild(pWatch, childWatch);
        return true;
    }
    else
    {
        if (pWatch->GetChildCount() > 0)
        {
            int childcount = pWatch->GetChildCount();

            for (int child = 0; child < childcount; ++child)
            {
                cb::shared_ptr<dbg_DAP::DAPWatch> cWatch = cb::static_pointer_cast<dbg_DAP::DAPWatch>(pWatch->GetChild(child));

                if (AddWatchChildByReqestSequence(cWatch, requestSeq, var))
                {
                    return true;
                }
            }
        }

        return false;
    }
}

void DBG_DAP_Watches::OnVariables(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::VariablesResponse * resp = event.GetDapResponse()->As<dap::VariablesResponse>();

    if (resp)
    {
        for (const dap::Variable & var : resp->variables)
        {
            m_stackdapvariables.push_back(var);
#ifdef DAP_DEBUG_ENABLE
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
                else
                {
                    AddWatchChildByReqestSequence(*it, resp->request_seq, var);
                }
            }
        }

        UpdateDAPWatches(int(cbDebuggerPlugin::DebugWindows::Watches));
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
