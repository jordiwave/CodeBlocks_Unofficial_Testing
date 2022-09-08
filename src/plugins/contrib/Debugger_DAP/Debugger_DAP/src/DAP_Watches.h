/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef __DAP_WATCHES_H__
#define __DAP_WATCHES_H__

// System and library includes
#include <memory>
#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/wxprec.h>

// CB includes
#include <cbplugin.h>
#include <tinyxml2.h>

// DAP debugger includes
#include "debugger_logger.h"
#include "definitions.h"

// DAP protocol includes
#include "Client.hpp"
#include "Process.hpp"
#include "dlg_SettingsOptions.h"

class TextCtrlLogger;
class Compiler;

class DBG_DAP_Watches
{
    public:
        /** Constructor. */
        DBG_DAP_Watches(cbDebuggerPlugin * plugin, dbg_DAP::LogPaneLogger * logger, dap::Client * pDAPClient);
        /** Destructor. */
        ~DBG_DAP_Watches();

        dbg_DAP::DebuggerConfiguration & GetActiveConfigEx();

        // watches
        void CreateStartWatches();
        void UpdateDAPWatches(int updateType);
        cb::shared_ptr<cbWatch> AddWatch(const wxString & symbol, bool update);
        cb::shared_ptr<cbWatch> AddWatch(dbg_DAP::DAPWatch * watch, cb_unused bool update);
        cb::shared_ptr<cbWatch> AddMemoryRange(uint64_t address, uint64_t size, const wxString & symbol, bool update);
        void DeleteWatch(cb::shared_ptr<cbWatch> watch);
        bool HasWatch(cb::shared_ptr<cbWatch> watch);
        bool IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> & watch);
        void ShowWatchProperties(cb::shared_ptr<cbWatch> watch);
        bool SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value);
        void ExpandWatch(cb::shared_ptr<cbWatch> watch);
        void CollapseWatch(cb::shared_ptr<cbWatch> watch);
        void UpdateWatch(cb::shared_ptr<cbWatch> watch);
        void DoWatches();

        /** Any descendent plugin should override this method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        void OnAttachReal();

        /** Any descendent plugin should override this method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        void OnReleaseReal(bool appShutDown);

        void UpdateDebugDialogs(bool bClearAllData);
        void OnProjectOpened(CodeBlocksEvent & event);
        void CleanupWhenProjectClosed(cbProject * project);

        bool SaveStateToFile(cbProject * prj);
        bool LoadStateFromFile(cbProject * prj);

        /// Dap events
        void OnScopes(DAPEvent & event);
        void OnVariables(DAPEvent & event);
        void OnInitializedEvent(DAPEvent & event);
        void SetProject(cbProject * m_pProject);

    private:
        cbDebuggerPlugin * m_plugin;
        cbProject * m_pProject;
        dbg_DAP::LogPaneLogger * m_pLogger;
        dap::Client * m_pDAPClient;

        // misc
        void DAPDebuggerResetData(dbg_DAP::ResetDataType bClearAllData);
        void OnProcessBreakpointData(const wxString & brkDescription);
        bool AddWatchChildByReqestSequence(cb::shared_ptr<dbg_DAP::DAPWatch> Watch, int requestSeq, const dap::Variable & var);

        // Watches
        dbg_DAP::DAPWatchesContainer m_watches;
        dbg_DAP::DAPMapWatchesToType m_mapWatchesToType;
        cb::shared_ptr<dbg_DAP::DAPWatch> m_WatchLocalsandArgs;
        std::vector<dap::Variable> m_stackdapvariables;

};

#endif // __DAP_WATCHES_H__
