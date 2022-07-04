#include "NSIFile.h"

#include <globals.h>
#include "langheader.h"
#include <wx/ffile.h>

wxString com = _T("; ");
wxString comlin = _T(";-------------------------");
wxString emp = _T("");

NSIFile::NSIFile(wxString name, wxString execpath, wxString nsispath)
{
    m_name = name;
    m_execpath = execpath;
    m_nsispath = nsispath;
    nsi = new wxTextFile(execpath + _T("/") + name + _T(".nsi"));
    m_buninstall = false;
    m_exefile = wxEmptyString;
    m_license = wxEmptyString;
}

NSIFile::~NSIFile()
{
}

wxArrayString NSIFile::MakeRelative(wxArrayString ar)
{
    for (unsigned int i = 0; i < ar.GetCount(); i++)
    {
        wxFileName * a = new wxFileName(ar[i]);

        if (a->GetExt() == _T("exe"))
        {
            m_exefile = a->GetFullName();
        }
    }

    return ar;
}

void NSIFile::SetAll(wxString file, wxString inst, wxArrayInt pages, wxArrayString files, bool shortcut)
{
    m_fileout = file;
    m_definstdir = inst;

    if (inst == _("Desktop"))
    {
        m_definstdir = _T("$DESKTOP");
    }

    if (inst == _("Programfiles"))
    {
        m_definstdir = _T("$PROGRAMFILES");
    }

    m_pages = pages;
    m_filesalwaysinstall = MakeRelative(files);
    m_filesuninstall = m_filesalwaysinstall;
    m_bshort = shortcut;
}

void NSIFile::SetLicense(wxString lic)
{
    m_license = lic;
}

void NSIFile::SetSections(wxArrayString sec)
{
    m_sections = sec;
}

void NSIFile::SetComponent(CompItemArray comp)
{
    m_comp = comp;
}

void NSIFile::Create()
{
    if (wxFileExists(m_execpath + _T("/") + m_name + _T(".nsi")))
    {
        wxRemoveFile(m_execpath + _T("/") + m_name + _T(".nsi"));
    }

    nsi->Create();
    CreateLangHeader();
    Header();
    nsi->AddLine(comlin);
    nsi->AddLine(emp);
    Variablen();
    nsi->AddLine(emp);
    nsi->AddLine(comlin);
    nsi->AddLine(emp);
    Insttypes();
    nsi->AddLine(emp);
    nsi->AddLine(comlin);
    nsi->AddLine(emp);
    Pages();
    nsi->AddLine(emp);
    nsi->AddLine(comlin);
    nsi->AddLine(emp);
    Components();
    nsi->AddLine(emp);
    nsi->AddLine(comlin);
    nsi->AddLine(emp);
    SecInstall();
    SecUninstall();
    nsi->Write();
    nsi->Close();
    ExecuteNSIMake();
}

void NSIFile::CreateLangHeader()
{
    wxString lhpath = m_execpath + _T("/lang.nsh");

    if (wxFileExists(lhpath))
    {
        wxRemoveFile(lhpath);
    }

    wxFFile * lhf = new wxFFile(lhpath, _T("wb"));
    lhf->Write(lang_nsh, sizeof lang_nsh);
    lhf->Close();
}

void NSIFile::Header()
{
    nsi->AddLine(com + m_name + _T(".nsi"));
    nsi->AddLine(com);
    nsi->AddLine(com + _("This script was generated by NSISGUI don't edit it!"));
    nsi->AddLine(com);
}

void NSIFile::Variablen()
{
    nsi->AddLine(com + _("The name of the installer"));
    nsi->AddLine(_T("Name \"") + m_name + _T("\""));
    nsi->AddLine(emp);
    nsi->AddLine(com + _("The file to write"));
    nsi->AddLine(_T("OutFile \"") + m_fileout + _T("\""));
    nsi->AddLine(emp);
    nsi->AddLine(com + _("The default installation directory"));
    nsi->AddLine(_T("InstallDir \"") + m_definstdir + _T("\\") + m_name + _T("\""));
    nsi->AddLine(emp);
    nsi->AddLine(com + _("Turn XP Style on"));
    nsi->AddLine(_T("XPStyle on"));
    nsi->AddLine(emp);
    nsi->AddLine(com + _("Request application privileges for Windows Vista"));
    nsi->AddLine(_T("RequestExecutionLevel user"));
    nsi->AddLine(emp);
    nsi->AddLine(com + _("Language"));
    nsi->AddLine(_T("!include \"lang.nsh\""));

    if (m_license != wxEmptyString)
    {
        nsi->AddLine(emp);
        nsi->AddLine(com + _("License Data"));
        nsi->AddLine(_T("LicenseData \"") + m_license + _T("\""));
    }
}

void NSIFile::Insttypes()
{
    for (unsigned int i = 0; i < m_sections.GetCount(); i++)
    {
        nsi->AddLine(_T("Insttype ") + m_sections[i]);
    }
}

void NSIFile::Pages()
{
    nsi->AddLine(com + _T("Pages"));

    for (unsigned int i = 0; i < m_pages.GetCount(); i++)
    {
        if (m_pages[i] == 0)
        {
            nsi->AddLine(_T("Page license"));
        }

        if (m_pages[i] == 1)
        {
            nsi->AddLine(_T("Page components"));
        }

        if (m_pages[i] == 2)
        {
            nsi->AddLine(_T("Page directory"));
        }

        if (m_pages[i] == 3)
        {
            nsi->AddLine(_T("Page instfiles"));
        }

        if (m_pages[i] == 4)
        {
            nsi->AddLine(_T("UninstPage uninstConfirm"));
        }

        if (m_pages[i] == 5)
        {
            nsi->AddLine(_T("UninstPage instfiles"));
            m_buninstall = true;
        }
    }
}

void NSIFile::SecInstall()
{
    nsi->AddLine(com + _("The stuff to install"));
    nsi->AddLine(_T("Section \"\""));
    nsi->AddLine(emp);

    if (m_filesalwaysinstall.GetCount() > 0)
    {
        nsi->AddLine(_("\t") + com + _("Set output path to the installation directory."));
        nsi->AddLine(_T("\tSetOutPath $INSTDIR"));
    }

    for (unsigned int i = 0; i < m_filesalwaysinstall.GetCount(); i++)
    {
        nsi->AddLine(_T("\tFile \"") + m_filesalwaysinstall[i] + _T("\""));
    }

    if (m_bshort && (m_exefile != wxEmptyString))
    {
        nsi->AddLine(_T("\tCreateShortCut $DESKTOP\\") + m_name + _T(".lnk $INSTDIR\\") + m_exefile);
    }

    if (m_buninstall)
    {
        nsi->AddLine(_T("\tWriteUninstaller $INSTDIR\\uninstall.exe"));
    }

    nsi->AddLine(_T("SectionEnd"));
}

void NSIFile::Components()
{
    for (unsigned int i = 0; i < m_comp.GetCount(); i++)
    {
        CompItem * item = m_comp[i];

        if (!item->m_Haschilds)
        {
            Component(item->m_Name, item->m_Files, item->m_Sections);
        }
        else
        {
            ComponentWithChild(item->m_Name, item->m_Childs);
        }
    }
}

void NSIFile::ComponentWithChild(wxString name, CompItemArray child)
{
    nsi->AddLine(_T("SectionGroup \"") + name + _T("\""));

    for (unsigned int i = 0; i < child.GetCount(); i++)
    {
        Component(child[i]->m_Name, child[i]->m_Files, child[i]->m_Sections, _T("\t"));
    }

    nsi->AddLine(_T("SectionGroupEnd"));
}

void NSIFile::Component(wxString name, wxArrayString file, wxArrayInt insttyp, wxString tab)
{
    wxArrayString files = MakeRelative(file);
    nsi->AddLine(tab + _T("Section \"") + name + _T("\""));

    if (insttyp.GetCount() > 0)
    {
        wxString insec = tab + _T("\tSectionIn");
        wxString temp;

        for (unsigned int i = 0; i < insttyp.GetCount(); i++)
        {
            temp.Printf(_T(" %i"), insttyp[i] + 1);
            insec += temp;
        }

        nsi->AddLine(insec);
    }

    if (files.GetCount() > 0)
    {
        nsi->AddLine(tab + _T("\tSetOutPath $INSTDIR"));
    }

    for (unsigned int i = 0; i < files.GetCount(); i++)
    {
        for (unsigned int j = 0; j < m_filesalwaysinstall.GetCount(); j++)
            if (m_filesalwaysinstall[j] == files[i])
            {
                m_filesalwaysinstall.RemoveAt(j);
            }

        nsi->AddLine(tab + _T("\tFile \"") + files[i] + _T("\""));
    }

    nsi->AddLine(tab + _T("SectionEnd"));
}

void NSIFile::SecUninstall()
{
    if (m_buninstall)
    {
        nsi->AddLine(emp);
        nsi->AddLine(comlin);
        nsi->AddLine(emp);
        nsi->AddLine(com + _("The stuff to uninstall"));
        nsi->AddLine(_T("Section \"Uninstall\""));
        nsi->AddLine(_T("\tDelete $INSTDIR\\uninstall.exe"));

        for (unsigned int i = 0; i < m_filesuninstall.GetCount(); i++)
        {
            wxFileName * fn = new wxFileName(m_filesuninstall[i]);
            nsi->AddLine(_T("\tDelete \"$INSTDIR\\") + fn->GetFullName() + _T("\""));
        }

        if (m_bshort && (m_exefile != wxEmptyString))
        {
            nsi->AddLine(_T("\tDelete \"$DESKTOP\\") + m_name + _T(".lnk\""));
        }

        nsi->AddLine(_T("\tRMDir $INSTDIR"));
        nsi->AddLine(_T("SectionEnd"));
    }
}

void NSIFile::ExecuteNSIMake()
{
    wxExecute(m_nsispath + _T("/makensis ") +  m_name + _T(".nsi"), m_output);
}

wxArrayString NSIFile::GetOutput()
{
    return m_output;
}