#include <format>
#include <thread>
#include <nlohmann/json.hpp>
#include <wx/activityindicator.h>
#include <wx/base64.h>
#include <wx/webrequest.h>
#include <wx/wizard.h>
#include <wx/wx.h>
#include <cpp_utils/match_expected.h>

#include "WorkspacePage.h"
#include "WorkspaceView.h"

#include "../../curl/CurlConnection.h"
#include "../../webrequests/RepositoriesRequest.h"

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

    wxWeakRef isWindowValid(this);
    std::thread([this, workspaces, isWindowValid]
    {
        const CurlConnection connection;
        for (const auto& workspace: workspaces)
        {
            RepositoriesRequest repositoriesRequest(connection);

            ip::match_expected(
                repositoriesRequest.GetRepositories(workspace),
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
    m_pWorkspaceView->ShowActivityIndicator();
    Layout();
}

void WorkspacePage::StopBusyAnimation()
{
    Enable();
    m_pWorkspaceView->HideActivityIndicator();
    Layout();
}
