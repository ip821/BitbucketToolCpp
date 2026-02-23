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
class wxActivityIndicator;

class WorkspacePage : public wxWizardPageSimple
{
    SetupWizard& m_wizard;
    SetupWizardContext& m_context;
    wxStaticBox& m_staticBox;
    wxActivityIndicator& m_activityIndicator;

    std::vector<ProcessedWorkspace> m_workspaces;
    bool m_repositoriesFetched = false;

    static wxActivityIndicator* CreateActivityIndicator(wxStaticBox* pStaticBox);
    void StartBusyAnimation();
    void StopBusyAnimation();

public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
    void StartRepositoriesRequest(const Workspace& workspace, std::size_t index);
};


#endif //WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H
