#include "DAP_Debugger_State.h"


Debugger_State::eDAPState  Debugger_State::DAPDebuggerState = Debugger_State::eDAPState::NotConnected;

bool Debugger_State::IsRunning()
{
    return (Debugger_State::DAPDebuggerState != Debugger_State::eDAPState::NotConnected);
}

bool Debugger_State::IsStopped()
{
    return (Debugger_State::DAPDebuggerState == Debugger_State::eDAPState::Stopped);
}

bool Debugger_State::IsBusy()
{
    return (Debugger_State::DAPDebuggerState != Debugger_State::eDAPState::Stopped);
}


void Debugger_State::SetState(Debugger_State::eDAPState newState)
{
    Debugger_State::DAPDebuggerState = newState;
}
