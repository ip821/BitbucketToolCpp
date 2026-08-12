#include "MenuBuilder.h"

#include <wx/menu.h>

MenuBuilder::MenuBuilder(wxMenu& menu, const int firstItemId) :
    m_menu(menu),
    m_nextItemId(firstItemId)
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

void MenuBuilder::InsertSeparator()
{
    m_menu.InsertSeparator(m_insertIndex++)->SetId(m_nextItemId++);
}

void MenuBuilder::StartNewColumnAtEnd()
{
    m_insertIndex = m_menu.GetMenuItemCount();
    m_menu.Break();
}
