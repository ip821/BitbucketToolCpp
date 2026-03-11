//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYWIZARD_H
#define WXWIDGETSPLAYGROUND_MYWIZARD_H

#include <wx/wizard.h>
#include <wx/activityindicator.h>
#include "SetupWizardContext.h"

class LoginPage;

class SetupWizard : public wxWizard
{
public:
    explicit SetupWizard(wxWindow* pWindow);
    void Run();

private:
    LoginPage* m_pLoginPage;
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

#endif //WXWIDGETSPLAYGROUND_MYWIZARD_H
