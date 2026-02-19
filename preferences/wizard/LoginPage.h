//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_LOGINPAGE_H
#define WXWIDGETSPLAYGROUND_LOGINPAGE_H

#include "LoginThread.h"
#include "../../stdafx.h"

class wxActivityIndicator;

class LoginPage : public wxWizardPageSimple
{
    friend class SetupWizard;
private:
    bool m_loginCompleted = false;
    LoginThread* m_pLoginThread;
    wxActivityIndicator* m_pLoader;

    void OnLoginCompleted(wxThreadEvent& event);
public:
    explicit LoginPage(wxWizard* pWindow);
};


#endif //WXWIDGETSPLAYGROUND_LOGINPAGE_H
