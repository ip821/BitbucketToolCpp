#include <format>
#include <nlohmann/json.hpp>
#include <wx/activityindicator.h>
#include <wx/base64.h>
#include <wx/webrequest.h>
#include <wx/wizard.h>
#include <wx/wx.h>

#include "WorkspacePage.h"

#include <thread>

#include "../../Switch.h"
#include "../../curl/CurlConnection.h"
#include "../../webrequests/RepositoriesRequest.h"

WorkspacePage::WorkspacePage(SetupWizard *pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_wizard(*pWizard),
    m_context(context)
{
    m_pActivityIndicator = CreateActivityIndicator(this);
    m_pActivityIndicator->Hide();

    const auto pListBox = new wxCheckListBox(this, wxID_ANY);
    m_pListBox = pListBox;

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pListBox, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
    pMainSizer->Add(m_pActivityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 5));

    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this, pListBox](wxWizardEvent& event)
    {
        m_asyncOperationCompletedSuccessfully = false;

        if (!event.GetDirection())
            return;

        pListBox->Clear();
        for (const auto& ws: m_context.m_workspaces)
        {
            pListBox->Append(ws.slug);
        }
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [pListBox, this](wxWizardEvent& event)
    {
        if (m_asyncOperationCompletedSuccessfully)
            return;

        if (!event.GetDirection() && !m_asyncOperationInProgress)
            return;

        event.Veto();

        wxArrayInt checkedItemIndexes;
        if (const auto checkedCount = pListBox->GetCheckedItems(checkedItemIndexes);
            !checkedCount)
        {
            return;
        }

        if (!m_asyncOperationInProgress)
            StartAsyncOperation();
    });
}

void WorkspacePage::StartAsyncOperation()
{
    if (m_asyncOperationInProgress)
        return;

    m_asyncOperationInProgress = true;

    StartBusyAnimation();

    wxArrayInt checkedItemIndexes;
    m_pListBox->GetCheckedItems(checkedItemIndexes);

    std::vector<Workspace> workspaces;
    for (const auto& index: checkedItemIndexes)
    {
        const auto& workspace = m_context.m_workspaces.at(index);
        workspaces.push_back({workspace});
    }

    m_context.m_repositories.clear();

    wxWeakRef isWindowValid(this);
    std::thread([this, workspaces, isWindowValid]
    {
        const CurlConnection connection;
        for (const auto& workspace: workspaces)
        {
            RepositoriesRequest repositoriesRequest(connection);

            Match(repositoriesRequest.GetRepositories(workspace),
                  [this](const RepositoriesSuccess& success)
                  {
                      for (const auto& repository: success.repositories)
                      {
                          m_context.m_repositories.push_back(repository);
                      }
                  },
                  [](const Error& error)
                  {
                      wxLogError("Failed to get repositories: %s", error.message);
                  }
            );
        }

        CallAfter([this, isWindowValid]
        {
            if (!isWindowValid)
                return;

            StopBusyAnimation();
            m_asyncOperationInProgress = false;

            if (!m_context.m_repositories.empty())
            {
                m_asyncOperationCompletedSuccessfully = true;
                m_wizard.ShowPage(GetNext());
            }
        });
    }).detach();
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
