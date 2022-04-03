#ifndef CLANGLOCATOR_H
#define CLANGLOCATOR_H

#include <cstddef>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/arrstr.h>

// clang executable variables to simplify the code
#ifdef __WXMSW__
    #define CLANG_FILENAME "clang.exe"
    #define CLANG_DAEMON_FILENAME "clangd.exe"
#else
    #define CLANG_FILENAME "clang"
    #define CLANG_DAEMON_FILENAME "clangd"
#endif

class ClangLocator
{
    public:
        ClangLocator();
        virtual ~ClangLocator();

        bool LocateLLVMResources(wxString & LLVMMasterPath, wxString & clangDaemonFilename, wxString & clangExeFilename, wxString & clangIncDir);
        static bool LocateIncludeClangDir(const wxString & LLVMMasterPath, const wxString & detectedClangVersion, wxString & clangIncClangPath);

        static wxString GetExeFileVersion(const wxString & clangExeFilename);
        static bool IsClangFileVersionValid(const wxString & clangExeFilename);

    private:
        wxArrayString m_SearchPathsToTry;   // Paths loaded in constructor

#ifdef __WXMSW__
        void WindowsAddClangFoundSearchPaths(const wxString compilerMasterPath);
#endif
        void LoadSearchPaths();
        wxArrayString GetEnvPaths() const;
        std::size_t ScanForFiles(wxString path, wxArrayString & foundFiles, wxString mask);
#ifdef __WXMSW__
        bool ReadMSWInstallLocation(const wxString & regkey, wxString & installPath, wxString & llvmVersion);
#endif
        wxString LocateFilenameInPath(wxString findFileName);
        bool CheckDirectoryStructureFileName(const wxString & location, const wxString & findFilename, wxString & detectedFilename, wxString & detectedeDir);
        bool LocateLLVMFilename(const wxString & location, const wxString & findFilename, wxString & detectedFilename, wxString & detectedeDir);
};

#endif // CLANGLOCATOR_H
