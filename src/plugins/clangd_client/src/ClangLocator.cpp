#include <string>

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#if defined(__WXMSW__)
    #include <wx/msw/registry.h>
#endif // __WXMSW__

#include "ClangLocator.h"

#include "globals.h"
#include "manager.h"
#include "logmanager.h"
#include <cbproject.h>
#include "configmanager.h"
#include "compiler.h"
#include "compilerfactory.h"
#include "projectmanager.h"

#if defined(_WIN32)
    #include "winprocess/asyncprocess/procutils.h"
#else
    #include "procutils.h"
#endif


// ----------------------------------------------------------------------------
ClangLocator::ClangLocator()
{
    //ctor
}
// ----------------------------------------------------------------------------
ClangLocator::~ClangLocator()
{
    //dtor
}

#ifdef __WXMSW__
void ClangLocator::WindowsAddClangFoundSearchPaths(const wxString compilerMasterPath)
{
    // Check directory depth and only check if depth is 2 or more!!!!
    wxFileName fnMasterPath(compilerMasterPath);

    if (fnMasterPath.GetDirCount() != 0)
    {
        wxString cMPathLower(compilerMasterPath);
        cMPathLower.Lower();

        if (
            (cMPathLower.Find("clang") != wxNOT_FOUND)  ||
            (cMPathLower.Find("llvm") != wxNOT_FOUND)
        )
        {
            // Path is to the LLVM or CLANG compiler, so exit
            return;
        }

        // Currently only the MSYS2 compiler supports multiple compiler configurations
        if (cMPathLower.Find("mingw") != wxNOT_FOUND)
        {
            // Go up one directory
            wxFileName fnMPTest(compilerMasterPath);
            fnMPTest = fnMPTest.GetPath();

            if (wxDirExists(fnMPTest.GetFullPath()))
            {
                if (cMPathLower.Find("32") != wxNOT_FOUND)
                {
                    wxString testDir = fnMPTest.GetFullPath() + wxFILE_SEP_PATH + "clang32";

                    if (wxDirExists(testDir))
                    {
                        m_SearchPathsToTry.Add(testDir);
                    }
                }
                else
                {
                    wxString testDir = fnMPTest.GetFullPath() + wxFILE_SEP_PATH + "clang64";

                    if (wxDirExists(testDir))
                    {
                        m_SearchPathsToTry.Add(testDir);
                    }

#ifdef __arm__
                    testDir = fnMPTest.GetFullPath() + wxFILE_SEP_PATH + "clangarm64";

                    if (wxDirExists(testDir))
                    {
                        m_SearchPathsToTry.Add(testDir);
                    }

#endif // #ifdef __arm__
                }
            }
        }
    }
}

#endif // ifdef __WXMSW__
void ClangLocator::LoadSearchPaths()
{
    m_SearchPathsToTry.Clear();
    // Add Project default compiler path to search if a project is loaded
    cbProject * pProject = Manager::Get()->GetProjectManager()->GetActiveProject();

    if (pProject)
    {
        // First check project global compiler is valid
        int compilerIdx = CompilerFactory::GetCompilerIndex(pProject->GetCompilerID());

        if (compilerIdx != -1)
        {
            Compiler * prjCompiler = CompilerFactory::GetCompiler(compilerIdx);

            if (prjCompiler)
            {
                wxString mPath = prjCompiler->GetMasterPath();

                if (!mPath.empty() && wxDirExists(mPath))
                {
                    m_SearchPathsToTry.Add(mPath);
#ifdef __WXMSW__
                    WindowsAddClangFoundSearchPaths(mPath);
#endif // ifdef __WXMSW__
                }
            }
        }
    }

    // Add default compiler path to search
    Compiler * defaultCompiler = CompilerFactory::GetDefaultCompiler();

    if (defaultCompiler)
    {
        wxString mPath = defaultCompiler->GetMasterPath();

        if (!mPath.empty() && wxDirExists(mPath))
        {
            m_SearchPathsToTry.Add(mPath);
#ifdef __WXMSW__
            WindowsAddClangFoundSearchPaths(mPath);
#endif // ifdef __WXMSW__
        }
    }

#if defined(__WXMSW__)

    // Add just in case
    if (wxDirExists("c:\\program files\\LLVM"))
    {
        m_SearchPathsToTry.Add("c:\\program files\\LLVM");
    }

    if (wxDirExists("c:\\LLVM"))
    {
        m_SearchPathsToTry.Add("c:\\LLVM");
    }

    // ADD Windows LLVM registry entries
    wxString llvmInstallPath, llvmVersion;
    wxArrayString regKeys;
    regKeys.Add("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    regKeys.Add("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");

    for (size_t i = 0; i < regKeys.size(); ++i)
    {
        if (ReadMSWInstallLocation(regKeys.Item(i), llvmInstallPath, llvmVersion))
        {
            if (!llvmInstallPath.empty() && wxDirExists(llvmInstallPath))
            {
                m_SearchPathsToTry.Add(llvmInstallPath);
            }
        }
    }

#else
    m_SearchPathsToTry.Add("/usr");
#endif
    // Just in case
    wxFileName cbExecFileName(wxStandardPaths::Get().GetExecutablePath());

    if (wxDirExists(cbExecFileName.GetPath() + "lsp"))
    {
        m_SearchPathsToTry.Add(cbExecFileName.GetPath() + "lsp");
    }
}
// ----------------------------------------------------------------------------
wxString ClangLocator::LocateFilenameInPath(wxString findFileName)
{
    wxString location = wxEmptyString;
    wxArrayString clangLocations;
    wxArrayString envPaths = GetEnvPaths();

    for (wxString path : envPaths)
    {
        wxLogNull nolog; //turn off 'not found' messages
        size_t cnt = ScanForFiles(path, clangLocations, findFileName);

        for (size_t ii = 0; ii < cnt; ++ii)
        {
            if (wxFileExists(clangLocations[ii]))
            {
                wxString endBin = _(wxFILE_SEP_PATH) + "bin";

                if (clangLocations[ii].EndsWith(endBin))
                {
                    wxFileName fnLocation(clangLocations[ii]);
                    location = fnLocation.GetPath();        // This removes the "bin" directory
                    break;
                }
            }
        }

        if (!location.empty())
        {
            break;
        }
    }

    return location;
}

// ----------------------------------------------------------------------------
bool ClangLocator::CheckDirectoryStructureFileName(const wxString & location, const wxString & findFilename, wxString & detectedFilename, wxString & detectedeDir)
{
    bool bFileNameFound = false;
    detectedFilename = wxEmptyString;
    detectedeDir = wxEmptyString;

    if (wxFileExists(location + wxFILE_SEP_PATH + findFilename))
    {
        wxString endBin = _(wxFILE_SEP_PATH) + "bin";

        if (location.EndsWith(endBin))
        {
            detectedFilename = location + wxFILE_SEP_PATH + findFilename;
            wxFileName fnLocation(location);
            detectedeDir = fnLocation.GetPath(); // This removes the "bin" directory
            bFileNameFound = true;
        }
    }

    if (!bFileNameFound && wxFileExists(location + wxFILE_SEP_PATH + "bin" + wxFILE_SEP_PATH + findFilename))
    {
        detectedFilename = location + wxFILE_SEP_PATH + "bin" + wxFILE_SEP_PATH + findFilename;
        detectedeDir = location + wxFILE_SEP_PATH + "bin";
        bFileNameFound = true;
    }

    if (!bFileNameFound && wxDirExists(location + wxFILE_SEP_PATH + "lsp" + wxFILE_SEP_PATH + findFilename))
    {
        detectedFilename = location + wxFILE_SEP_PATH + "lsp" + wxFILE_SEP_PATH + findFilename;
        detectedeDir = location + wxFILE_SEP_PATH + "lsp";
        bFileNameFound = true;
    }

    return bFileNameFound;
}

// ----------------------------------------------------------------------------
bool ClangLocator::LocateLLVMFilename(const wxString & location, const wxString & findFilename, wxString & detectedFilename, wxString & detectedeDir)
{
    bool bFileNameFound = false;

    if (!location.empty())
    {
        if (wxFileExists(location))
        {
            wxFileName fLocation(location);

            if (wxFileExists(fLocation.GetPath() + wxFILE_SEP_PATH + findFilename))
            {
                detectedFilename = fLocation.GetPath() + wxFILE_SEP_PATH + findFilename;
                detectedeDir = fLocation.GetPath();
                bFileNameFound = true;
            }
        }

        if (!bFileNameFound && wxDirExists(location))
        {
            bFileNameFound  = CheckDirectoryStructureFileName(location, findFilename, detectedFilename, detectedeDir);
        }
    }

    if (!bFileNameFound)
    {
        for (wxString pathToTry : m_SearchPathsToTry)
        {
            if (!bFileNameFound && wxDirExists(pathToTry))
            {
                bFileNameFound  = CheckDirectoryStructureFileName(pathToTry, findFilename, detectedFilename, detectedeDir);

                if (bFileNameFound)
                {
                    break;
                }
            }
        }
    }

    if (!bFileNameFound)
    {
        // Check clangd in the path
        wxString clangDaemonFilenameCheck = LocateFilenameInPath(findFilename);

        if (!clangDaemonFilenameCheck.empty())
        {
            wxFileName fLocation(clangDaemonFilenameCheck);
            detectedFilename = clangDaemonFilenameCheck;
            detectedeDir = fLocation.GetPath();
            bFileNameFound = true;
        }
    }

    return bFileNameFound;
}

// ----------------------------------------------------------------------------
bool ClangLocator::LocateLLVMResources(wxString & LLVMMasterPath, wxString & clangDaemonFilename, wxString & clangExeFilename, wxString & clangIncDir)
{
    // Clear detected data
    LLVMMasterPath = wxEmptyString;
    clangDaemonFilename = wxEmptyString;
    clangExeFilename = wxEmptyString;
    clangIncDir = wxEmptyString;
    // Load search paths
    LoadSearchPaths();
    // Load old path if available
    ConfigManager * cfg = Manager::Get()->GetConfigManager(_T("clangd_client"));
    wxString cfgLLVM_MasterPath = cfg->Read("/LLVM_MasterPath", wxEmptyString);
    // need to use strings
    const wxString clangDaemonFilenameConst(CLANG_DAEMON_FILENAME);
    const wxString clangFilenameConst(CLANG_FILENAME);
    wxString clangDaemonDirectory = wxEmptyString;
    wxString clangDirectory = wxEmptyString;
    // Detect clangDaemon file
    bool bClangDaemonFound = ClangLocator::LocateLLVMFilename(cfgLLVM_MasterPath, clangDaemonFilenameConst, clangDaemonFilename, clangDaemonDirectory);
    // Detect clang file
    bool bClangFound;

    if (clangDaemonDirectory.empty())
    {
        bClangFound = ClangLocator::LocateLLVMFilename(cfgLLVM_MasterPath, clangFilenameConst, clangExeFilename, clangDirectory);
    }
    else
    {
        bClangFound = ClangLocator::LocateLLVMFilename(clangDaemonDirectory, clangFilenameConst, clangExeFilename, clangDirectory);
    }

    // Configure masterpath from detected directories
    if (clangDaemonDirectory.empty())
    {
        if (!clangDirectory.empty())
        {
            LLVMMasterPath = clangDirectory;
        }
    }
    else
    {
        LLVMMasterPath = clangDaemonDirectory;
    }

    bool bLLVMResourceDirFound = false;

    if (!LLVMMasterPath.empty())
    {
        wxString detectedClangVersion = wxEmptyString;

        if (!clangDaemonFilename.empty())
        {
            detectedClangVersion =  GetExeFileVersion(clangDaemonFilename);
        }

        bLLVMResourceDirFound = LocateIncludeClangDir(LLVMMasterPath, detectedClangVersion, clangIncDir);
    }

    return (bClangDaemonFound || bClangFound || bLLVMResourceDirFound);
}

// ----------------------------------------------------------------------------
bool ClangLocator::LocateIncludeClangDir(const wxString & LLVMMasterPath, const wxString & detectedClangVersion, wxString & clangIncClangPath)
{
    bool bIncludeClangDirFound = false;
    clangIncClangPath = wxEmptyString;

    if (!LLVMMasterPath.empty())
    {
        wxString endBin = _(wxFILE_SEP_PATH) + "bin";

        if (LLVMMasterPath.EndsWith(endBin))
        {
            wxFileName fnIncClang(LLVMMasterPath);

            if (wxDirExists(fnIncClang.GetPath()))
            {
                wxString clangIncClangPathChecking = fnIncClang.GetPath() + wxFILE_SEP_PATH + "lib" + wxFILE_SEP_PATH +  "clang";

                if (wxDirExists(clangIncClangPathChecking))
                {
                    if (detectedClangVersion.empty())
                    {
                        clangIncClangPath = clangIncClangPathChecking;
                        bIncludeClangDirFound = true;
                    }
                    else
                    {
                        clangIncClangPathChecking += (wxFILE_SEP_PATH + detectedClangVersion);

                        if (wxDirExists(clangIncClangPathChecking))
                        {
                            clangIncClangPath = clangIncClangPathChecking;
                            bIncludeClangDirFound = true;
                        }
                    }
                }
            }
        }
    }

    return bIncludeClangDirFound;
}

// ----------------------------------------------------------------------------
wxArrayString ClangLocator::GetEnvPaths() const
{
    wxString path;

    if (!::wxGetEnv("PATH", &path))
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
size_t ClangLocator::ScanForFiles(wxString path, wxArrayString & foundFiles, wxString mask)
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
    if (path.Matches("/mnt/?/*"))
    {
        return 0;    //eliminate massive number of wsl windows paths
    }

#endif

    if (not wxDirExists(path))
    {
        return 0;
    }

    wxString filename = wxFindFirstFile(path + wxFILE_SEP_PATH + mask, wxFILE);

    while (filename.Length())
    {
        wxFileName file(path + wxFILE_SEP_PATH + filename);
        file.MakeAbsolute();
        foundFiles.Add(file.GetFullPath());
        filename = wxFindNextFile();

        if (filename.empty())
        {
            break;
        }
    }

    return foundFiles.GetCount();
}

// ----------------------------------------------------------------------------
#ifdef __WXMSW__
bool ClangLocator::ReadMSWInstallLocation(const wxString & regkey, wxString & installPath, wxString & llvmVersion)
{
    wxRegKey reg(wxRegKey::HKLM, regkey);
    installPath.Clear();
    llvmVersion.Clear();

    if (reg.Exists())
    {
        reg.QueryValue("DisplayIcon", installPath);
        reg.QueryValue("DisplayVersion", llvmVersion);
    }

    return !installPath.IsEmpty() && !llvmVersion.IsEmpty();
}
#endif

// ----------------------------------------------------------------------------
wxString ClangLocator::GetExeFileVersion(const wxString & clangExeFilename)
{
    wxString command;
    wxArrayString stdoutArr;
    command << clangExeFilename << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);

    if (!stdoutArr.IsEmpty())
    {
        wxString versionString = stdoutArr.Item(0);
        wxRegEx reCmd("[0-9]+(\\.[0-9]+)+");

        if (reCmd.Matches(versionString))
        {
            versionString = reCmd.GetMatch(versionString);
        }
        else
        {
            if (versionString.Find("(") != wxNOT_FOUND)
            {
                //versionString = versionString.AfterLast('(');
                //versionString = versionString.BeforeLast(')');
                versionString = versionString.BeforeFirst('(');
            }
        }

        return versionString;
    }

    return wxString();
}

// ----------------------------------------------------------------------------
bool ClangLocator::IsClangFileVersionValid(const wxString & clangExeFilename)
{
    wxString clangdVersion = GetExeFileVersion(clangExeFilename);
    clangdVersion = clangdVersion.BeforeFirst('.').AfterLast(' ');
    int versionNum = stoi(clangdVersion.ToStdString());
    return (versionNum >= 13);
}
