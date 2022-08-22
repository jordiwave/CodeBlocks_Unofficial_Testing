#ifndef IL_GLOBALS_H
#define IL_GLOBALS_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/filename.h>
    #include <wx/txtstrm.h>

    #include <editorbase.h>
    #include <editormanager.h>
    #include <logmanager.h>
    #include <manager.h>
#endif

wxString GetParentDir(const wxString & path);

bool DirIsChildOf(const wxString & path, const wxString & child);

bool WildCardListMatch(wxString list, wxString name, bool strip = false);

bool PromptSaveOpenFile(wxString message, wxFileName path);


#endif //IL_GLOBALS_H
