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
#include "SetupWizardContext.h"
#include "../../Constants.h"

LoginPage::LoginPage(wxWizard* pWindow, SetupWizardContext& context) :
    wxWizardPageSimple(pWindow),
    m_wizard(*pWindow),
    m_context(context),
    m_staticBox(*new wxStaticBox(this, wxID_ANY, wxT(""))),
    m_errorStaticText(*new wxStaticText(&m_staticBox, wxID_ANY, wxT(""))),
    m_activityIndicator(*CreateActivityIndicator(&m_staticBox)),
    m_loginTextCtrl(*new wxTextCtrl(&m_staticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                    wxTextValidator(wxFILTER_NONE, &m_email))),
    m_passwordTextCtrl(*new wxTextCtrl(&m_staticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD,
                                       wxTextValidator(wxFILTER_NONE, &m_password)))
{
    m_errorStaticText.SetWindowStyleFlag(wxALIGN_CENTER_HORIZONTAL);
    m_errorStaticText.SetForegroundColour(wxColour(255, 0, 0));
    m_errorStaticText.Hide();

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);

    const auto pStaticBox = &m_staticBox;
    m_activityIndicator.Hide();

    const auto pStaticBoxSizer = new wxStaticBoxSizer(pStaticBox, wxVERTICAL);

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);

    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Login")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(&m_loginTextCtrl,
                    wxSizerFlags().Left().Expand());
    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Password")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(&m_passwordTextCtrl,
                    wxSizerFlags().Left().Expand());

    pStaticBoxSizer->Add(pGridSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));
    pStaticBoxSizer->Add(&m_errorStaticText, wxSizerFlags().Expand().Border(wxALL, 15));
    pStaticBoxSizer->Add(&m_activityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 15));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

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
        wxSecretValue password(m_passwordTextCtrl.GetValue());
        store.Save(SecretStoreAppName, m_loginTextCtrl.GetValue(), password);
    }

    StartGetWorkspacesRequest();

    event.Veto();
}

wxActivityIndicator* LoginPage::CreateActivityIndicator(wxStaticBox* pStaticBox)
{
    const auto pLoader = new wxActivityIndicator(pStaticBox);
#ifdef __WXMSW__
    pLoader->SetDoubleBuffered(true);
#endif
    return pLoader;
}

void LoginPage::StartBusyAnimation()
{
    Disable();
    m_activityIndicator.Show();
    m_activityIndicator.Start();
    Layout();
}

void LoginPage::StopBusyAnimation()
{
    Enable();
    m_activityIndicator.Stop();
    m_activityIndicator.Hide();
    Layout();
}

void LoginPage::HideErrorMessage()
{
    m_errorStaticText.SetLabelText("");
    m_errorStaticText.Hide();
    Layout();
}

void LoginPage::ShowErrorMessage(const wxString& str)
{
    if (str.IsEmpty())
        return;

    m_errorStaticText.SetLabelText(str);
    m_errorStaticText.Show();
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
