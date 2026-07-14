#pragma once

#include "WizardPages.h"
#include "bitbucket_api/Structs.h"

class RepositoryView : public RepositoryViewBase
{
public:
    explicit RepositoryView(wxWindow *parent);
    void SetRepositories(const std::vector<Repository>& repositories) const;
    unsigned int GetCheckedItems(wxArrayInt& checkedItemIndexes) const;
};
