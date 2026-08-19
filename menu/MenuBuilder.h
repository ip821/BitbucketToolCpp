#pragma once

#include <cstddef>

class wxMenu;
class wxMenuItem;
class wxString;

class MenuBuilder
{
public:
    MenuBuilder(wxMenu& menu, int nextItemId);

    wxMenuItem* InsertItem(const wxString& label);
    wxMenuItem* InsertDisabledItem(const wxString& label);
    wxMenu* InsertSubMenu(const wxString& label);
    void InsertSeparator();

    [[nodiscard]] int GetLastUsedItemId() const;

private:
    wxMenu& m_menu;
    int m_nextItemId;
    std::size_t m_insertIndex{};
};
