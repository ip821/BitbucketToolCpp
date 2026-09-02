#include "LoginPage.h"

#include <thread>
#include <utility>
#include <cpp_utils/match_expected.h>
#include <wx/wx.h>

#include "LoginView.h"
#include "LoginCompletedThreadEvent.h"
#include "SetupWizard.h"
#include "SetupWizardContext.h"
#include "../Credentials.h"
#include "bitbucket_api/Requests.h"

LoginPage::LoginPage(wxWizard *pWindow, SetupWizardContext& context) :
    wxWizardPageSimple(pWindow),
    m_wizard(*pWindow),
    m_context(context)
{
    m_pLoginView = new LoginView(this);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(m_pLoginView, wxSizerFlags().Expand().Proportion(1));
    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this](wxWizardEvent&)
    {
        m_asyncOperationCompletedSuccessfully = false;

        if (const auto result = Credentials::GetCredentials();
            result.has_value())
        {
            const auto& [strEmail, strPassword] = result.value();
            m_pLoginView->m_email = strEmail;
            m_pLoginView->m_password = strPassword;
        }

        m_pLoginView->TransferDataToWindow();
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this](wxWizardEvent& event)
    {
        if (m_asyncOperationCompletedSuccessfully)
            return;

        event.Veto();

        if (!m_asyncOperationInProgress)
            StartAsyncOperation();
    });

    Bind(LoginCompletedThreadEvent::EventType, &LoginPage::OnAsyncOperationCompleted, this);
}

void LoginPage::StartBusyAnimation()
{
    Disable();
    m_pLoginView->ShowActivityIndicator();
    Layout();
}

void LoginPage::StopBusyAnimation()
{
    Enable();
    m_pLoginView->HideActivityIndicator();
    Layout();
}

void LoginPage::HideErrorMessage()
{
    m_pLoginView->ClearErrorMessage();
    Layout();
}

void LoginPage::ShowErrorMessage(const wxString& str)
{
    if (str.IsEmpty())
        return;

    m_pLoginView->SetErrorMessage(str);
    Layout();
}

void LoginPage::StopAsyncOperation()
{
    m_thread.request_stop();
    if (m_thread.joinable())
        m_thread.join();
}

void LoginPage::StartAsyncOperation()
{
    if (m_asyncOperationInProgress)
        return;

    if (!m_pLoginView->TransferDataFromWindow())
        return;

    m_asyncOperationInProgress = true;
    StartBusyAnimation();

    HideErrorMessage();

    Credentials::SetCredentials(m_pLoginView->m_email, m_pLoginView->m_password);

    m_context.m_workspaces.clear();

    m_thread = std::jthread([this](const std::stop_token stopToken)
    {
        const auto credentials = Credentials::GetCredentialsBase64().ToStdString();

        const WorkspacesRequest workspacesRequest;
        auto response = workspacesRequest.GetWorkspaces(credentials);

        if (stopToken.stop_requested())
            return;

        const auto event = new LoginCompletedThreadEvent(stopToken, std::move(response));
        wxQueueEvent(this, event);
    });
}

void LoginPage::OnAsyncOperationCompleted(wxThreadEvent& event)
{
    const auto& loginEvent = static_cast<LoginCompletedThreadEvent&>(event);
    if (loginEvent.IsCancelled())
        return;

    StopBusyAnimation();
    m_asyncOperationInProgress = false;

    ip::match_expected(
        loginEvent.GetResponse(),
        [this](const auto& workspaces)
        {
            for (const auto& workspace: workspaces)
            {
                m_context.m_workspaces.push_back(workspace);
            }
            m_asyncOperationCompletedSuccessfully = true;
            m_wizard.ShowPage(GetNext());
        },
        [this](const auto& error)
        {
            ShowErrorMessage(error.message);
        }
    );
}
