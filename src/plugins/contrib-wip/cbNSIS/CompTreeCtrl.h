#ifndef COMPTREECTRL_H
#define COMPTREECTRL_H

#include "wx_pch.h"

#ifndef WX_PRECOMP
#include <wx/treectrl.h>
#endif

#include "CompMap.h"

class CompTreeCtrl : public wxTreeCtrl
{
public:
    CompTreeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTR_DEFAULT_STYLE,
                 const wxValidator &validator = wxDefaultValidator,
                 const wxString& name = wxTreeCtrlNameStr);
    virtual ~CompTreeCtrl();

    void AddComp(CompItem* item);
    void RemComp();

    void AddSection(int section);
    void RemoveSection(int section);
    void ChangeSection(int section);

    CompItemArray GetComponents();
    void SetComponents(CompItemArray itemarray);
protected:
private:
    class CompItemHolder: public wxTreeItemData
    {
    public:
        CompItemHolder(CompItem* Item): m_Item(Item) {}
        virtual ~CompItemHolder() {}
        CompItem* m_Item;
    };
    void ToggleImage(wxTreeItemId par, int value);
    void UpdateParent(wxTreeItemId child);
    void SetChecked(wxTreeItemId id, int value);

    void SetSection(int section, wxTreeItemId id);
    void RemSection(int section, wxTreeItemId id);
    void ChgSection(int section, wxTreeItemId id);

    void CreateChilds(wxTreeItemId parent, CompItemArray itemarray);

    void OnLeftDown(wxMouseEvent &event);

    DECLARE_EVENT_TABLE();
};

#endif // COMPTREECTRL_H
