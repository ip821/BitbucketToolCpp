#pragma once

#include <wx/wizard.h>
#include <wx/activityindicator.h>

#include "SetupWizardContext.h"

class LoginPage;
class WorkspacePage;

class SetupWizard : public wxWizard
{
public:
    explicit SetupWizard(wxWindow* pWindow);
    ~SetupWizard() override;
    void Run();

private:
    LoginPage* m_pLoginPage;
    WorkspacePage* m_pWorkspacePage;
    SetupWizardContext m_context;
};

inline wxActivityIndicator* CreateActivityIndicator(wxWindow* pWindow)
{
    const auto pLoader = new wxActivityIndicator(pWindow);
#ifdef __WXMSW__
    pLoader->SetDoubleBuffered(true);
#endif
    return pLoader;
}
