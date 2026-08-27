#pragma once

#include <cstddef>

class wxMenu;
class wxMenuItem;
class wxString;

class MenuBuilder
{
public:
    explicit MenuBuilder(wxMenu& menu);

    wxMenuItem* InsertItem(const wxString& label);
    wxMenuItem* InsertDisabledItem(const wxString& label);
    wxMenu* InsertSubMenu(const wxString& label);
    void InsertSeparator();

private:
    wxMenu& m_menu;
    std::size_t m_insertIndex{};
};
