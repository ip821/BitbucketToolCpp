//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_LOGINPAGE_H
#define WXWIDGETSPLAYGROUND_LOGINPAGE_H

#include <wx/wizard.h>

class wxTextCtrl;
class wxStaticText;
class wxStaticBox;
struct SetupWizardContext;
class wxWebRequestEvent;
class wxActivityIndicator;

class LoginPage : public wxWizardPageSimple
{
    friend class SetupWizard;

    wxWizard& m_wizard;
    SetupWizardContext& m_context;

    bool m_loginInProgress = false;
    bool m_loginCompleted = false;

    wxStaticBox& m_staticBox;
    wxStaticText& m_errorStaticText;
    wxActivityIndicator& m_activityIndicator;
    wxTextCtrl& m_loginTextCtrl;
    wxTextCtrl& m_passwordTextCtrl;

    wxString m_email;
    wxString m_password;

    static wxActivityIndicator* CreateActivityIndicator(wxStaticBox *pStaticBox);

    void OnGetWorkspacesRequestStateChanged(wxWebRequestEvent& event);
    void OnPageChanging(wxWizardEvent& event);
    void OnPageShown(wxWizardEvent& event);

    void StartGetWorkspacesRequest();

    void HideErrorMessage();
    void ShowErrorMessage(const wxString& str);
    void StartBusyAnimation();
    void StopBusyAnimation();

public:
    explicit LoginPage(wxWizard* pWindow, SetupWizardContext& context);
};


#endif //WXWIDGETSPLAYGROUND_LOGINPAGE_H
