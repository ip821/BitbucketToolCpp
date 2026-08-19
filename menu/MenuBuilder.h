#pragma once

#include <cstddef>

class wxMenu;
class wxMenuItem;
class wxString;

class MenuBuilder
{
public:
    MenuBuilder(wxMenu& menu, int nextItemId);
    MenuBuilder(wxMenu& menu, MenuBuilder& parent);

    wxMenuItem* InsertItem(const wxString& label);
    wxMenuItem* InsertDisabledItem(const wxString& label);
    wxMenu* InsertSubMenu(const wxString& label);
    void InsertSeparator();

private:
    wxMenu& m_menu;
    int m_nextItemId{};
    MenuBuilder* m_parent{};
    std::size_t m_insertIndex{};

    int TakeNextItemId();
};
