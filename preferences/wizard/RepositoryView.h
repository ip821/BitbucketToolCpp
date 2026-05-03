#pragma once

#include "WizardPages.h"
#include "../../webrequests/Repository.h"

class RepositoryView : public RepositoryViewBase
{
public:
    explicit RepositoryView(wxWindow *parent);
    void SetRepositories(const std::vector<Repository>& repositories) const;
    unsigned int GetCheckedItems(wxArrayInt& checkedItemIndexes) const;
};
