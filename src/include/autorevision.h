/*0*/
// Don't include this header, only configmanager-revision.cpp should do this.
#ifndef AUTOREVISION_H
#define AUTOREVISION_H


#include <wx/string.h>

namespace autorevision
{
const unsigned int svn_revision = 12740;
const wxString svnRevision(_T("12740 EXPERIMENTAL PLUS"));
const wxString svnDate(_T("2022-03-05 00:00:00"));
}

#endif
