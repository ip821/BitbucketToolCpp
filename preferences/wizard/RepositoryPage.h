//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H
#define WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H

struct SetupWizardContext;
class wxStaticBox;
class SetupWizard;

class RepositoryPage : public wxWizardPageSimple
{
    //SetupWizard& m_wizard;
    SetupWizardContext& m_context;
    wxStaticBox& m_staticBox;

public:
    explicit RepositoryPage(SetupWizard* pWizard, SetupWizardContext& context);
};


#endif //WXWIDGETSPLAYGROUND_REPOSITORYPAGE_H
