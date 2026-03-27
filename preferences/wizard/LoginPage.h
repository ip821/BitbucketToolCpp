#pragma once

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

    bool m_asyncOperationInProgress = false;
    bool m_asyncOperationCompletedSuccessfully = false;

    wxStaticText* m_pErrorStaticText;
    wxActivityIndicator* m_pActivityIndicator;
    wxTextCtrl* m_pLoginTextCtrl;
    wxTextCtrl* m_pPasswordTextCtrl;

    wxString m_email;
    wxString m_password;

    void StartAsyncOperation();

    void HideErrorMessage();
    void ShowErrorMessage(const wxString& str);
    void StartBusyAnimation();
    void StopBusyAnimation();

public:
    explicit LoginPage(wxWizard* pWindow, SetupWizardContext& context);
};
