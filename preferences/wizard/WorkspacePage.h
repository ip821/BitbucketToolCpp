//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H
#define WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H

#include "SetupWizard.h"

struct ProcessedWorkspace
{
    Workspace m_workspace;
    bool m_isProcessed = false;
};

class wxStaticBox;

class WorkspacePage : public wxWizardPageSimple
{
    SetupWizard& m_wizard;
    SetupWizardContext& m_context;
    wxStaticBox& m_staticBox;

    std::vector<ProcessedWorkspace> m_workspaces;
    bool m_repositoriesFetched = false;

public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
    void StartRepositoriesRequest(const Workspace& workspace, std::size_t index);
};


#endif //WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H
