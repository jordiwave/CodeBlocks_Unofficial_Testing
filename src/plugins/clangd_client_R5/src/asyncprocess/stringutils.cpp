//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : procutils.cpp
//
// -------------------------------------------------------------------------
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
// Curated for CodeBlocks by pecan 2020/09/28

#include <string>
#include "wx/string.h"
#include "wx/utils.h"   //wxToLower()

// ----------------------------------------------------------------------------
std::string ToStdString(const wxString& str)
// ----------------------------------------------------------------------------
{
    wxCharBuffer cb = str.ToAscii();
    const char* data = cb.data();
    if(!data) { data = str.mb_str(wxConvUTF8).data(); }
    if(!data) { data = str.To8BitData(); }

    std::string res;
    if(!data) { return res; }
    res = data;
    return res;
}
// ----------------------------------------------------------------------------
bool NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower)
// ----------------------------------------------------------------------------
{
    if(offset == str.size()) {
        return false;
    }
    size_t start = wxString::npos;
    word.Clear();
    for(; offset < str.size(); ++offset) {
        wxChar ch = str[offset];
        bool isWhitespace = ((ch == ' ') || (ch == '\t'));
        if(isWhitespace && (start != wxString::npos)) {
            // we found a trailing whitespace
            break;
        } else if(isWhitespace && (start == wxString::npos)) {
            // skip leading whitespace
            continue;
        } else if(start == wxString::npos) {
            start = offset;
        }
        if(makeLower) {
            ch = wxTolower(ch);
        }
        word << ch;
    }

    if((start != wxString::npos) && (offset > start)) {
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool FuzzyMatch(const wxString& needle, const wxString& haystack)
// ----------------------------------------------------------------------------
{
    wxString word;
    size_t offset = 0;
    wxString lcHaystack = haystack.Lower();
    while(NextWord(needle, offset, word, true)) {
        if(!lcHaystack.Contains(word)) {
            return false;
        }
    }
    return true;
}

