#include "MenuBuilder.h"

#include <wx/menu.h>

MenuBuilder::MenuBuilder(wxMenu& menu, const int nextItemId) :
    m_menu(menu),
    m_nextItemId(nextItemId)
{
}

wxMenuItem* MenuBuilder::InsertItem(const wxString& label)
{
    return m_menu.Insert(m_insertIndex++, m_nextItemId++, label);
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
    m_menu.Insert(m_insertIndex++, m_nextItemId++, label, subMenu);
    return subMenu;
}

void MenuBuilder::InsertSeparator()
{
    m_menu.InsertSeparator(m_insertIndex++)->SetId(m_nextItemId++);
}

int MenuBuilder::GetLastUsedItemId() const
{
    return m_nextItemId;
}
