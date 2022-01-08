//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : worker_thread.cpp
//
// -------------------------------------------------------------------------
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
////////////////////////////////////////////////////////////////////////////// // Curated for CodeBlocks by pecan 2020/09/28
// Curated for CodeBlocks by pecan 2020/09/28

class wxEvtHandler;
class IProcess;
#include <wx/string.h>
//-#include "macros.h"
#include "asyncprocess.h"       //(ph 2020/09/26)

#ifdef __WXMSW__
#include "winprocess_impl.h"
#else
#include "unixprocess_impl.h"
#endif

IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir,
                             const clEnvList_t* env)
{
    clEnvironment e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcessCB(wxEvtHandler* parent, IProcessCallback* cb, const wxString& cmd, size_t flags,
                               const wxString& workingDir, const clEnvList_t* env)
{
    clEnvironment e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir, const clEnvList_t* env)
{
    clEnvironment e(env);
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(NULL, cmd, errMsg, flags | IProcessCreateSync, workingDir);
#else
    return UnixProcessImpl::Execute(NULL, cmd, flags | IProcessCreateSync, workingDir);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int& exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);

    exitCode = 0;
    return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);
}

void IProcess::WaitForTerminate(wxString& output)
{
    if(IsRedirect()) {
        wxString buff;
        wxString buffErr;
        while(Read(buff, buffErr)) {
            output << buff;
            if(!buff.IsEmpty() && !buffErr.IsEmpty()) { output << "\n"; }
            output << buffErr;
        }
    } else {
        // Just wait for the process to terminate in a busy loop
        while(IsAlive()) {
            wxThread::Sleep(10);
        }
    }
}
