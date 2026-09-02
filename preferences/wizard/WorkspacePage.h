#pragma once

#include <thread>

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
    friend class SetupWizard;

    SetupWizard& m_wizard;
    SetupWizardContext& m_context;

    WorkspaceView* m_pWorkspaceView;

    std::jthread m_thread;

    bool m_asyncOperationInProgress = false;
    bool m_asyncOperationCompletedSuccessfully = false;

    void StartBusyAnimation();
    void StopBusyAnimation();

    void StartAsyncOperation();
    void StopAsyncOperation();
    void OnAsyncOperationCompleted(wxThreadEvent& event);
public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
};
