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
    bool m_loginCompleted = false;
    LoginThread* m_pLoginThread = nullptr;

    wxStaticBox& m_staticBox;
    wxActivityIndicator& m_activityIndicator;

    static wxActivityIndicator* CreateActivityIndicator(wxStaticBox *pStaticBox);
    void OnLoginCompleted(wxThreadEvent& event);

public:
    explicit LoginPage(wxWizard* pWindow);
};


#endif //WXWIDGETSPLAYGROUND_LOGINPAGE_H
