#pragma once

#include <wx/checklst.h>
#include <wx/panel.h>

#include "bitbucket_api/Structs.h"

class wxActivityIndicator;

class WorkspaceView : public wxPanel
{
    wxCheckListBox* m_pListBox;
    wxActivityIndicator* m_pActivityIndicator;

public:
    explicit WorkspaceView(wxWindow *parent);
    void SetWorkspaces(const std::vector<Workspace>& workspaces) const;
    unsigned int GetCheckedItems(wxArrayInt& checkedItemsIndexes) const;
    void ShowActivityIndicator() const;
    void HideActivityIndicator() const;
};
