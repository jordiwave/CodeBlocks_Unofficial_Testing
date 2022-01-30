#include "NSISGUIFile.h"

NSISGUIFile::NSISGUIFile()
{
    Ngf = new wxXmlDocument();
}

NSISGUIFile::~NSISGUIFile()
{
}

void NSISGUIFile::Save(NGFOptions opt)
{
    if (wxFileExists(opt.Name + _T(".ngf"))) wxRemoveFile(opt.Name + _T(".ngf"));

    wxXmlNode *Root = new wxXmlNode(wxXML_ELEMENT_NODE,_T("GeOROOT"));
    {
        CreateOption(Root,_T("Name"),opt.Name);
        CreateOption(Root,_T("OutputName"),opt.OutputName);
        CreateOptionInt(Root,_T("InstallPath"),opt.InstallPath);
        CreateOptionBool(Root,_T("Shortcut"),opt.Shortcut);
        CreateOptionBool(Root,_T("License"),opt.License);
        CreateOptionBool(Root,_T("Components"),opt.Components);
        CreateOptionBool(Root,_T("Directory"),opt.Directory);
        CreateOptionBool(Root,_T("ConfirmUI"),opt.ConfirmUI);
        CreateOptionBool(Root,_T("Uninstall"),opt.Uninstall);
        CreateOption(Root,_T("LicPath"),opt.LicPath);
        CreateOption(Root,_T("LicText"),opt.LicText);

        CreateArrayOption(Root,_T("InstallTypes"),opt.InstallTypes);
        CreateOptionCompItemArray(Root,_T("Items"),opt.Items);
        CreateArrayOption(Root,_T("Files"),opt.Files);
    }
    Ngf->SetRoot(Root);
    Ngf->Save(opt.Name + _T(".ngf"));
}

void NSISGUIFile::CreateOption(wxXmlNode* parent,wxString Option, wxString Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    TTemp->SetContent(Value);
    Temp->AddChild(TTemp);
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateOptionBool(wxXmlNode* parent,wxString Option, bool Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    wxString a;
    if (Value)a = _T("true");
    else a = _T("false");
    TTemp->SetContent(a);
    Temp->AddChild(TTemp);
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateOptionInt(wxXmlNode* parent,wxString Option, int Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    wxString a;
    a.Printf(_T("%i"),Value);
    TTemp->SetContent(a);
    Temp->AddChild(TTemp);
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateArrayOption(wxXmlNode* parent,wxString Option, wxArrayString Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    wxString a;
    a.Printf(_T("%i"),Value.GetCount());
    TTemp->SetContent(a);
    Temp->AddChild(TTemp);

    for (unsigned int i=0; i<Value.GetCount(); i++)
    {
        CreateOption(Temp,Option,Value[i]);
    }
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateArrayOptionInt(wxXmlNode* parent,wxString Option, wxArrayInt Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    wxString a;
    a.Printf(_T("%i"),Value.GetCount());
    TTemp->SetContent(a);
    Temp->AddChild(TTemp);

    for (unsigned int i=0; i<Value.GetCount(); i++)
    {
        CreateOptionInt(Temp,Option,Value[i]);
    }
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateOptionCompItem(wxXmlNode* parent,wxString Option,CompItem* Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    TTemp->SetContent(Option);
    Temp->AddChild(TTemp);
    {
        CreateOption(Temp,_T("m_Name"),Value->m_Name);
        CreateArrayOption(Temp,_T("m_Files"),Value->m_Files);
        CreateOptionBool(Temp,_T("m_Haschilds"),Value->m_Haschilds);
        CreateArrayOptionInt(Temp,_T("m_Sections"),Value->m_Sections);
        CreateOptionInt(Temp,_T("m_Checked"),Value->m_Checked);
        if (Value->m_Haschilds)
            for (unsigned int i=0; i<Value->m_Childs.GetCount(); i++)
                CreateOptionCompItem(Temp,_T("m_Childs"),Value->m_Childs[i]);
    }
    parent->AddChild(Temp);
}

void NSISGUIFile::CreateOptionCompItemArray(wxXmlNode* parent,wxString Option, CompItemArray Value)
{
    wxXmlNode *Temp = new wxXmlNode(wxXML_ELEMENT_NODE,Option);
    wxXmlNode *TTemp = new wxXmlNode(wxXML_TEXT_NODE,_T("GeOTC1"));
    TTemp->SetContent(Option);
    Temp->AddChild(TTemp);

    for (unsigned int i=0; i<Value.GetCount(); i++)
        CreateOptionCompItem(Temp,Option,Value[i]);
    parent->AddChild(Temp);
}

NGFOptions NSISGUIFile::Load(wxString File)
{
    Ngf->Load(File);
    NGFOptions opt;
    wxXmlNode* Root = Ngf->GetRoot();
    opt.Name = GetOption(Root,_T("Name"));
    opt.OutputName = GetOption(Root,_T("OutputName"));
    opt.InstallPath = GetOptionInt(Root,_T("InstallPath"));
    opt.Shortcut = GetOptionBool(Root,_T("Shortcut"));
    opt.License = GetOptionBool(Root,_T("License"));
    opt.Components = GetOptionBool(Root,_T("Components"));
    opt.Directory = GetOptionBool(Root,_T("Directory"));
    opt.ConfirmUI = GetOptionBool(Root,_T("ConfirmUI"));
    opt.Uninstall = GetOptionBool(Root,_T("Uninstall"));

    opt.LicPath = GetOption(Root,_T("LicPath"));
    opt.LicText = GetOption(Root,_T("LicText"));

    opt.InstallTypes = GetArrayOption(Root,_T("InstallTypes"));
    opt.Items = GetOptionCompItemArray(Root,_T("Items"));

    opt.Files = GetArrayOption(Root,_T("Files"));

    return opt;
}

wxString NSISGUIFile::GetOption(wxXmlNode* parent,wxString Option)
{

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            return child->GetNodeContent();
        }
        child = child->GetNext();
    }
    return wxEmptyString;
}

bool NSISGUIFile::GetOptionBool(wxXmlNode* parent,wxString Option)
{

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            if (child->GetNodeContent()==_T("true"))
                return true;
            else
                return false;
        }
        child = child->GetNext();
    }
    return false;
}

int NSISGUIFile::GetOptionInt(wxXmlNode* parent,wxString Option)
{

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            long i;
            child->GetNodeContent().ToLong(&i);
            return i;
        }
        child = child->GetNext();
    }
    return 0;
}

wxArrayString NSISGUIFile::GetArrayOption(wxXmlNode* parent,wxString Option)
{
    wxArrayString ar;

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            wxXmlNode *child2 = child->GetChildren();
            while (child2)
            {
                if (child2->GetNodeContent()!=wxEmptyString)
                    ar.Add(child2->GetNodeContent());
                child2 = child2->GetNext();
            }
            return ar;
        }
        child = child->GetNext();
    }
    return ar;
}

wxArrayInt NSISGUIFile::GetArrayOptionInt(wxXmlNode* parent,wxString Option)
{
    wxArrayInt ai;

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            wxXmlNode *child2 = child->GetChildren();
            while (child2)
            {
                if (child2->GetNodeContent()!=wxEmptyString)
                {
                    long i;
                    child2->GetNodeContent().ToLong(&i);
                    ai.Add(i);
                }
                child2 = child2->GetNext();
            }
            return ai;
        }
        child = child->GetNext();
    }
    return ai;
}

CompItem* NSISGUIFile::GetOptionCompItem(wxXmlNode* parent,wxString Option)
{
    CompItem* item = new CompItem();
    item->m_Name = GetOption(parent,_T("m_Name"));
    item->m_Files = GetArrayOption(parent,_T("m_Files"));
    item->m_Haschilds = GetOptionBool(parent,_T("m_Haschilds"));

    if (item->m_Haschilds)
    {
        wxXmlNode *child = parent->GetChildren();
        while (child)
        {
            if (child->GetName()==_T("m_Childs"))
                item->m_Childs.Add(GetOptionCompItem(child,Option));
            child = child->GetNext();
        }
    }
    item->m_Sections = GetArrayOptionInt(parent,_T("m_Sections"));
    item->m_Checked = GetOptionInt(parent,_T("m_Checked"));
    return item;
}

CompItemArray NSISGUIFile::GetOptionCompItemArray(wxXmlNode* parent,wxString Option)
{
    CompItemArray itar;

    wxXmlNode *child = parent->GetChildren();
    while (child)
    {

        if (child->GetName() == Option)
        {
            wxXmlNode *child2 = child->GetChildren();
            while (child2)
            {
                if (child2->GetNodeContent()!=wxEmptyString)
                    itar.Add(GetOptionCompItem(child2,Option));
                child2 = child2->GetNext();
            }
            return itar;
        }
        child = child->GetNext();
    }
    return itar;
}
