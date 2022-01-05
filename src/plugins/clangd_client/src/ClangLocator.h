#ifndef CLANGLOCATOR_H
#define CLANGLOCATOR_H

#include <cstddef>
#include <wx/string.h>

class wxArrayString;

class ClangLocator
{
    public:
        ClangLocator();
        virtual ~ClangLocator();

        wxString Locate_Clang();
        wxString Locate_Clangd();
        wxString Locate_ResourceDir(wxString clangDir="");

        wxString MSWLocate();
        wxArrayString GetEnvPaths() const;
        std::size_t ScanForFiles(wxString path, wxArrayString& foundFiles, wxString mask);
        bool ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion);
        wxString GetClangVersion(const wxString& clangBinary);

        // PATH environment variable separator
        #ifdef __WXMSW__
        #define ENV_PATH_SEPARATOR ";"
        #else
        #define ENV_PATH_SEPARATOR ":"
        #endif

    private:
};

#endif // CLANGLOCATOR_H
