#ifndef CLANGLOCATOR_H
#define CLANGLOCATOR_H

#include <cstddef>
#include <wx/string.h>
#include <wx/filename.h>

class wxArrayString;

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

    wxString Locate_Clang();
    wxString Locate_ClangDaemon();
    wxString Locate_ResourceDir(wxFileName fnClangd);

    wxString MSWLocate();
    wxArrayString GetEnvPaths() const;
    std::size_t ScanForFiles(wxString path, wxArrayString& foundFiles, wxString mask);
    bool ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion);
    wxString GetClangVersion(const wxString& clangBinary);
    wxString GetClangDaemonVersion(const wxString& clangDaemonBinary);
    bool IsClangMajorVersionNumberValid(const wxString& clangBinary);
    bool IsClangDaemonMajorVersionNumberValid(const wxString& clangBinary);

private:
    wxString Locate_ClangxFile(wxString findClangxFileName);
    wxString GetLLVMClangxVersion(const wxString& clangxBinary);
    bool IsClangxMajorVersionNumberValid(const wxString& clangxBinary);
};

#endif // CLANGLOCATOR_H
