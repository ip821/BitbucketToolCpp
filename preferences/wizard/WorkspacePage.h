#pragma once

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

    wxCheckListBox* m_pListBox;

    wxActivityIndicator* m_pActivityIndicator;

    bool m_asyncOperationInProgress = false;
    bool m_asyncOperationCompletedSuccessfully = false;

    void StartBusyAnimation();
    void StopBusyAnimation();

    void StartAsyncOperation();
public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
};
