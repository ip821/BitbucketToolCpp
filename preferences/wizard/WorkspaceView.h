#pragma once

#include "WizardPages.h"
#include "bitbucket_api/Structs.h"

class WorkspaceView : public WorkspaceViewBase
{
public:
    explicit WorkspaceView(wxWindow *parent);
    void SetWorkspaces(const std::vector<Workspace>& workspaces) const;
    unsigned int GetCheckedItems(wxArrayInt& checkedItemsIndexes) const;
    void ShowActivityIndicator() const;
    void HideActivityIndicator() const;
};
