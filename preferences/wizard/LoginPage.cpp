//
// Created by Igor Palkin on 27.12.2025.
//

#include <wx/wx.h>
#include <wx/activityindicator.h>
#include <wx/secretstore.h>
#include <wx/base64.h>
#include <wx/webrequest.h>
#include <nlohmann/json.hpp>

#include "LoginPage.h"

#include "SetupWizard.h"
#include "SetupWizardContext.h"
#include "../../Constants.h"

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

    Bind(wxEVT_WEBREQUEST_STATE, &LoginPage::OnGetWorkspacesRequestStateChanged, this);
    Bind(wxEVT_WIZARD_PAGE_CHANGING, &LoginPage::OnPageChanging, this);
    Bind(wxEVT_WIZARD_PAGE_SHOWN, &LoginPage::OnPageShown, this);
}

void LoginPage::OnPageShown(wxWizardEvent& event)
{
    if (!event.GetDirection())
        return;

    const auto store = wxSecretStore::GetDefault();
    if (store.IsOk())
    {
        wxString strUsername;
        wxSecretValue strPassword;
        if (store.Load(SecretStoreAppName, strUsername, strPassword))
        {
            m_email = strUsername;
            m_password = strPassword.GetAsString();
        }
    }
    TransferDataToWindow();
}

void LoginPage::OnPageChanging(wxWizardEvent& event)
{
    if (!event.GetDirection()
        || m_loginInProgress
    )
        return;

    if (m_loginCompleted)
    {
        m_loginCompleted = false;
        return;
    }

    StartBusyAnimation();

    auto store = wxSecretStore::GetDefault();
    if (store.IsOk())
    {
        wxSecretValue password(m_pPasswordTextCtrl->GetValue());
        store.Save(SecretStoreAppName, m_pLoginTextCtrl->GetValue(), password);
    }

    StartGetWorkspacesRequest();

    event.Veto();
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

void LoginPage::StartGetWorkspacesRequest()
{
    if (m_loginInProgress)
        return;

    HideErrorMessage();

    m_loginInProgress = true;

    if (!TransferDataFromWindow())
        return;

    const wxString email = m_email;
    const wxString appPassword = m_password;

    const wxString auth = email + ":" + appPassword;
    const wxCharBuffer utf8 = auth.ToUTF8();
    const wxString encoded = wxBase64Encode(utf8.data(), utf8.length());

    wxWebRequest request =
        wxWebSession::GetDefault().CreateRequest(
            this,
            "https://api.bitbucket.org/2.0/workspaces"
        );

    request.SetHeader("Authorization", "Basic " + encoded);
    request.SetHeader("Accept", "application/json");

    request.Start();
}

void LoginPage::OnGetWorkspacesRequestStateChanged(wxWebRequestEvent& event)
{
    auto state = event.GetState();
    switch (state)
    {
    case wxWebRequest::State_Completed:
        break;
    case wxWebRequest::State_Unauthorized:
        ShowErrorMessage(wxT("Authorization failed"));
        break;
    case wxWebRequest::State_Failed:
        ShowErrorMessage(event.GetErrorDescription());
        break;
    case wxWebRequest::State_Cancelled:
        ShowErrorMessage(wxT("Request cancelled"));
        break;
    case wxWebRequest::State_Idle:
        return;
    case wxWebRequest::State_Active:
        return;
    }

    StopBusyAnimation();
    m_loginInProgress = false;

    if (state != wxWebRequest::State_Completed)
        return;

    const wxWebResponse& response = event.GetResponse();

    const auto strBody = response.AsString();
    const auto buffer = strBody.ToUTF8();
    constexpr auto pParserCallback = nullptr;
    constexpr auto allowExceptions = false;
    const auto jObject = nlohmann::json::parse(buffer.data(), buffer.data() + buffer.length(), pParserCallback,
                                               allowExceptions);

    if (response.GetStatus() == 200)
    {
        m_context.m_workspaces.clear();

        wxString workspaceNames;
        auto jWorkspaces = jObject["values"];
        for (const auto& jWorkspace : jWorkspaces)
        {
            const Workspace& workspace = {
                jWorkspace["name"].get<std::string>(),
                jWorkspace["slug"].get<std::string>()
            };
            m_context.m_workspaces.push_back(workspace);
        }
        m_loginCompleted = true;
        m_wizard.ShowPage(GetNext());
    }
    else
    {
        ShowErrorMessage(strBody);
    }
}
