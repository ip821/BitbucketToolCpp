#pragma once

#include <wx/checklst.h>
#include <wx/panel.h>

#include "bitbucket_api/Structs.h"

class RepositoryView : public wxPanel
{
    wxCheckListBox* m_pListBox;

public:
    explicit RepositoryView(wxWindow *parent);
    void SetRepositories(const std::vector<Repository>& repositories) const;
    unsigned int GetCheckedItems(wxArrayInt& checkedItemIndexes) const;
};
