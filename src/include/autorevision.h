/*12885*/
//don't include this header, only configmanager-revision.cpp should do this.
#ifndef AUTOREVISION_H
#define AUTOREVISION_H

#include <wx/string.h>

namespace autorevision
{
const unsigned int svn_revision = 12885;
const wxString svnRevision(_T("12885"));
const wxString svnDate(_T("2022-09-09 00:00:00"));
}

#endif
