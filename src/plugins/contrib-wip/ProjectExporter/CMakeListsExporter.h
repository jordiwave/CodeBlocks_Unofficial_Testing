#ifndef __CMAKELISTSEXPORTER_H__
#define __CMAKELISTSEXPORTER_H__

// System include files
#include <wx/regex.h>

// ProjectExporter include files
#include "ExporterBase.h"

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

        void ExpandMacros(wxString& buffer, bool subrequest = false);
        void ConvertMacros(wxString& buffer, bool subrequest = false);
        void ExportGlobalVariableSets(ExportMode eMode);
        void ExportMacros();

        wxString ValidateFilename(const wxString& iFileName);
        wxString GetHumanReadableOptionRelation(ProjectBuildTarget* buildTarget, OptionsRelationType type);

        wxString        m_content;
        wxArrayString   m_options;
        wxRegEx         m_RE_Unix;
        wxRegEx         m_RE_DOS;

};

#endif // __CMAKELISTSEXPORTER_H__
