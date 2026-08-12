#pragma once

#include <cstddef>

class wxMenu;
class wxMenuItem;
class wxString;

class MenuBuilder
{
public:
    MenuBuilder(wxMenu& menu, int firstItemId);

    wxMenuItem* InsertItem(const wxString& label);
    wxMenuItem* InsertDisabledItem(const wxString& label);
    void InsertSeparator();
    void StartNewColumnAtEnd();

private:
    wxMenu& m_menu;
    int m_nextItemId;
    std::size_t m_insertIndex{};
};
