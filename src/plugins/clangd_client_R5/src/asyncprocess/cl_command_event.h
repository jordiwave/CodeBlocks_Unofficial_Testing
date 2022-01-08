//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_command_event.h
//
// -------------------------------------------------------------------------
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
// Curated for CodeBlocks by pecan 2020/09/28

#ifndef CLCOMMANDEVENT_H
#define CLCOMMANDEVENT_H

#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/sharedptr.h>

// Set of flags that can be passed within the 'S{G}etInt' function of clCommandEvent
enum {
    kEventImportingFolder = 0x00000001,
};

/// a wxCommandEvent that takes ownership of the clientData
class /*WXDLLIMPEXP_CL*/ clCommandEvent : public wxCommandEvent
{
protected:
    wxSharedPtr<wxClientData> m_ptr;
    wxArrayString m_strings;
    wxString m_fileName;
    wxString m_oldName;
    bool m_answer;
    bool m_allowed;
    int m_lineNumber;
    bool m_selected;

public:
    clCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCommandEvent(const clCommandEvent& event);
    clCommandEvent& operator=(const clCommandEvent& src);
    virtual ~clCommandEvent();

    clCommandEvent& SetSelected(bool selected)
    {
        this->m_selected = selected;
        return *this;
    }
    bool IsSelected() const { return m_selected; }

    // Veto management
    void Veto() { this->m_allowed = false; }
    void Allow() { this->m_allowed = true; }

    // Hides wxCommandEvent::Set{Get}ClientObject
    void SetClientObject(wxClientData* clientObject);

    wxClientData* GetClientObject() const;
    virtual wxEvent* Clone() const;

    clCommandEvent& SetLineNumber(int lineNumber)
    {
        this->m_lineNumber = lineNumber;
        return *this;
    }
    int GetLineNumber() const { return m_lineNumber; }
    clCommandEvent& SetAllowed(bool allowed)
    {
        this->m_allowed = allowed;
        return *this;
    }
    clCommandEvent& SetAnswer(bool answer)
    {
        this->m_answer = answer;
        return *this;
    }
    clCommandEvent& SetFileName(const wxString& fileName)
    {
        this->m_fileName = fileName;
        return *this;
    }
    clCommandEvent& SetOldName(const wxString& oldName)
    {
        this->m_oldName = oldName;
        return *this;
    }
    clCommandEvent& SetPtr(const wxSharedPtr<wxClientData>& ptr)
    {
        this->m_ptr = ptr;
        return *this;
    }
    clCommandEvent& SetStrings(const wxArrayString& strings)
    {
        this->m_strings = strings;
        return *this;
    }
    bool IsAllowed() const { return m_allowed; }
    bool IsAnswer() const { return m_answer; }
    const wxString& GetFileName() const { return m_fileName; }
    const wxString& GetOldName() const { return m_oldName; }
    const wxSharedPtr<wxClientData>& GetPtr() const { return m_ptr; }
    const wxArrayString& GetStrings() const { return m_strings; }
    wxArrayString& GetStrings() { return m_strings; }
};

typedef void (wxEvtHandler::*clCommandEventFunction)(clCommandEvent&);
#define clCommandEventHandler(func) wxEVENT_HANDLER_CAST(clCommandEventFunction, func)

/// Source control event
class /*WXDLLIMPEXP_CL*/ clSourceControlEvent : public clCommandEvent
{
protected:
    wxString m_sourceControlName;

public:
    clSourceControlEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clSourceControlEvent(const clSourceControlEvent& event);
    clSourceControlEvent& operator=(const clSourceControlEvent& src);
    virtual ~clSourceControlEvent();
    virtual wxEvent* Clone() const;
    void SetSourceControlName(const wxString& sourceControlName) { this->m_sourceControlName = sourceControlName; }
    const wxString& GetSourceControlName() const { return m_sourceControlName; }
};

typedef void (wxEvtHandler::*clSourceControlEventFunction)(clSourceControlEvent&);
#define clSourceControlEventHandler(func) wxEVENT_HANDLER_CAST(clSourceControlEventFunction, func)

// --------------------------------------------------------------
// Processs event
// --------------------------------------------------------------
class IProcess;
class /*WXDLLIMPEXP_CL*/ clProcessEvent : public clCommandEvent
{
    wxString m_output;
    IProcess* m_process;

public:
    clProcessEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clProcessEvent(const clProcessEvent& event);
    clProcessEvent& operator=(const clProcessEvent& src);
    virtual ~clProcessEvent();
    virtual wxEvent* Clone() const { return new clProcessEvent(*this); }

    void SetOutput(const wxString& output) { this->m_output = output; }
    void SetProcess(IProcess* process) { this->m_process = process; }
    const wxString& GetOutput() const { return m_output; }
    IProcess* GetProcess() { return m_process; }
};

typedef void (wxEvtHandler::*clProcessEventFunction)(clProcessEvent&);
#define clProcessEventHandler(func) wxEVENT_HANDLER_CAST(clProcessEventFunction, func)

#endif // CLCOMMANDEVENT_H
