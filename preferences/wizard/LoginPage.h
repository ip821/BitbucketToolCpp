#pragma once

#include <thread>
#include <wx/wizard.h>

class LoginView;
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

    LoginView* m_pLoginView;

    std::jthread m_thread;

    void StartAsyncOperation();
    void StopAsyncOperation();

    void HideErrorMessage();
    void ShowErrorMessage(const wxString& str);
    void StartBusyAnimation();
    void StopBusyAnimation();

public:
    explicit LoginPage(wxWizard* pWindow, SetupWizardContext& context);
};
