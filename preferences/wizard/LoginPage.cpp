#include <thread>

#include <wx/wx.h>
#include <wx/activityindicator.h>
#include <wx/webrequest.h>
#include <nlohmann/json.hpp>
#include <cpp_utils/match_expected.h>

#include "LoginPage.h"
#include "LoginView.h"

#include "SetupWizard.h"
#include "SetupWizardContext.h"
#include "../Credentials.h"
#include "../../webrequests/WorkspacesRequest.h"

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

    wxWeakRef isWindowValid(this);
    m_thread = std::jthread([isWindowValid, this]
    {
        const WorkspacesRequest workspacesRequest;
        const auto response = workspacesRequest.GetWorkspaces();

        wxTheApp->CallAfter([isWindowValid, response, this]
        {
            if (!isWindowValid)
                return;

            StopBusyAnimation();
            m_asyncOperationInProgress = false;

            ip::match_expected(
                response,
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
        });
    });
}
