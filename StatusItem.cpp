#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/notifmsg.h>

#include <thread>
#include <ranges>

#include "StatusItem.h"

#include <cpp_utils/match_expected.h>

#include "http/HttpConnection.h"
#include "preferences/settings/Config.h"
#include "preferences/PreferencesWindow.h"
#include "pull_requests/PullRequestService.h"
#include "webrequests/CurrentUserRequest.h"
#include "webrequests/PullRequestsRequest.h"

enum
{
    MENU_ITEM_PREFERENCES_ID = 10000,
    MENU_ITEM_QUIT_ID,
    MENU_ITEM_UPDATE_ID,
    MENU_ITEM_CREATE_PULL_REQUEST_ID = MENU_ITEM_PREFERENCES_ID + 100,
    MENU_ITEM_LAST_SEPARATOR = MENU_ITEM_PREFERENCES_ID + 1000 - 1,
};

constexpr auto tenSeconds = 10 * 1000;
constexpr auto fiveMinutes = 5 * 60 * 1000;

StatusItem::StatusItem() :
    wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM)
{
    m_pDialog = new PreferencesWindow();

#if defined(__WXOSX__)
    SetIcon("status32@2x");
    SetTitle("1/2");
#else
    m_statusBitmap = wxXmlResource::Get()->LoadBitmap("status32");
    if (!m_statusBitmap.IsOk())
    {
        wxMessageBox("Bitmap was loaded incorrectly");
    }
    m_bitmapBundle = wxBitmapBundle::FromBitmap(m_statusBitmap);
    if (!m_bitmapBundle.IsOk())
    {
        wxMessageBox("Could not load status image");
    }
    if (!IsAvailable())
    {
        wxMessageBox("System icon is not available");
    }
    SetIcon(m_bitmapBundle, "Tooltip");
#endif
    m_pCreatePullRequestsMenu = new wxMenu;
    m_pCreatePullRequestsMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuCreatePr, this);

    const auto pMenu = new wxMenu;
    pMenu->AppendSeparator()->SetId(MENU_ITEM_LAST_SEPARATOR);
    pMenu->AppendSubMenu(m_pCreatePullRequestsMenu, "&Create pull request");
    pMenu->AppendSeparator();
    pMenu->Append(MENU_ITEM_UPDATE_ID, "&Update");
    pMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuUpdate, this, MENU_ITEM_UPDATE_ID);
    pMenu->Append(MENU_ITEM_PREFERENCES_ID, "&Preferences...");
    pMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuPreferences, this, MENU_ITEM_PREFERENCES_ID);
    pMenu->Append(MENU_ITEM_QUIT_ID, "&Quit");
    pMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuQuit, this, MENU_ITEM_QUIT_ID);
    m_pMenu = pMenu;

    Bind(wxEVT_TASKBAR_LEFT_DCLICK, &StatusItem::OnLeftButtonDClick, this);

    m_pTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        OnUpdatePullRequests({.showNotification = false});
    });
    m_pTimer->StartOnce(tenSeconds);
}

void StatusItem::ShowPreferencesDialog() const
{
    if (!m_pDialog->IsVisible())
        m_pDialog->Show(true);

    m_pDialog->Raise();
}

void StatusItem::OnMenuPreferences(wxCommandEvent&)
{
    ShowPreferencesDialog();
}

void StatusItem::OnMenuQuit(wxCommandEvent&)
{
    wxExit();
}

void StatusItem::OnMenuUpdate(wxCommandEvent&)
{
    OnUpdatePullRequests({.showNotification = true});
}

void StatusItem::OnMenuCreatePr(wxCommandEvent& event)
{
    const auto menuItemId = event.GetId();
    const auto pMenuItem = m_pCreatePullRequestsMenu->FindItemByPosition(menuItemId - MENU_ITEM_CREATE_PULL_REQUEST_ID);

    const auto repository = pMenuItem->GetItemLabel();
    const auto parts = wxSplit(repository, '/');
    const auto workspace = parts[0];
    const auto repo = parts[1];

    const wxString bitbucketHostname = "https://bitbucket.org";
    const auto url = bitbucketHostname +
            "/" + workspace +
            "/" + repo +
            "/pull-requests/new";

    wxLaunchDefaultBrowser(url);
}

void StatusItem::RemoveAllPrMenuItems()
{
    for (const auto menuItems = m_pMenu->GetMenuItems();
         const auto& item: menuItems)
    {
        if (item->GetId() > MENU_ITEM_LAST_SEPARATOR)
        {
            m_pMenu->Delete(item);
        }
    }
}

void StatusItem::UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories)
{
    for (const auto menuItems = m_pCreatePullRequestsMenu->GetMenuItems();
         const auto& item: menuItems)
    {
        m_pCreatePullRequestsMenu->Delete(item);
    }

    auto index = 0;
    for (const auto& repository: repositories)
    {
        m_pCreatePullRequestsMenu->Append(MENU_ITEM_CREATE_PULL_REQUEST_ID + index++, repository.full_name);
    }

    m_repositories = repositories;
}

void StatusItem::ShowErrorNotification(const wxString& message) const
{
    wxNotificationMessage notification("BitbucketTool", message);
    notification.Show();
}

void StatusItem::OnUpdatePullRequests(const OnUpdatePullRequestsArgs& args)
{
    const auto repositories = Config::GetRepositories();

    UpdateCreatePullRequestsMenu(repositories);

    wxWeakRef isWindowValid(this);
    std::thread([this, args, isWindowValid]
    {
        PullRequestService pullRequestService;
        const auto result = pullRequestService.GetPullRequests();

        if (!result)
        {
            ShowErrorNotification(result.error().message);
            return;
        }

        const auto pullRequestsInfo = result.value();

        CallAfter([isWindowValid, args, this, pullRequestsInfo]
        {
            if (!isWindowValid)
                return;

            RemoveAllPrMenuItems();

            auto index = 0;
            auto id = MENU_ITEM_LAST_SEPARATOR + 1;
            m_pMenu->Insert(index++, id++, "Pull requests to review")->Enable(false);

            auto waitingForMyApprovalPullRequests = std::views::filter(
                pullRequestsInfo.pullRequests,
                [&pullRequestsInfo](const PullRequestInfo& it)
                {
                    const auto& pullRequest = it.pullRequest;
                    const auto& currentUser = pullRequestsInfo.currentUser;
                    const auto& currentUserParticipant = pullRequest.GetParticipantForUser(currentUser);
                    return
                            pullRequest.author.uuid != currentUser.uuid
                            && !pullRequest.draft
                            && currentUserParticipant.has_value()
                            && !currentUserParticipant.value().approved;
                });

            for (const auto& [pullRequest]: waitingForMyApprovalPullRequests)
            {
                m_pMenu->Insert(index++, id++, pullRequest.GetTitle().Left(90));
            }
            // m_pMenu->Insert(index++, id++, "   New item 1");
            // m_pMenu->Insert(index++, id++, "   New item 2")->Enable(false);

            m_pMenu->InsertSeparator(index++);
            m_pMenu->Insert(index++, id++, "Your pull requests")->Enable(false);
            auto myPullRequests = std::views::filter(
                pullRequestsInfo.pullRequests,
                [&pullRequestsInfo](const PullRequestInfo& it)
                {
                    const auto& pullRequest = it.pullRequest;
                    const auto& currentUser = pullRequestsInfo.currentUser;
                    return pullRequest.author.uuid == currentUser.uuid && pullRequest.state == Open;
                });

            for (const auto& [pullRequest]: myPullRequests)
            {
                m_pMenu->Insert(index++, id++, pullRequest.GetTitle().Left(90));
            }
            // m_pMenu->Insert(index++, id++, "   New item 1");
            // m_pMenu->Insert(index++, id++, "   New item 2")->Enable(false);

            if (args.showNotification)
            {
                wxNotificationMessage notification("BitbucketTool", "Pull requests were updated");
                notification.Show();
            }

            // SetTitle();

            m_pTimer->StartOnce(fiveMinutes);
        });
    }).detach();
}

wxMenu *StatusItem::GetPopupMenu()
{
    return m_pMenu;
}

void StatusItem::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    ShowPreferencesDialog();
}
