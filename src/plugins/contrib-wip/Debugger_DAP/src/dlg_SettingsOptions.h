/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef _DEBUGGER_DAP_CMD_DEBUGGEROPTIONSDLG_H_
#define _DEBUGGER_DAP_CMD_DEBUGGEROPTIONSDLG_H_

#include <debuggermanager.h>

class ConfigManagerWrapper;

namespace dbg_DAP
{
class DebuggerConfiguration : public cbDebuggerConfiguration
{
    public:
        explicit DebuggerConfiguration(const ConfigManagerWrapper & config);

        cbDebuggerConfiguration * Clone() const override;
        wxPanel * MakePanel(wxWindow * parent) override;
        bool SaveChanges(wxPanel * panel) override;

    public:
        enum Flags
        {
            WatchFuncLocalsArgs,
            CatchExceptions,
            EvalExpression,
            AddOtherProjectDirs,
            DoNotRun,
            PersistDebugElements,
            StopOnMain,
            RunDAPServer
        };

        bool GetFlag(Flags flag);
        void SetFlag(Flags flag, bool value);
        wxString GetDAPExecutable(bool expandMacro = true);
        wxString GetDAPPortNumber();
        wxString GetDAPPythonHomeEnvSetting();
        wxString GetDisassemblyFlavorCommand();
        wxString GetInitialCommands();

};

} // namespace dbg_DAP

#endif // _DEBUGGER_DAP_CMD_DEBUGGEROPTIONSDLG_H_
