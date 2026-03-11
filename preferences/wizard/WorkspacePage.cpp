#include <format>
#include <nlohmann/json.hpp>
#include <ranges>
#include <wx/activityindicator.h>
#include <wx/base64.h>
#include <wx/secretstore.h>
#include <wx/webrequest.h>
#include <wx/wizard.h>
#include <wx/wx.h>

#include "../../Constants.h"
#include "WorkspacePage.h"

WorkspacePage::WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_wizard(*pWizard),
    m_context(context)
{
    m_pActivityIndicator = CreateActivityIndicator(this);
    m_pActivityIndicator->Hide();

    const auto pListBox = new wxCheckListBox(this, wxID_ANY);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pListBox, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
    pMainSizer->Add(m_pActivityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 5));

    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this, pListBox](wxWizardEvent&)
    {
        pListBox->Clear();

        for (const auto& ws : m_context.m_workspaces)
        {
            pListBox->Append(ws.m_name);
        }
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [pListBox, this](wxWizardEvent& event)
    {
        if (!event.GetDirection())
            return;

        if (m_repositoriesFetched)
        {
            m_repositoriesFetched = false;
            return;
        }

        if (!m_workspaces.empty())
        {
            event.Veto();
            return;
        }

        wxArrayInt checkedItemIndexes;
        if (const auto checkedCount = pListBox->GetCheckedItems(checkedItemIndexes);
            !checkedCount)
        {
            event.Veto();
            return;
        }

        for (const auto& index : checkedItemIndexes)
        {
            const auto& workspace = m_context.m_workspaces.at(index);
            m_workspaces.push_back({workspace});
        }

        m_context.m_repositories.clear();

        auto index = 0;
        for (const auto& workspace : m_workspaces)
        {
            StartRepositoriesRequest(workspace.m_workspace, index);
            index++;
        }

        StartBusyAnimation();

        event.Veto();
    });

    Bind(wxEVT_WEBREQUEST_STATE, [this](wxWebRequestEvent& event)
    {
        if (event.GetState() != wxWebRequest::State_Completed)
            return;

        const wxWebResponse& response = event.GetResponse();

        if (response.GetStatus() == 200)
        {
            const auto strBody = response.AsString();
            const auto buffer = strBody.ToUTF8();
            constexpr auto pParserCallback = nullptr;
            constexpr auto allowExceptions = false;
            const auto jObject = nlohmann::json::parse(buffer.data(), buffer.data() + buffer.length(),
                                                       pParserCallback,
                                                       allowExceptions);

            wxString repositoryNames;
            const auto& jRepositories = jObject["values"];
            for (const auto& jRepository : jRepositories)
            {
                const Repository& repository = {
                    jRepository["full_name"].get<std::string>(),
                    jRepository["slug"].get<std::string>()
                };
                m_context.m_repositories.push_back(repository);
            }

            const auto& request = event.GetRequest();
            const auto& index = request.GetId();

            m_workspaces[index].m_isProcessed = true;
            if (std::ranges::all_of(m_workspaces, [](const auto& item) { return item.m_isProcessed; }))
            {
                m_workspaces.clear();
                StopBusyAnimation();
                m_repositoriesFetched = true;
                m_wizard.ShowPage(GetNext());
            }
        }
    });
}

void WorkspacePage::StartRepositoriesRequest(const Workspace& workspace, std::size_t index)
{
    wxString strEmail;
    wxSecretValue strPassword;

    const auto store = wxSecretStore::GetDefault();
    if (store.IsOk())
    {
        if (!store.Load(SecretStoreAppName, strEmail, strPassword))
            return;
    }

    const wxString auth = strEmail + ":" + strPassword.GetAsString();
    const wxCharBuffer utf8 = auth.ToUTF8();
    const wxString encoded = wxBase64Encode(utf8.data(), utf8.length());

    const std::string strFormattedUrl = std::format("https://api.bitbucket.org/2.0/repositories/{}/",
                                                    workspace.m_slug.ToStdString());

    wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, wxString::FromUTF8(strFormattedUrl), index);

    request.SetHeader("Authorization", "Basic " + encoded);
    request.SetHeader("Accept", "application/json");

    request.Start();
}

void WorkspacePage::StartBusyAnimation()
{
    Disable();
    m_pActivityIndicator->Show();
    m_pActivityIndicator->Start();
    Layout();
}

void WorkspacePage::StopBusyAnimation()
{
    Enable();
    m_pActivityIndicator->Stop();
    m_pActivityIndicator->Hide();
    Layout();
}
