/***************************************************************
 * Name:      wx_pch.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:    GeO (GeO.GeO@live.de)
 * Created:   2009-06-28
 * Copyright: GeO ()
 * License:
 **************************************************************/

#ifndef WX_PCH_H_INCLUDED
#define WX_PCH_H_INCLUDED

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifdef WX_PRECOMP
    #include <wx/treectrl.h>
    #include <wx/textfile.h>
    #include <wx/filename.h>
    #include <wx/stdpaths.h>
    #include <wx/dynarray.h>
    #include <wx/ffile.h>
#endif // WX_PRECOMP

#endif // WX_PCH_H_INCLUDED
