#ifndef NSISGUIFILE_H
#define NSISGUIFILE_H

#include <wx/xml/xml.h>
#include "CompMap.h"

class NSISGUIFile
{
    public:
        NSISGUIFile();
        virtual ~NSISGUIFile();

        void Save(NGFOptions opt);
        NGFOptions Load(wxString File);

    protected:
    private:
        void CreateOption(wxXmlNode * parent, wxString Option, wxString Value);
        void CreateOptionBool(wxXmlNode * parent, wxString Option, bool Value);
        void CreateOptionInt(wxXmlNode * parent, wxString Option, int Value);
        void CreateOptionCompItem(wxXmlNode * parent, wxString Option, CompItem * Value);
        void CreateOptionCompItemArray(wxXmlNode * parent, wxString Option, CompItemArray Value);
        void CreateArrayOptionInt(wxXmlNode * parent, wxString Option, wxArrayInt Value);

        void CreateArrayOption(wxXmlNode * parent, wxString Option, wxArrayString Value);

        wxString GetOption(wxXmlNode * parent, wxString Option);
        bool GetOptionBool(wxXmlNode * parent, wxString Option);
        int GetOptionInt(wxXmlNode * parent, wxString Option);
        CompItem * GetOptionCompItem(wxXmlNode * parent, wxString Option);
        CompItemArray GetOptionCompItemArray(wxXmlNode * parent, wxString Option);
        wxArrayInt GetArrayOptionInt(wxXmlNode * parent, wxString Option);
        wxArrayString GetArrayOption(wxXmlNode * parent, wxString Option);

        wxXmlDocument * Ngf;
};

#endif // NSISGUIFILE_H
