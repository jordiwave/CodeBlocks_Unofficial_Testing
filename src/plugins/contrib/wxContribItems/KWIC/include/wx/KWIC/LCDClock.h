/////////////////////////////////////////////////////////////////////////////
// Name:        LCDClock.h
// Purpose:     wxIndustrialControls Library
// Author:      Marco Cavallini <m.cavallini AT koansoftware.com>
// Modified by:
// Copyright:   (C)2004-2006 Copyright by Koan s.a.s. - www.koansoftware.com
// Licence:     KWIC License http://www.koansoftware.com/kwic/kwic-license.htm
/////////////////////////////////////////////////////////////////////////////
//
//	Cleaned up and modified by Gary Harris for wxSmithKWIC, 2010.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef LCDCLOCK_H
#define LCDCLOCK_H

#include "LCDWindow.h"
#include <wx/timer.h>

#define TIMER_TIME 3001

#include "wx/dlimpexp.h"

class CTimeAlarm;

class WXEXPORT kwxLCDClock : public kwxLCDDisplay
{
public:

    kwxLCDClock(wxWindow *parent, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
    kwxLCDClock() {};
    ~kwxLCDClock() ;

    bool Create(wxWindow *parent, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
    void StartClock();
    void StopClock();
    CTimeAlarm *alarm;

private:
    DECLARE_EVENT_TABLE()

    wxTimer m_timer;

    wxDateTime m_LastCheck;
    void OnTimer(wxTimerEvent &event);
};

#endif
