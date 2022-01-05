
#include <string>
#include "ClangLocator.h"

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/filefn.h>
#include <wx/utils.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/msw/registry.h>

#include "globals.h"
#include "manager.h"
#include "logmanager.h"
#include "configmanager.h"
#include "asyncprocess\procutils.h"

// ----------------------------------------------------------------------------
namespace   //anonymous
// ----------------------------------------------------------------------------
{
    wxString fileSep = wxFILE_SEP_PATH;
    bool wxFound(int result){return result != wxNOT_FOUND;};
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
wxString ClangLocator::Locate_Clang()
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
            size_t cnt = ScanForFiles(path, clangLocations, "clang.exe");
            for (size_t ii=0; ii<cnt; ++ii)
            {
                if (wxFileExists(clangLocations[ii]))
                {
                    location = clangLocations[ii];
                    break;
                }
            }
            if (location.Length()) break;
        }
    }
    wxString fileSep = wxFILE_SEP_PATH;
    if (location.Length())
    {
        location << fileSep << "bin";
        location.Replace("\\\\", "\\");
        #ifdef __WXMSW__
        if (not wxFileExists(location + fileSep + "clang.exe"))
            location = wxEmptyString;
        #else
        if (not wxFileExists(location + fileSep + "clang"))
            location = wxEmptyString;
        #endif
    }

    return location;
}
// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_ResourceDir(wxString clangDir)
// ----------------------------------------------------------------------------
{
    wxString location;
    wxString clangLocation = clangDir;
    wxString dirSep = wxFILE_SEP_PATH;

    if (clangDir.empty())
        clangLocation = Locate_Clang();

    if (clangLocation.empty())
        return wxEmptyString;
    if (clangLocation.EndsWith(dirSep + "bin"))
        clangLocation.Replace(dirSep + "bin", "");
    clangLocation += fileSep + "lib\\clang";
    wxString dirname = wxFindFirstFile(clangLocation + dirSep + "*", wxDIR);
    if (dirname.Length() )
        location = dirname;
    else location = wxEmptyString;

    return location;
}
// ----------------------------------------------------------------------------
wxString ClangLocator::Locate_Clangd()
// ----------------------------------------------------------------------------
{
    wxString location;
    wxArrayString clangLocations;

    // See if executable dir contains ...lsp/clangd.*
    ConfigManager* pCfgMgr = Manager::Get()->GetConfigManager("app");
    wxString execDir = pCfgMgr->GetExecutableFolder();
    if (wxFileExists(execDir + fileSep + "lsp" + fileSep + "clangd.exe"))
        return execDir + fileSep + "lsp";

    // Try to find clang from the registry
    location = MSWLocate();

    // Try to find clang from the environment path
    if (location.empty())
    {
        wxArrayString envPaths = GetEnvPaths();
        for (wxString path : envPaths)
        {
            size_t cnt = ScanForFiles(path, clangLocations, "clang.exe");
            for (size_t ii=0; ii<cnt; ++ii)
            {
                if (wxFileExists(clangLocations[ii]))
                {
                    location = clangLocations[ii];
                    break;
                }
            }
            if (location.Length()) break;
        }
    }
    if (location.Length())
    {
        location << fileSep << "bin";
        location.Replace("\\\\","\\");
        location.Replace("//","/");
        if (not wxFileExists(location + fileSep + "clangd.exe"))
            location = wxEmptyString;
    }
    // clangd must be version 13 or above
    // Version 12 crashes when changing lines at bottom of .h files
    wxString executable = location + fileSep + "clangd.exe";
    wxString version = GetClangVersion(executable);
    //eg., clangd version 10.0,0
    version = version.BeforeFirst('.').AfterLast(' ');
    int versionNum = std::stoi(version.ToStdString());
    if (versionNum < 13)
    {
        cbMessageBox("clangd version must be 13 or above.", "Error");
        return wxEmptyString;
    }
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
    for(size_t i = 0; i < regKeys.size(); ++i) {
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
        wxString msg; msg << "GetEnvPaths() Could not read environment variable PATH";
        Manager::Get()->GetLogManager()->DebugLog(msg);
        return {};
    }

    wxArrayString mergedPaths;
    wxArrayString paths = ::wxStringTokenize(path, ENV_PATH_SEPARATOR, wxTOKEN_STRTOK);
    return paths;
}
// ----------------------------------------------------------------------------
size_t ClangLocator::ScanForFiles(wxString path, wxArrayString& foundFiles, wxString mask)
// ----------------------------------------------------------------------------
{
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
    if(reg.Exists()) {
        reg.QueryValue("DisplayIcon", installPath);
        reg.QueryValue("DisplayVersion", llvmVersion);
    }
    return !installPath.IsEmpty() && !llvmVersion.IsEmpty();
#else
    return false;
#endif
}
// ----------------------------------------------------------------------------
wxString ClangLocator::GetClangVersion(const wxString& clangBinary)
// ----------------------------------------------------------------------------
{
    wxString command;
    wxArrayString stdoutArr;
    command << clangBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if(not stdoutArr.IsEmpty()) {
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
