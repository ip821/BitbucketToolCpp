#pragma once

#include "SetupWizard.h"

class WorkspaceView;

struct ProcessedWorkspace
{
    Workspace m_workspace{};
    bool m_isProcessed{};
};

class wxStaticBox;
class wxActivityIndicator;

class WorkspacePage : public wxWizardPageSimple
{
    SetupWizard& m_wizard;
    SetupWizardContext& m_context;

    WorkspaceView* m_pWorkspaceView;

    bool m_asyncOperationInProgress = false;
    bool m_asyncOperationCompletedSuccessfully = false;

    void StartBusyAnimation();
    void StopBusyAnimation();

    void StartAsyncOperation();
public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
};
