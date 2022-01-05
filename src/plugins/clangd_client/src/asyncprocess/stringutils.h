//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : worker_thread.cpp
//
// -------------------------------------------------------------------------
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
////////////////////////////////////////////////////////////////////////////// #ifndef STRINGUTILS_H
// Curated for CodeBlocks by pecan 2020/09/28

#ifndef STRINGUTILS_H
#define STRINGUTILS_H


#include <wx/string.h>
#include <wx/arrstr.h>

std::string ToStdString(const wxString& str);
bool FuzzyMatch(const wxString& needle, const wxString& haystack);
#endif
