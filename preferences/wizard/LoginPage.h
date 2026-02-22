//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_LOGINPAGE_H
#define WXWIDGETSPLAYGROUND_LOGINPAGE_H

#include "../../stdafx.h"

class wxWebRequestEvent;
class wxActivityIndicator;

class LoginPage : public wxWizardPageSimple
{
    friend class SetupWizard;
    bool m_loginInProgress = false;
    bool m_loginCompleted = false;

    wxStaticBox& m_staticBox;
    wxStaticText& m_errorText;
    wxActivityIndicator& m_activityIndicator;
    wxTextCtrl& m_loginText;
    wxTextCtrl& m_passwordText;

    static wxActivityIndicator* CreateActivityIndicator(wxStaticBox *pStaticBox);
    void OnGetWorkspacesCompleted(wxWebRequestEvent& event);
    void StartGetWorkspaces();

    void HideErrorMessage();
    void ShowErrorMessage(const wxString& str);
    void StartBusyAnimation();
    void StopBusyAnimation();

public:
    explicit LoginPage(wxWizard* pWindow);
};


#endif //WXWIDGETSPLAYGROUND_LOGINPAGE_H
