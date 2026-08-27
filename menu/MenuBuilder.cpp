#include "MenuBuilder.h"

#include <wx/menu.h>

MenuBuilder::MenuBuilder(wxMenu& menu) :
    m_menu(menu)
{
}

wxMenuItem* MenuBuilder::InsertItem(const wxString& label)
{
    return m_menu.Insert(m_insertIndex++, wxID_ANY, label);
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
    m_menu.Insert(m_insertIndex++, wxID_ANY, label, subMenu);
    return subMenu;
}

void MenuBuilder::InsertSeparator()
{
    m_menu.InsertSeparator(m_insertIndex++);
}
