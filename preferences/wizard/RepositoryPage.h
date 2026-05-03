//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H
#define WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H

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


#endif //WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H
