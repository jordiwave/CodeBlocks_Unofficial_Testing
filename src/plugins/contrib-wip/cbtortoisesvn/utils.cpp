//******************************************************************************
//* Name:      utils.cpp
//* Purpose:   CBTortoiseSVN utilities
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#include "utils.h"

#include <cbeditor.h>
#include <cbproject.h>
#include <cbworkspace.h>
#include <ConfigManager.h>
#include <editormanager.h>
#include <projectmanager.h>

#include <wx/fileconf.h>

//******************************************************************************

#define bzero(a) memset(a,0,sizeof(a)) //easier -- shortcut

//******************************************************************************
using namespace CBTSVN;
//******************************************************************************

CBTSVN::Logger & CBTSVN::Logger::GetInstance()
{
    static CBTSVN::Logger logger;
    return logger;
}

//******************************************************************************

void CBTSVN::Logger::log(const wxString & log)
{
    for (event_subscribers::const_iterator it =
                m_subscribers.begin(); it != m_subscribers.end(); ++it)
    {
        (*it)->OnLogEvent(log);
    }
}

//******************************************************************************

void CBTSVN::Logger::Subscribe(ILogSink & client)
{
    if (std::find(m_subscribers.begin(),
                  m_subscribers.end(), &client) == m_subscribers.end())
    {
        m_subscribers.push_back(&client);
    }
}

//******************************************************************************

void CBTSVN::Logger::Unsubscribe(ILogSink & client)
{
    event_subscribers::iterator it =
        std::find(m_subscribers.begin(), m_subscribers.end(),
                  &client);

    if (it != m_subscribers.end())
    {
        m_subscribers.erase(it);
    }
}

//******************************************************************************
//******************************************************************************
//******************************************************************************

wxString CBTSVN::GetBaseDir(const wxString & filename)
{
    wxFileName base(filename);
    return base.GetPath(wxPATH_GET_VOLUME);
}

//******************************************************************************

void CBTSVN::GetFolders(const wxString & plugin_name)
{
    wxMessageBox(ConfigManager::GetDataFolder()
                 , _("GetDataFolder..."),
                 wxOK | wxICON_INFORMATION);
    wxMessageBox(ConfigManager::GetConfigFolder()
                 , _("GetConfigFolder..."),
                 wxOK | wxICON_INFORMATION);
    wxMessageBox(ConfigManager::GetDataFolder()
                 , _("GetDataFolder..."),
                 wxOK | wxICON_INFORMATION);
    wxMessageBox(GetGlobalIniFile(plugin_name)
                 , _("GetGlobalIniFile..."),
                 wxOK | wxICON_INFORMATION);
}

//******************************************************************************

std::vector<int> CBTSVN::convert(const wxString & s)
{
    std::vector<int> out;
    std::string::size_type i;
    wxString in = s;

    while ((i = in.find_first_of(_(","))) != std::string::npos)
    {
        wxString s = in.substr(0, i);
        long index;
        s.ToLong(&index);
        out.push_back(index);
        in = in.substr(i + 1);
    }

    if (in.size() > 0)
    {
        long index;
        in.ToLong(&index);
        out.push_back(index);
    }

    return out;
}

//******************************************************************************

wxString CBTSVN::convert(const std::vector<int> & vec)
{
    wxString s;

    for (size_t i = 0; i < vec.size(); ++i)
    {
        s += wxString::Format(_("%d"), vec.at(i));

        if (i != (vec.size() - 1))
        {
            s += _(",");
        }
    }

    return s;
}

//******************************************************************************

bool IsWinNT()  //check if we're running NT
{
    OSVERSIONINFO osv;
    osv.dwOSVersionInfoSize = sizeof(osv);
    GetVersionEx(&osv);
    return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

//******************************************************************************

void ErrorMessage(const wxString & str) //display detailed error info
{
    LPWSTR msg;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPWSTR) &msg,
        0,
        NULL);
    wxString message(msg);
    message.Replace(_("\r"), _(""));
    message.Replace(_("\n"), _(""));
    CBTSVN::Logger::GetInstance().log(str + _(" --> ") + message);
    LocalFree(msg);
}

//******************************************************************************

int CBTSVN::Run(const wxString & app, const wxString & dir, const wxString & command, wxString & output)
{
    char buf[1024]; //i/o buffer
    STARTUPINFO si;
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR sd; //security information for pipes
    PROCESS_INFORMATION pi;
    HANDLE newstdin, newstdout, read_stdout, write_stdin; //pipe handles

    if (IsWinNT()) //initialize security descriptor (Windows NT)
    {
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, true, NULL, false);
        sa.lpSecurityDescriptor = &sd;
    }
    else
    {
        sa.lpSecurityDescriptor = NULL;
    }

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;         //allow inheritable handles

    if (!CreatePipe(&newstdin, &write_stdin, &sa, 0)) //create stdin pipe
    {
        ErrorMessage(_("Error: CreatePipe"));
        return -1;
    }

    if (!CreatePipe(&read_stdout, &newstdout, &sa, 0)) //create stdout pipe
    {
        ErrorMessage(_("Error: CreatePipe"));
        CloseHandle(newstdin);
        CloseHandle(write_stdin);
        return -1;
    }

    GetStartupInfo(&si);      //set startupinfo for the spawned process
    /*
    The dwFlags member tells CreateProcess how to make the process.
    STARTF_USESTDHANDLES validates the hStd* members. STARTF_USESHOWWINDOW
    validates the wShowWindow member.
    */
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = newstdout;
    si.hStdError = newstdout;     //set the new handles for the child process
    si.hStdInput = newstdin;

    //spawn the child process
    if (!CreateProcess(
                app.fn_str(),                   // lpApplicationName,
                (WCHAR *)command.fn_str(),      // lpCommandLine,
                NULL,                           // lpProcessAttributes,
                NULL,                           // lpThreadAttributes,
                TRUE,                           // bInheritHandles,
                CREATE_NEW_CONSOLE,             // dwCreationFlags,
                NULL,                           // lpEnvironment,
                dir.fn_str(),                   // lpCurrentDirectory,
                &si,                            // lpStartupInfo,
                &pi))                           // lpProcessInformation
    {
        ErrorMessage(_("Error: CreateProcess (creating \"") + app + _("\")"));
        CloseHandle(newstdin);
        CloseHandle(newstdout);
        CloseHandle(read_stdout);
        CloseHandle(write_stdin);
        return -1;
    }

    unsigned long exit = 0; //process exit code
    unsigned long bread;   //bytes read
    unsigned long avail;   //bytes available
    bzero(buf);

    for (;;)     //main program loop
    {
        DWORD result = WaitForSingleObject(pi.hProcess, 1);
        PeekNamedPipe(read_stdout, buf, 1023, &bread, &avail, NULL);

        //check to see if there is any data to read from stdout
        if (bread != 0)
        {
            bzero(buf);

            if (avail > 1023)
            {
                while (bread >= 1023)
                {
                    ReadFile(read_stdout, buf, 1023, &bread, NULL); //read the stdout pipe

                    for (unsigned int i = 0; i < bread; i++)
                    {
                        output += buf[i];
                    }

                    bzero(buf);
                }
            }
            else
            {
                ReadFile(read_stdout, buf, 1023, &bread, NULL);

                for (unsigned int i = 0; i < bread; i++)
                {
                    output += buf[i];
                }
            }
        }
        else
            if (result == WAIT_OBJECT_0)
            {
                break;
            }
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(newstdin);
    CloseHandle(newstdout);
    CloseHandle(read_stdout);
    CloseHandle(write_stdin);
    return exit;
}

//******************************************************************************

bool CBTSVN::Run(bool blocked, bool hidden, const wxString & command, unsigned long & exit_code)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (hidden)
    {
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }

    exit_code = 0;
    wxWCharBuffer buf = command.wc_str(wxConvLocal);
    WCHAR * COMMAND = buf.data();
    // Start the child process.
    bool result = CreateProcess(NULL, COMMAND, NULL, NULL, FALSE, 0,
                                NULL, NULL, &si, &pi);

    if (blocked)
    {
        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);
        // Get the return value of the child process
        result = result && GetExitCodeProcess(pi.hProcess, &exit_code);
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

//******************************************************************************

bool CBTSVN::GlobalInifileHasEntry(const wxString & plugin_name, const wxString & key)
{
    return IniFileHasEntry(plugin_name, GetGlobalIniFile(plugin_name), key);
}

//******************************************************************************

wxString CBTSVN::GetGlobalIniFile(const wxString & plugin_name)
{
    return ConfigManager::GetConfigFolder() + _("\\") + plugin_name + _(".ini");
}

//******************************************************************************

bool CBTSVN::ReadStringFromGlobalInifile(const wxString & plugin_name, const wxString & key, wxString & value)
{
    return ReadStringFromInifile(plugin_name, GetGlobalIniFile(plugin_name), key, value);
}

//******************************************************************************

bool CBTSVN::WriteStringToGlobalInifile(const wxString & plugin_name, const wxString & key, const wxString & value)
{
    return WriteStringToInifile(plugin_name, GetGlobalIniFile(plugin_name), key, value);
}

//******************************************************************************

wxString CBTSVN::GetWorkspaceFilename()
{
    cbWorkspace * workspace = Manager::Get()->GetProjectManager()->GetWorkspace();

    if (!workspace)
    {
        return _("");
    }

    return workspace->GetFilename();
}

//******************************************************************************

wxString  CBTSVN::GetEditorFilename()
{
    EditorManager * man = Manager::Get()->GetEditorManager();

    if (!man)
    {
        return _("");
    }

    // prevent invocation on "start here"
    cbEditor * builtin_active_editor = man->GetBuiltinActiveEditor();

    if (!builtin_active_editor)
    {
        return _("");
    }

    EditorBase * active_editor = man->GetActiveEditor();

    if (!active_editor)
    {
        return _("");
    }

    return active_editor->GetFilename();
}

//******************************************************************************

wxString CBTSVN::GetProjectFilename()
{
    cbProject * project = Manager::Get()->GetProjectManager()->GetActiveProject();

    if (!project)
    {
        return _("");
    }

    return project->GetFilename();
}

//******************************************************************************

wxString CBTSVN::GetProjectIniFile(const wxString & plugin_name)
{
    wxString filename = GetProjectFilename();

    if (filename == _(""))
    {
        return _("");
    }

    return GetBaseDir(filename) + _("\\") + plugin_name + _(".ini");
}

//******************************************************************************

wxString CBTSVN::GetWorkspaceIniFile(const wxString & plugin_name)
{
    wxString filename = GetWorkspaceFilename();

    if (filename == _(""))
    {
        return _("");
    }

    return GetBaseDir(filename) + _("\\") + plugin_name + _(".ini");
}

//******************************************************************************

bool CBTSVN::IniFileHasEntry(const wxString & plugin_name, const wxString & inifile, const wxString & key)
{
    wxFileConfig configFile(plugin_name, wxT("Code::Blocks"), inifile);
    return configFile.HasEntry(key);
}

//******************************************************************************

bool CBTSVN::ReadStringFromInifile(const wxString & plugin_name, const wxString & inifile, const wxString & key, wxString & value)
{
    wxFileConfig configFile(plugin_name, wxT("Code::Blocks"), inifile);
    return configFile.Read(key, &value);
}

//******************************************************************************

bool CBTSVN::WriteStringToInifile(const wxString & plugin_name, const wxString & inifile, const wxString & key, const wxString & value)
{
    wxFileConfig configFile(plugin_name, wxT("Code::Blocks"), inifile);
    return configFile.Write(key, value);
}

//******************************************************************************

bool CBTSVN::ReadStringFromProjectInifile(const wxString & plugin_name, const wxString & key, wxString & value)
{
    return ReadStringFromInifile(plugin_name, GetProjectIniFile(plugin_name), key, value);
}

//******************************************************************************

bool CBTSVN::WriteStringToProjectInifile(const wxString & plugin_name, const wxString & key, const wxString & value)
{
    return WriteStringToInifile(plugin_name, GetProjectIniFile(plugin_name), key, value);
}

//******************************************************************************

bool CBTSVN::ReadStringFromWorkspaceInifile(const wxString & plugin_name, const wxString & key, wxString & value)
{
    return ReadStringFromInifile(plugin_name, GetWorkspaceIniFile(plugin_name), key, value);
}

//******************************************************************************

bool CBTSVN::WriteStringToWorkspaceInifile(const wxString & plugin_name, const wxString & key, const wxString & value)
{
    return WriteStringToInifile(plugin_name, GetWorkspaceIniFile(plugin_name), key, value);
}

//******************************************************************************

wxString CBTSVN::GetCustomDir(const wxString & plugin_name, const wxString & filename, const wxString & key_custom_location, const wxString & key_custom_relative)
{
    wxString dir, rel;
    ReadStringFromProjectInifile(plugin_name, key_custom_location, dir);
    ReadStringFromProjectInifile(plugin_name, key_custom_relative, rel);
    bool IsRelative = (rel == _("1") || rel == _(""));

    if (!IsRelative)
    {
        return dir;
    }

    wxString basedir = GetBaseDir(filename);
    wxFileName directory;
    directory.AssignDir(dir);
    directory.MakeAbsolute(basedir);
    return directory.GetFullPath();
}

//******************************************************************************
// End of file
