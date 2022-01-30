// Don't include this header, only configmanager-revision.cpp should do this.
#ifndef AUTOREVISION_H
#define AUTOREVISION_H


#include <wx/string.h>

namespace autorevision
{
const unsigned int svn_revision = 12684;
const wxString svnRevision(_T("12684_EXPERIMENTAL_PLUS"));
const wxString svnDate(_T("2022-01-29 00:00:00"));
}

#endif
