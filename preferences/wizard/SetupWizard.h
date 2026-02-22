//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYWIZARD_H
#define WXWIDGETSPLAYGROUND_MYWIZARD_H

#include "SetupWizardContext.h"
#include "../../stdafx.h"

class LoginPage;

class SetupWizard : public wxWizard
{
public:
    explicit SetupWizard(wxWindow* pWindow);
    void Run();

private:
    LoginPage* m_pLoginPage;
    SetupWizardContext m_context;

    wxDECLARE_EVENT_TABLE();
};


#endif //WXWIDGETSPLAYGROUND_MYWIZARD_H
