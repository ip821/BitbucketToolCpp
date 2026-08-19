#include "MenuBuilder.h"

#include <wx/menu.h>

MenuBuilder::MenuBuilder(wxMenu& menu, const int nextItemId) :
    m_menu(menu),
    m_nextItemId(nextItemId)
{
}

MenuBuilder::MenuBuilder(wxMenu& menu, MenuBuilder& parent) :
    m_menu(menu),
    m_parent(&parent)
{
}

wxMenuItem* MenuBuilder::InsertItem(const wxString& label)
{
    return m_menu.Insert(m_insertIndex++, TakeNextItemId(), label);
}

wxMenuItem* MenuBuilder::InsertDisabledItem(const wxString& label)
{
    auto* item = InsertItem(label);
    item->Enable(false);
    return item;
}

wxMenu* MenuBuilder::InsertSubMenu(const wxString& label)
{
    auto* subMenu = new wxMenu();
    m_menu.Insert(m_insertIndex++, TakeNextItemId(), label, subMenu);
    return subMenu;
}

void MenuBuilder::InsertSeparator()
{
    m_menu.InsertSeparator(m_insertIndex++)->SetId(TakeNextItemId());
}

int MenuBuilder::TakeNextItemId()
{
    if (m_parent)
        return m_parent->TakeNextItemId();

    return m_nextItemId++;
}
