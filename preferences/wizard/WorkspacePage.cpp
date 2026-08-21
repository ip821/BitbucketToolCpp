#include "WorkspacePage.h"

#include <format>
#include <thread>
#include <cpp_utils/match_expected.h>
#include <wx/activityindicator.h>
#include <wx/base64.h>
#include <wx/wizard.h>
#include <wx/wx.h>

#include "WorkspaceView.h"
#include "../Credentials.h"
#include "bitbucket_api/Requests.h"
#include "bitbucket_api/Structs.h"

WorkspacePage::WorkspacePage(SetupWizard *pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_wizard(*pWizard),
    m_context(context)
{
    m_pWorkspaceView = new WorkspaceView(this);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(m_pWorkspaceView, wxSizerFlags().Expand().Proportion(1));

    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this](wxWizardEvent& event)
    {
        m_asyncOperationCompletedSuccessfully = false;

        if (!event.GetDirection())
            return;

        m_pWorkspaceView->SetWorkspaces(m_context.m_workspaces);
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this](wxWizardEvent& event)
    {
        if (m_asyncOperationCompletedSuccessfully)
            return;

        if (!event.GetDirection() && !m_asyncOperationInProgress)
            return;

        event.Veto();

        wxArrayInt checkedItemIndexes;
        if (const auto checkedCount = m_pWorkspaceView->GetCheckedItems(checkedItemIndexes);
            !checkedCount)
        {
            return;
        }

        if (!m_asyncOperationInProgress)
            StartAsyncOperation();
    });
}

void WorkspacePage::StopAsyncOperation()
{
    m_thread.request_stop();
    if (m_thread.joinable())
        m_thread.join();
}

void WorkspacePage::StartAsyncOperation()
{
    if (m_asyncOperationInProgress)
        return;

    m_asyncOperationInProgress = true;

    StartBusyAnimation();

    wxArrayInt checkedItemIndexes;
    m_pWorkspaceView->GetCheckedItems(checkedItemIndexes);

    std::vector<Workspace> workspaces;
    for (const auto& index: checkedItemIndexes)
    {
        const auto& workspace = m_context.m_workspaces.at(index);
        workspaces.push_back({workspace});
    }

    m_context.m_repositories.clear();

    m_thread = std::jthread([this, workspaces](const std::stop_token stopToken)
    {
        std::vector<Repository> repositories;
        for (const auto& workspace: workspaces)
        {
            if (stopToken.stop_requested())
                return;

            const auto credentials = Credentials::GetCredentialsBase64().ToStdString();
            RepositoriesRequest repositoriesRequest;

            ip::match_expected(
                repositoriesRequest.GetRepositories(credentials, workspace),
                [&repositories](const auto& success)
                {
                    for (const auto& repository: success.values)
                    {
                        repositories.push_back(repository);
                    }
                },
                [](const auto& error)
                {
                    wxLogError("Failed to get repositories: %s", error.message);
                }
            );
        }

        if (stopToken.stop_requested())
            return;

        this->CallAfter([this, repositories, stopToken]
        {
            if (stopToken.stop_requested())
                return;

            StopBusyAnimation();
            m_asyncOperationInProgress = false;

            m_context.m_repositories = repositories;

            if (!m_context.m_repositories.empty())
            {
                m_asyncOperationCompletedSuccessfully = true;
                m_wizard.ShowPage(GetNext());
            }
        });
    });
}

void WorkspacePage::StartBusyAnimation()
{
    Disable();
    m_pWorkspaceView->ShowActivityIndicator();
    Layout();
}

void WorkspacePage::StopBusyAnimation()
{
    Enable();
    m_pWorkspaceView->HideActivityIndicator();
    Layout();
}
