/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/

#ifndef __DEBUGGER_STATE_H__
#define __DEBUGGER_STATE_H__

class Debugger_State
{
    public :
        enum eDAPState
        {
            NotConnected = 0,
            Connected,
            Stopped,
            Running
        };


        static bool IsRunning();
        static bool IsStopped();
        static bool IsBusy();
        static void SetState(Debugger_State::eDAPState newState);

    private :
        static eDAPState  DAPDebuggerState;
};

#endif // __DEBUGGER_STATE_H__
