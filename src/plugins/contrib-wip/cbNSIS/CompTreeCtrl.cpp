#include "CompTreeCtrl.h"

#include "checkbox.xpm"

BEGIN_EVENT_TABLE(CompTreeCtrl,wxTreeCtrl)
    EVT_LEFT_DOWN(CompTreeCtrl::OnLeftDown)
END_EVENT_TABLE()

CompTreeCtrl::CompTreeCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,const wxValidator &validator,const wxString& name) : wxTreeCtrl(parent,id,pos,size,style,validator,name)
{
    wxImageList *list = new wxImageList(13, 13);
    list->Add(wxIcon(checkbox_xpm));
    list->Add(wxIcon(checkedbox_xpm));
    list->Add(wxIcon(checkedgraybox_xpm));
    AssignImageList(list);
}

CompTreeCtrl::~CompTreeCtrl()
{
}

void CompTreeCtrl::AddComp(CompItem* item)
{
    if (GetSelection())
    {
        if (GetItemParent(GetSelection()) == GetRootItem())
            AppendItem(GetSelection(),item->m_Name,0,-1,new CompItemHolder(item));
    }
    else
        AppendItem(GetRootItem(),item->m_Name,0,-1,new CompItemHolder(item));
}

void CompTreeCtrl::RemComp()
{
    Delete(GetSelection());
}

void CompTreeCtrl::CreateChilds(wxTreeItemId parent, CompItemArray itemarray)
{
    for (unsigned int i=0; i<itemarray.GetCount(); i++)
    {
        wxTreeItemId me = AppendItem(parent,itemarray[i]->m_Name,0,-1,new CompItemHolder(itemarray[i]));
        if(itemarray[i]->m_Checked)
        {
            SetItemImage(me,1);
            SetChecked(me,1);
            UpdateParent(me);
        }
        if(itemarray[i]->m_Haschilds)
            CreateChilds(me,itemarray[i]->m_Childs);
    }
}

void CompTreeCtrl::SetComponents(CompItemArray itemarray)
{
    wxTreeItemId root = GetRootItem();
    DeleteChildren(root);
    CreateChilds(root,itemarray);
}

CompItemArray CompTreeCtrl::GetComponents()
{
    CompItemArray comp;
    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetFirstChild(GetRootItem(),cookie);
    while (id.IsOk())
    {
        CompItem* item = ((CompItemHolder*)GetItemData(id))->m_Item;
        if (ItemHasChildren(id))
        {
            item->m_Haschilds = true;
            wxTreeItemIdValue cookie2;
            wxTreeItemId artistID = GetFirstChild(id, cookie2);
            while (artistID.IsOk())
            {
                item->m_Childs.Add(((CompItemHolder*)GetItemData(artistID))->m_Item);
                artistID = GetNextChild(id, cookie2);
            }
        }
        else
            item->m_Haschilds = false;

        comp.Add(item);
        id = GetNextChild(GetRootItem(), cookie);
    }
    return comp;
}

void CompTreeCtrl::ToggleImage(wxTreeItemId par, int value)
{
    SetItemImage(par,value);
    SetChecked(par,value);
    wxTreeItemIdValue cookie2;
    wxTreeItemId ID = GetFirstChild(par, cookie2);
    while (ID.IsOk())
    {
        SetItemImage(ID,value);
        SetChecked(ID,value);
        ID = GetNextChild(par, cookie2);
    }
}

void CompTreeCtrl::UpdateParent(wxTreeItemId child)
{
    wxTreeItemId par = GetItemParent(child);
    if (par!=GetRootItem())
    {
        unsigned int imgids = 0;
        wxTreeItemIdValue cookie2;
        wxTreeItemId ID = GetFirstChild(par, cookie2);
        while (ID.IsOk())
        {
            imgids += GetItemImage(ID);
            ID = GetNextChild(par, cookie2);
        }
        SetItemImage(par,2);
        SetChecked(par,2);
        if (imgids == 0)
        {
            SetItemImage(par,0);
            SetChecked(par,0);
        }
        if (imgids == GetChildrenCount(par))
        {
            SetItemImage(par,1);
            SetChecked(par,1);
        }
    }
}

void CompTreeCtrl::SetChecked(wxTreeItemId id,int value)
{
    CompItem* item = ((CompItemHolder*)GetItemData(id))->m_Item;
    item->m_Checked = value;
    SetItemData(id,new CompItemHolder(item));
}

void CompTreeCtrl::SetSection(int section, wxTreeItemId id)
{
    CompItem* item = ((CompItemHolder*)GetItemData(id))->m_Item;
    if (item->m_Checked == 1)
    {
        item->m_Sections.Add(section);
        SetItemData(id,new CompItemHolder(item));
    }
}

void CompTreeCtrl::AddSection(int section)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetFirstChild(GetRootItem(), cookie);
    while (id.IsOk())
    {
        wxTreeItemIdValue cookie2;
        wxTreeItemId id2 = GetFirstChild(id, cookie2);
        while (id2.IsOk())
        {
            SetSection(section,id2);
            id2 = GetNextChild(id, cookie2);
        }
        SetSection(section,id);
        id = GetNextChild(GetRootItem(), cookie);
    }
}

void CompTreeCtrl::RemSection(int section, wxTreeItemId id)
{
    CompItem* item = ((CompItemHolder*)GetItemData(id))->m_Item;
    for (unsigned int i=0; i<item->m_Sections.GetCount(); i++)
    {
        if (item->m_Sections[i] == section)
            item->m_Sections.RemoveAt(i);
    }
    SetItemData(id,new CompItemHolder(item));
}

void CompTreeCtrl::RemoveSection(int section)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetFirstChild(GetRootItem(), cookie);
    while (id.IsOk())
    {
        wxTreeItemIdValue cookie2;
        wxTreeItemId id2 = GetFirstChild(id, cookie2);
        while (id2.IsOk())
        {
            RemSection(section,id2);
            id2 = GetNextChild(id, cookie2);
        }
        RemSection(section,id);
        id = GetNextChild(GetRootItem(), cookie);
    }
}

void CompTreeCtrl::ChgSection(int section, wxTreeItemId id)
{
    CompItem* item = ((CompItemHolder*)GetItemData(id))->m_Item;
    for (unsigned int i=0; i<item->m_Sections.GetCount(); i++)
    {
        if (item->m_Sections[i] == section)
        {
            ToggleImage(id, 1);
            UpdateParent(id);
        }
    }
    SetItemData(id,new CompItemHolder(item));
}

void CompTreeCtrl::ChangeSection(int section)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetFirstChild(GetRootItem(), cookie);
    while (id.IsOk())
    {
        ToggleImage(id, 0);
        SetChecked(id,0);
        if (ItemHasChildren(id))
        {
            wxTreeItemIdValue cookie2;
            wxTreeItemId id2 = GetFirstChild(id, cookie2);
            while (id2.IsOk())
            {
                ToggleImage(id2, 0);
                SetChecked(id2,0);
                ChgSection(section,id2);
                id2 = GetNextChild(id, cookie2);
            }
        }
        else
        {
            ChgSection(section,id);
        }
        id = GetNextChild(GetRootItem(), cookie);
    }
}

void CompTreeCtrl::OnLeftDown(wxMouseEvent &event)
{
    int flags;
    wxTreeItemId id = HitTest(event.GetPosition(), flags);
    if (flags & wxTREE_HITTEST_ONITEMICON)
    {
        if (GetItemImage(id) == 0)
            ToggleImage(id, 1);
        else
            ToggleImage(id, 0);
        UpdateParent(id);
    }
    event.Skip();
}
