#include <wx/wx.h>
#include <wx/wizard.h>
#include <wx/secretstore.h>
#include <wx/base64.h>
#include <wx/webrequest.h>
#include <ranges>
#include <nlohmann/json.hpp>

#include "WorkspacePage.h"
#include "../../Constants.h"

WorkspacePage::WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_wizard(*pWizard),
    m_context(context),
    m_staticBox(*new wxStaticBox(this, wxID_ANY, wxT("")))
{
    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    const auto pStaticBoxSizer = new wxStaticBoxSizer(&m_staticBox, wxVERTICAL);

    const auto pListBox = new wxCheckListBox(this, wxID_ANY);

    pStaticBoxSizer->Add(pListBox, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

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
        {
            m_repositoriesFetched = false;
            return;
        }

        if (m_repositoriesFetched)
            return;

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
            StartRepositoriesRequest(workspace, m_workspaces.size() - 1);
        }

        event.Veto();
    });

    Bind(wxEVT_WEBREQUEST_STATE, [this](wxWebRequestEvent& event)
    {
        const auto& request = event.GetRequest();
        const auto& index = request.GetId();
        m_workspaces[index].m_isProcessed = true;
        if (std::ranges::all_of(m_workspaces, [](const auto& item) { return item.m_isProcessed; }))
        {
            const wxWebResponse& response = event.GetResponse();

            const auto strBody = response.AsString();
            const auto buffer = strBody.ToUTF8();
            constexpr auto pParserCallback = nullptr;
            constexpr auto allowExceptions = false;
            const auto jObject = nlohmann::json::parse(buffer.data(), buffer.data() + buffer.length(), pParserCallback,
                                                       allowExceptions);

            if (response.GetStatus() == 200)
            {
                m_context.m_repositories.clear();

                wxString repositoryNames;
                const auto jRepositories = jObject["values"];
                for (const auto& jRepository : jRepositories)
                {
                    const Repository& repository = {
                        jRepository["full_name"].get<std::string>(),
                        jRepository["slug"].get<std::string>()
                    };
                    m_context.m_repositories.push_back(repository);
                }

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
