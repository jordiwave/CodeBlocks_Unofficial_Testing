#ifndef __CMAKELISTSEXPORTER_H__
#define __CMAKELISTSEXPORTER_H__

// System include files
#include <wx/regex.h>
#include <wx/textfile.h>

// ProjectExporter include files
#include "../ExporterBase.h"

class CMakeListsExporter : public ExporterBase
{
    public:
        /** Default constructor */
        CMakeListsExporter();
        /** Default destructor */
        virtual ~CMakeListsExporter();
        void RunExport();

    protected:

    private:
        enum ExportMode
        {
            GVS_EXPORT_ALL	= 0x00,
            GVS_EXPORT_DEFAULT_ONLY,
            GVS_EXPORT_NON_DEFAULT
        };

        void ExpandMacros(wxString & buffer);
        void ConvertMacros(wxString & buffer, bool bConvertWindowsSlashToLinux);
        void ExportGlobalVariableSets(ExportMode eMode);
        wxString ExportMacros(ProjectBuildTarget * buildTarget);
        void ExportGlobalVariables();
        wxString GetTargetRootDirectory(ProjectBuildTarget * buildTarget);

        wxString ValidateFilename(const wxString & iFileName);
        wxString GetHumanReadableOptionRelation(ProjectBuildTarget * buildTarget, OptionsRelationType type);

        const wxChar * EOL = wxTextFile::GetEOL();
        wxString    m_CBProjectRootDir;
        wxString    m_ContentCMakeListTarget;
        wxString    m_ContentCMakeListGlobalVariables;
        wxString    m_sGlobalVariableFileName;
        wxString    m_ContentCMakeListTopLevel;
        wxRegEx     m_RE_Unix;
        wxRegEx     m_RE_DOS;

        std::map<wxString, wxString> m_mapTargetOuputs;
};

#endif // __CMAKELISTSEXPORTER_H__
