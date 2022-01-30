#include <wx/dir.h>

#include <string>
#include "ClangLocator.h"

#include <wx/arrstr.h>
#include <wx/filefn.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#if defined(__WXMSW__)
#include <wx/msw/registry.h>
#endif // __WXMSW__

#include "globals.h"
#include "manager.h"
#include "logmanager.h"
#include "configmanager.h"

#if defined(_WIN32)
#include "winprocess/asyncprocess/procutils.h"
#else
#include "procutils.h"
#endif


// ----------------------------------------------------------------------------
namespace   //anonymous
// ----------------------------------------------------------------------------
{
wxString fileSep = wxFILE_SEP_PATH;
bool wxFound(int result)
{
    return result != wxNOT_FOUND;
};
}
// ----------------------------------------------------------------------------
ClangLocator::ClangLocator()
// ----------------------------------------------------------------------------
{
    //ctor
}
// ----------------------------------------------------------------------------
ClangLocator::~ClangLocator()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_ClangxFile(wxString findClangxFileName)
// ----------------------------------------------------------------------------
{
    wxString location;
    wxArrayString clangLocations;

    // Try to find clang from the registry
    location = MSWLocate();

    // Try to find clang from the environment path
    if (location.empty())
    {
        wxArrayString envPaths = GetEnvPaths();
        for (wxString path : envPaths)
        {
            wxLogNull nolog; //turn off 'not found' messages
            size_t cnt = ScanForFiles(path, clangLocations, findClangxFileName);
            for (size_t ii=0; ii<cnt; ++ii)
            {
                if (wxFileExists(clangLocations[ii]))
                {
                    location = clangLocations[ii];
                    break;
                }
            }
            if (location.Length())
                break;
        }
    }

    if (location.Length())
    {
        location << wxFILE_SEP_PATH << "bin";
        location.Replace("\\\\", "\\");
        location.Replace("//","/");
        if (not wxFileExists(location + wxFILE_SEP_PATH + findClangxFileName))
            location = wxEmptyString;
    }

    if (location.empty())
    {
        wxString base;
        base = wxStandardPaths::Get().GetExecutablePath();
        base = wxFileName(base).GetPath();
        if (base.Length())
        {
            if (wxFileExists(base + wxFILE_SEP_PATH + findClangxFileName))
            {
                location = base;
            }
        }
    }

    return location;
}

// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_Clang()
// ----------------------------------------------------------------------------
{
    return Locate_ClangxFile(CLANG_FILENAME);
}
// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_ClangDaemon()
// ----------------------------------------------------------------------------
{
    wxString location;
    wxArrayString clangLocations;

    // See if executable dir contains ...lsp/clangd.*
    ConfigManager* pCfgMgr = Manager::Get()->GetConfigManager("app");
    wxString execDir = pCfgMgr->GetExecutableFolder();
    if (wxFileExists(execDir + fileSep + "lsp" + fileSep + CLANG_DAEMON_FILENAME))
        return execDir + fileSep + "lsp";

    location = Locate_ClangxFile(CLANG_DAEMON_FILENAME);

    if (location.Length())
    {
        // clangd must be version 13 or above
        // Version 12 crashes when changing lines at bottom of .h files
        wxString executable = location + fileSep + CLANG_DAEMON_FILENAME;
        if (not IsClangDaemonMajorVersionNumberValid(executable))
        {
            cbMessageBox("clangd version must be 13 or above.", "Error");
            return wxEmptyString;
        }
    }

    return location;
}
// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_ResourceDir(wxFileName fnClangd)
// ----------------------------------------------------------------------------
{
    wxString location;
    wxString dirSep = wxFILE_SEP_PATH;

    // Not a valid path
    if (not fnClangd.Exists())
        return wxEmptyString;

    wxFileName fnClangExecutablePath(fnClangd.GetFullPath(), CLANG_FILENAME);

    // No clang / clang.exe found.
    if (not fnClangExecutablePath.FileExists())
        return wxEmptyString;

    // Get the version of this clang, we need to match it with the same resources dir
    ClangLocator clangLocator;
    wxString clangVersion = clangLocator.GetClangVersion(fnClangExecutablePath.GetFullPath());

    if (fnClangExecutablePath.GetPath().EndsWith(dirSep + "bin"))
    {
        fnClangExecutablePath.RemoveLastDir();
        fnClangExecutablePath.AppendDir("lib");
        fnClangExecutablePath.AppendDir("clang");
        fnClangExecutablePath.AppendDir(clangVersion);
    }
    fnClangExecutablePath.SetName(wxString("clang") << "-" << clangVersion);
    wxString resource = fnClangExecutablePath.GetFullPath(); // **Debugging**
    if (fnClangExecutablePath.DirExists())
        location = fnClangExecutablePath.GetPath();
    else location = wxString();

    return location;
}
// ----------------------------------------------------------------------------
wxString ClangLocator::MSWLocate()
// ----------------------------------------------------------------------------
{
#ifdef __WXMSW__
    wxString llvmInstallPath, llvmVersion;
    wxArrayString regKeys;
    regKeys.Add("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    regKeys.Add("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    for(size_t i = 0; i < regKeys.size(); ++i)
    {
        if(ReadMSWInstallLocation(regKeys.Item(i), llvmInstallPath, llvmVersion))
        {
            //CompilerPtr compiler(new Compiler(NULL));
            wxString clangLoc;
            clangLoc << llvmInstallPath;
            clangLoc.Replace("\\\\", "\\");
            return clangLoc;
        }
    }
#endif
    return wxEmptyString;
}
// ----------------------------------------------------------------------------
wxArrayString ClangLocator::GetEnvPaths() const
// ----------------------------------------------------------------------------
{
    wxString path;
    if(!::wxGetEnv("PATH", &path))
    {
        //-clWARNING() << "Could not read environment variable PATH";
        wxString msg;
        msg << "GetEnvPaths() Could not read environment variable PATH";
        Manager::Get()->GetLogManager()->DebugLog(msg);
        return {};
    }

    wxArrayString mergedPaths;
    wxArrayString paths = ::wxStringTokenize(path, wxPATH_SEP, wxTOKEN_STRTOK);
    return paths;
}
// ----------------------------------------------------------------------------
size_t ClangLocator::ScanForFiles(wxString path, wxArrayString& foundFiles, wxString mask)
// ----------------------------------------------------------------------------
{
#if defined(__WXGTK__)
    // Windows sublayer for unix places the entire windows path into the Linux $PATH environment as mount points
    // like:
    //    /mnt/c/Program Files/WindowsApps/Microsoft.WindowsTerminal_1.11.2921.0_x64__8wekyb3d8bbwe:
    //    /mnt/f/User/Programs/VMWare/bin/:
    //    /mnt/c/usr/bin:
    //    /mnt/c/Program Files (x86)/Intel/iCLS Client/:
    //    /mnt/c/Program Files/Intel/iCLS Client/:
    //    /mnt/c/WINDOWS/system32:
    //        ,,, nmany, many more ...
    //    /mnt/c/Users/Pecan/AppData/Local/Microsoft/WindowsApps:
    //    /mnt/f/user/Programs/LLVM/bin:
    //    /mnt/c/usr/bin:/snap/bin

    // Eliminate WSL windows mount points, else the search takes forever..
    if (path.Matches("/mnt/?/*")) return 0; //eliminate massive number of wsl windows paths //(ph 2021/12/18)
#endif

    if (not wxDirExists(path)) return 0; //(ph 2021/12/18))

    wxString filename = wxFindFirstFile(path + wxFILE_SEP_PATH + mask, wxFILE);
    while(filename.Length())
    {
        foundFiles.Add(path + wxFILE_SEP_PATH + filename);
        filename = wxFindNextFile();
        if (filename.empty()) break;
    }

    return foundFiles.GetCount();
}
// ----------------------------------------------------------------------------
bool ClangLocator::ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion)
// ----------------------------------------------------------------------------
{
#ifdef __WXMSW__
    wxRegKey reg(wxRegKey::HKLM, regkey);
    installPath.Clear();
    llvmVersion.Clear();
    if(reg.Exists())
    {
        reg.QueryValue("DisplayIcon", installPath);
        reg.QueryValue("DisplayVersion", llvmVersion);
    }
    return !installPath.IsEmpty() && !llvmVersion.IsEmpty();
#else
    return false;
#endif
}
// ----------------------------------------------------------------------------
wxString ClangLocator::GetLLVMClangxVersion(const wxString& clangxBinary)
// ----------------------------------------------------------------------------
{
    wxString command;
    wxArrayString stdoutArr;
    command << clangxBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if(not stdoutArr.IsEmpty())
    {
        wxString versionString = stdoutArr.Item(0);
        if (wxFound(versionString.Find("(")) )
        {
            //versionString = versionString.AfterLast('(');
            //versionString = versionString.BeforeLast(')');
            versionString = versionString.BeforeFirst('(');
        }
        return versionString;
    }
    return wxString();
}
// ----------------------------------------------------------------------------
wxString ClangLocator::GetClangVersion(const wxString& clangBinary)
// ----------------------------------------------------------------------------
{
    return GetLLVMClangxVersion(clangBinary);
}
// ----------------------------------------------------------------------------
wxString ClangLocator::GetClangDaemonVersion(const wxString& clangDaemonBinary)
// ----------------------------------------------------------------------------
{
    return GetLLVMClangxVersion(clangDaemonBinary);
}

// ----------------------------------------------------------------------------
bool ClangLocator::IsClangxMajorVersionNumberValid(const wxString& clangxBinary)
// ----------------------------------------------------------------------------
{
    wxString clangdVersion = GetLLVMClangxVersion(clangxBinary);
    clangdVersion = clangdVersion.BeforeFirst('.').AfterLast(' ');
    int versionNum = std::stoi(clangdVersion.ToStdString());
    return (versionNum >= 13);
}
// ----------------------------------------------------------------------------
bool ClangLocator::IsClangMajorVersionNumberValid(const wxString& clangBinary)
// ----------------------------------------------------------------------------
{
    return IsClangxMajorVersionNumberValid(clangBinary);
}

// ----------------------------------------------------------------------------
bool ClangLocator::IsClangDaemonMajorVersionNumberValid(const wxString& clangDaemonBinary)
// ----------------------------------------------------------------------------
{
    return IsClangxMajorVersionNumberValid(clangDaemonBinary);
}
