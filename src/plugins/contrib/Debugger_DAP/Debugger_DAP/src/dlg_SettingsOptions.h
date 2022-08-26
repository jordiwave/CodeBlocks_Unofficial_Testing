/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef __DEBUGGER_DAP_DLG_SETTINGSOPTIONS_H__
#define __DEBUGGER_DAP_DLG_SETTINGSOPTIONS_H__

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
            ExceptionCatch,
            ExceptionThrow,
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

    private:
        wxString SearchForDebuggerExecutable(wxString pathParam, const wxString &exeNameParam);
        wxString DetectDebuggerExecutable(const wxString &exeNameParam);

};

} // namespace dbg_DAP

#endif // __DEBUGGER_DAP_DLG_SETTINGSOPTIONS_H__
