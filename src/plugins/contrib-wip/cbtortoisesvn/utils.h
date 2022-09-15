//******************************************************************************
//* Name:      utils.h
//* Purpose:   CBTortoiseSVN  utilities
//* Author:    Jan van den Borst
//* Copyright: (c) Jan van den Borst
//* License:   GPL
//******************************************************************************

#ifndef _UTILS_H
#define _UTILS_H

//******************************************************************************

#include <wx/string.h>
#include <sstream>
#include <vector>

//******************************************************************************

namespace CBTSVN
{
template <typename Dst, typename Src>
Dst lexical_cast(const Src & s)
{
    Dst d = Dst();
    std::stringstream ss("");
    ss << s;
    ss >> d;
    return d;
}

class IInterfaceBase
{
    public:
        virtual ~IInterfaceBase()
        {}
};

class Noncopyable
{
        Noncopyable(const Noncopyable &);
        Noncopyable & operator=(const Noncopyable &);
    protected:
        Noncopyable()
        {}
        ~Noncopyable()
        {}
};

// conversions
std::vector<int> convert(const wxString & s);
wxString convert(const std::vector<int> & vec);

// generic os
int  Run(const wxString & app, const wxString & dir, const wxString & params, wxString & output);
bool Run(bool blocked, bool hidden, const wxString & command, unsigned long & exit_code);

// generic wxWidgets
wxString GetBaseDir(const wxString & filename);

// generic codeblocks
void GetFolders(const wxString & plugin_name);
wxString GetWorkspaceFilename();
wxString GetProjectFilename();
wxString GetEditorFilename();

// global settings
bool GlobalInifileHasEntry(const wxString & plugin_name, const wxString & key);
wxString GetGlobalIniFile(const wxString & plugin_name);
bool ReadStringFromGlobalInifile(const wxString & plugin_name, const wxString & key, wxString & value);
bool WriteStringToGlobalInifile(const wxString & plugin_name, const wxString & key, const wxString & value);

// project / workspace related settings
wxString GetProjectIniFile(const wxString & plugin_name);
wxString GetWorkspaceIniFile(const wxString & plugin_name);
bool ReadStringFromProjectInifile(const wxString & plugin_name, const wxString & key, wxString & value);
bool WriteStringToProjectInifile(const wxString & plugin_name, const wxString & key, const wxString & value);
bool ReadStringFromWorkspaceInifile(const wxString & plugin_name, const wxString & key, wxString & value);
bool WriteStringToWorkspaceInifile(const wxString & plugin_name, const wxString & key, const wxString & value);

// general
bool IniFileHasEntry(const wxString & plugin_name, const wxString & inifile, const wxString & key);
bool ReadStringFromInifile(const wxString & plugin_name, const wxString & inifile, const wxString & key, wxString & value);
bool WriteStringToInifile(const wxString & plugin_name, const wxString & inifile, const wxString & key, const wxString & value);

wxString GetCustomDir(const wxString & plugin_name, const wxString & filename, const wxString & key_custom_location, const wxString & key_custom_relative);
};

//******************************************************************************
#endif // _UTILS_H
