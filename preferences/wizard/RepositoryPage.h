#pragma once

class RepositoryView;
struct SetupWizardContext;
class SetupWizard;

class RepositoryPage : public wxWizardPageSimple
{
    SetupWizardContext& m_context;
    RepositoryView* m_pRepositoryView;

public:
    explicit RepositoryPage(SetupWizard* pWizard, SetupWizardContext& context);
};
