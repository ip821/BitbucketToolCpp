//
// Created by Igor Palkin on 27.12.2025.
//

#include <wx/wx.h>
#include <wx/activityindicator.h>
#include <wx/webrequest.h>
#include <nlohmann/json.hpp>

#include "LoginPage.h"

#include <thread>

#include "SetupWizard.h"
#include "SetupWizardContext.h"
#include "../Credentials.h"
#include "../../Switch.h"
#include "../../webrequests/CurlConnection.h"
#include "../../webrequests/WorkspacesRequest.h"

LoginPage::LoginPage(wxWizard* pWindow, SetupWizardContext& context) :
    wxWizardPageSimple(pWindow),
    m_wizard(*pWindow),
    m_context(context)
{
    m_pLoginTextCtrl = new wxTextCtrl(this, wxID_ANY);
    m_pLoginTextCtrl->SetValidator(wxTextValidator(wxFILTER_NONE, &m_email));

    m_pPasswordTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_pPasswordTextCtrl->SetValidator(wxTextValidator(wxFILTER_NONE, &m_password));

    m_pErrorStaticText = new wxStaticText(this, wxID_ANY, wxT(""));
    m_pErrorStaticText->SetWindowStyleFlag(wxALIGN_CENTER_HORIZONTAL);
    m_pErrorStaticText->SetForegroundColour(wxColour(255, 0, 0));
    m_pErrorStaticText->Hide();

    m_pActivityIndicator = CreateActivityIndicator(this);
    m_pActivityIndicator->Hide();

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Login")), wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(m_pLoginTextCtrl, wxSizerFlags().Left().Expand());
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Password")), wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(m_pPasswordTextCtrl, wxSizerFlags().Left().Expand());

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pGridSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));
    pMainSizer->Add(m_pErrorStaticText, wxSizerFlags().Expand().Border(wxALL, 15));
    pMainSizer->Add(m_pActivityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 15));

    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this](wxWizardEvent&)
    {
        m_asyncOperationCompletedSuccessfully = false;

        if (const auto result = Credentials::GetCredentials();
            result.has_value())
        {
            const auto& [strEmail, strPassword] = result.value();
            m_email = strEmail;
            m_password = strPassword;
        }

        TransferDataToWindow();
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
    m_pActivityIndicator->Show();
    m_pActivityIndicator->Start();
    Layout();
}

void LoginPage::StopBusyAnimation()
{
    Enable();
    m_pActivityIndicator->Stop();
    m_pActivityIndicator->Hide();
    Layout();
}

void LoginPage::HideErrorMessage()
{
    m_pErrorStaticText->SetLabelText("");
    m_pErrorStaticText->Hide();
    Layout();
}

void LoginPage::ShowErrorMessage(const wxString& str)
{
    if (str.IsEmpty())
        return;

    m_pErrorStaticText->SetLabelText(str);
    m_pErrorStaticText->Show();
    Layout();
}

void LoginPage::StartAsyncOperation()
{
    if (m_asyncOperationInProgress)
        return;

    if (!TransferDataFromWindow())
        return;

    m_asyncOperationInProgress = true;
    StartBusyAnimation();

    HideErrorMessage();

    Credentials::SetCredentials(m_email, m_password);

    m_context.m_workspaces.clear();

    wxWeakRef weakThis(this);
    std::thread([weakThis, this]
    {
        const CurlConnection connection;
        WorkspacesRequest workspacesRequest(connection);
        const auto response = workspacesRequest.GetWorkspaces();

        CallAfter([weakThis, response, this]
        {
            if (!weakThis)
                return;

            StopBusyAnimation();
            m_asyncOperationInProgress = false;

            Match(response,
                  [this](const WorkspacesSuccess& success)
                  {
                      for (const auto& workspace : success.workspaces)
                      {
                          m_context.m_workspaces.push_back(workspace);
                      }
                      m_asyncOperationCompletedSuccessfully = true;
                      m_wizard.ShowPage(GetNext());
                  },
                  [this](const Error& error)
                  {
                      ShowErrorMessage(error.message);
                  }
                );
        });
    }).detach();
}
