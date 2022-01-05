//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_command_event.cpp
//
// -------------------------------------------------------------------------
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
// Curated for CodeBlocks by pecan 2020/09/28

#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_answer(false)
    , m_allowed(true)
    , m_lineNumber(0)
    , m_selected(false)
{
}

clCommandEvent::clCommandEvent(const clCommandEvent& event)
    : wxCommandEvent(event)
    , m_answer(false)
    , m_allowed(true)
{
    *this = event;
}

clCommandEvent& clCommandEvent::operator=(const clCommandEvent& src)
{
    m_strings.clear();
    m_ptr = src.m_ptr;
    for(size_t i = 0; i < src.m_strings.size(); ++i) {
        m_strings.Add(src.m_strings.Item(i).c_str());
    }
    m_fileName = src.m_fileName;
    m_answer = src.m_answer;
    m_allowed = src.m_allowed;
    m_oldName = src.m_oldName;
    m_lineNumber = src.m_lineNumber;
    m_selected = src.m_selected;

    // Copy wxCommandEvent members here
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    return *this;
}

clCommandEvent::~clCommandEvent() { m_ptr.reset(); }

wxEvent* clCommandEvent::Clone() const
{
    clCommandEvent* new_event = new clCommandEvent(*this);
    return new_event;
}

void clCommandEvent::SetClientObject(wxClientData* clientObject) { m_ptr = clientObject; }

wxClientData* clCommandEvent::GetClientObject() const { return m_ptr.get(); }

//-------------------------------------------------------------------
// clProcessEvent
//-------------------------------------------------------------------

clProcessEvent::clProcessEvent(const clProcessEvent& event) { *this = event; }

clProcessEvent::clProcessEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_process(NULL)
{
}

clProcessEvent::~clProcessEvent() {}

clProcessEvent& clProcessEvent::operator=(const clProcessEvent& src)
{
    clCommandEvent::operator=(src);
    m_process = src.m_process;
    m_output = src.m_output;
    return *this;
}
