#ifndef __CMAKELISTSEXPORTER_H__
#define __CMAKELISTSEXPORTER_H__

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
        wxString GetOptions(const wxString & source);
        wxString ValidateFilename(const wxString & iFileName);
        wxString GetHumanReadableOptionRelation(ProjectBuildTarget * buildTarget, OptionsRelationType type);
        wxArrayString m_options;
};

#endif // __CMAKELISTSEXPORTER_H__
