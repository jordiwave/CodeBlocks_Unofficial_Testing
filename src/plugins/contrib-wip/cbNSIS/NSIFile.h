#ifndef NSIFILE_H
#define NSIFILE_H

#include "wx_pch.h"

#ifndef WX_PRECOMP
    #include <wx/textfile.h>
    #include <wx/filename.h>
    #include <wx/stdpaths.h>
#endif

#include "CompMap.h"

class NSIFile
{
    public:
        NSIFile(wxString name, wxString execpath, wxString nsispath);
        virtual ~NSIFile();
        void SetAll(wxString file, wxString inst, wxArrayInt pages, wxArrayString files, bool shortcut);
        void SetLicense(wxString lic);
        void SetSections(wxArrayString sec);
        void SetComponent(CompItemArray comp);
        void Create();
        wxArrayString GetOutput();
    protected:
    private:
        wxArrayString MakeRelative(wxArrayString ar);
        void CreateLangHeader();
        void Header();
        void Variablen();
        void Insttypes();
        void Pages();
        void SecInstall();
        void SecUninstall();
        void Components();
        void ComponentWithChild(wxString name, CompItemArray child);
        void Component(wxString name, wxArrayString files, wxArrayInt insttyp, wxString tab = wxEmptyString);
        void ExecuteNSIMake();

        wxTextFile * nsi;
        wxString m_nsispath;
        wxString m_execpath;

        wxArrayString m_output;
        wxString m_name;
        wxString m_fileout;
        wxString m_exefile;
        wxString m_license;
        wxString m_definstdir;
        CompItemArray m_comp;
        wxArrayInt m_pages;
        wxArrayString m_filesalwaysinstall;
        wxArrayString m_filesuninstall;
        wxArrayString m_sections;
        bool m_bshort;

        bool m_buninstall;
};

#endif // NSIFILE_H
