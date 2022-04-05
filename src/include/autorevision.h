/*0*/
// Don't include this header, only configmanager-revision.cpp should do this.
#ifndef AUTOREVISION_H
#define AUTOREVISION_H


#include <wx/string.h>

namespace autorevision
{
const unsigned int svn_revision = 12777;
const wxString svnRevision(_T("12777 EXPERIMENTAL PLUS"));
const wxString svnDate(_T("2022-04-04 00:00:00"));
}

#endif
