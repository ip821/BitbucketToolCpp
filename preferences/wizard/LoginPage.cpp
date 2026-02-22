//
// Created by Igor Palkin on 27.12.2025.
//

#include "../../stdafx.h"
#include "LoginPage.h"
#include <wx/activityindicator.h>
#include <wx/wizard.h>
#include <wx/webrequest.h>
#include "wx/base64.h"
#include <nlohmann/json.hpp>

const auto AppName = wxString("PRToolForBitbucketCpp");

LoginPage::LoginPage(wxWizard* pWindow) :
    wxWizardPageSimple(pWindow),
    m_staticBox(*new wxStaticBox(this, wxID_ANY, wxT(""))),
    m_errorText(*new wxStaticText(&m_staticBox, wxID_ANY, wxT(""))),
    m_activityIndicator(*CreateActivityIndicator(&m_staticBox)),
    m_loginText(*new wxTextCtrl(&m_staticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)),
    m_passwordText(*new wxTextCtrl(&m_staticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD))
{
    m_errorText.SetWindowStyleFlag(wxALIGN_CENTER_HORIZONTAL);
    m_errorText.SetForegroundColour(wxColour(255, 0, 0));
    m_errorText.Hide();

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);

    const auto pStaticBox = &m_staticBox;
    m_activityIndicator.Hide();

    const auto pStaticBoxSizer = new wxStaticBoxSizer(pStaticBox, wxVERTICAL);

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);

    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Login")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(&m_loginText,
                    wxSizerFlags().Left().Expand());
    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Password")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(&m_passwordText,
                    wxSizerFlags().Left().Expand());

    pStaticBoxSizer->Add(pGridSizer, wxSizerFlags().Expand().Border(wxALL, 15));
    pStaticBoxSizer->Add(&m_activityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 15));
    pStaticBoxSizer->Add(&m_errorText, wxSizerFlags(1).Expand().Border(wxALL, 15));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

    Bind(wxEVT_WEBREQUEST_STATE, &LoginPage::OnGetWorkspacesCompleted, this);

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this](wxWizardEvent& event)
    {
        if (!event.GetDirection()
            || m_loginCompleted
            || m_loginInProgress
        )
            return;

        auto store = wxSecretStore::GetDefault();
        if (store.IsOk())
        {
            wxSecretValue password(m_passwordText.GetValue());
            store.Save(AppName, m_loginText.GetValue(), password);
        }

        event.Veto();
        StartBusyAnimation();
        StartGetWorkspaces();
    });

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this](wxWizardEvent&)
    {
        const auto store = wxSecretStore::GetDefault();
        if (store.IsOk())
        {
            wxString strUsername;
            wxSecretValue strPassword;
            if (store.Load(AppName, strUsername, strPassword))
            {
                m_loginText.SetValue(strUsername);
                m_passwordText.SetValue(strPassword.GetAsString());
            }
        }
    });
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
    m_activityIndicator.Show();
    m_activityIndicator.Start();
    Layout();
}

void LoginPage::StopBusyAnimation()
{
    m_activityIndicator.Stop();
    m_activityIndicator.Hide();
    Layout();
}

void LoginPage::HideErrorMessage()
{
    m_errorText.SetLabelText("");
    m_errorText.Hide();
    Layout();
}

void LoginPage::ShowErrorMessage(const wxString& str)
{
    if (str.IsEmpty())
        return;

    m_errorText.SetLabelText(str);
    m_errorText.Show();
    Layout();
}

void LoginPage::StartGetWorkspaces()
{
    if (m_loginInProgress)
        return;

    HideErrorMessage();
    m_loginInProgress = true;

    const wxString email = m_loginText.GetValue();
    const wxString appPassword = m_passwordText.GetValue();

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

void LoginPage::OnGetWorkspacesCompleted(wxWebRequestEvent& event)
{
    StopBusyAnimation();
    m_loginInProgress = false;

    switch (event.GetState())
    {
    case wxWebRequest::State_Completed:
        break;
    case wxWebRequest::State_Unauthorized:
        ShowErrorMessage(wxT("Authorization failed"));
        return;
    case wxWebRequest::State_Failed:
        ShowErrorMessage(event.GetErrorDescription());
        break;
    case wxWebRequest::State_Cancelled:
        ShowErrorMessage(wxT("Request cancelled"));
        return;
    case wxWebRequest::State_Idle:
        return;
    case wxWebRequest::State_Active:
        return;
    }

    const wxWebResponse& response = event.GetResponse();

    const auto strBody = response.AsString();
    const auto buffer = strBody.ToUTF8();
    constexpr auto pParserCallback = nullptr;
    constexpr auto allowExceptions = false;
    const auto jObject = nlohmann::json::parse(buffer.data(), buffer.data() + buffer.length(), pParserCallback, allowExceptions);

    if (response.GetStatus() == 200)
    {
        wxString workspaceNames;
        auto jWorkspaces = jObject["values"];
        for (const auto& jWorkspace : jWorkspaces)
        {
            auto jName = jWorkspace["name"];
            workspaceNames += jName.get<std::string>();
            workspaceNames += "\n";
        }
        wxLogMessage("Workspaces JSON:\n%s", workspaceNames);
        m_loginCompleted = true;
    }
    else
    {
        ShowErrorMessage(strBody);
    }
}
