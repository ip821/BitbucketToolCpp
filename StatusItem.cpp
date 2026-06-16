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
#include "wx/clipbrd.h"
#include "wx/osx/clipbrd.h"

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
    pMenu->Append(MENU_ITEM_PREFERENCES_ID, "&Preferences...");
    pMenu->Append(MENU_ITEM_QUIT_ID, "&Quit");
    pMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuItemClick, this);
    m_pMenu = pMenu;

#ifdef __WXMSW__
    Bind(wxEVT_TASKBAR_LEFT_UP, &StatusItem::OnLeftButtonClick, this);
#endif

    m_pTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        OnUpdatePullRequests({.showNotification = false});
    });

#if defined(WXDEBUG)
    wxUnusedVar(tenSeconds);
    m_pTimer->StartOnce(1000);
#else
    m_pTimer->StartOnce(tenSeconds);
#endif
}

void StatusItem::ShowPreferencesDialog() const
{
    if (!m_pDialog->IsVisible())
        m_pDialog->Show(true);

    m_pDialog->Raise();
}

void StatusItem::OnMenuItemClick(wxCommandEvent& e)
{
    switch (e.GetId())
    {
        case MENU_ITEM_UPDATE_ID:
            OnUpdatePullRequests({.showNotification = true});
            return;

        case MENU_ITEM_PREFERENCES_ID:
            ShowPreferencesDialog();
            return;

        case MENU_ITEM_QUIT_ID:
            wxExit();
            return;

        default:
            break;
    }

    if (const auto it = m_menuItemIdToPullRequest.find(e.GetId());
        it != m_menuItemIdToPullRequest.end())
    {
        const auto href = it->second.pullRequest.links.html.href;
        if (wxGetKeyState(WXK_ALT))
        {
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(href));
                wxTheClipboard->Close();
            }
        }
        else
        {
            wxLaunchDefaultBrowser(href);
        }
    }
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

            m_menuItemIdToPullRequest.clear();

            auto index = 0;
            auto id = MENU_ITEM_LAST_SEPARATOR + 1;
            m_pMenu->Insert(index++, id++, "Pull requests to review")->Enable(false);

            for (const auto& pullRequest: pullRequestsInfo.waitingForMyApprovalPullRequests)
            {
                m_menuItemIdToPullRequest[id] = pullRequest;
                m_pMenu->Insert(index++, id++, pullRequest.GetMainMenuItemTitle().Left(90));

                const auto secondLineTitle = std::format(wxS("   {}"), pullRequest.GetAuthorAndBranchMenuItemTitle());
                m_pMenu->Insert(index++, id++, secondLineTitle)->Enable(false);

                const auto thirdLineTitle = std::format(wxS("   {}"), pullRequest.GetPullRequestDetailsMenuItemTitle());
                m_pMenu->Insert(index++, id++, thirdLineTitle)->Enable(false);
            }

            m_pMenu->InsertSeparator(index++);
            m_pMenu->Insert(index++, id++, "Your pull requests")->Enable(false);

            for (const auto& pullRequest: pullRequestsInfo.myPullRequests)
            {
                m_menuItemIdToPullRequest[id] = pullRequest;
                m_pMenu->Insert(index++, id++, pullRequest.GetMainMenuItemTitle().Left(90));
            }

            const auto waitingCount = pullRequestsInfo.waitingForMyApprovalPullRequests.size();
            const auto myCount = pullRequestsInfo.myPullRequests.size();
            if (waitingCount || myCount)
            {
                if (myCount)
                {
                    auto title = std::format(wxS("{}/{}"), waitingCount, myCount);
                    const auto hasFailedBuilds = std::ranges::any_of(
                        pullRequestsInfo.myPullRequests,
                        [](const auto& it)
                        {
                            return std::ranges::any_of(it.statuses, [](const auto& status) { return status.state == Failed; });
                        });
                    const auto hasSomeoneRequestedChanges = std::ranges::any_of(
                        pullRequestsInfo.myPullRequests,
                        [](const auto& it)
                        {
                            return std::ranges::any_of(it.pullRequest.participants, [](const auto& p) { return p.state == ChangesRequested; });
                        });

                    if (hasFailedBuilds || hasSomeoneRequestedChanges)
                        title += wxS(" (!)");
                } else
                {
                    SetStatusItemTitle(std::format(wxS("{}"), waitingCount));
                }
            } else
            {
                SetStatusItemTitle(wxS(""));
            }

            if (args.showNotification)
            {
                wxNotificationMessage notification("BitbucketTool", "Pull requests were updated");
                notification.Show();
            }

            m_pTimer->StartOnce(fiveMinutes);
        });
    }).detach();
}

wxMenu *StatusItem::GetPopupMenu()
{
    return m_pMenu;
}

void StatusItem::SetStatusItemTitle(const wxString& title)
{
#if defined(__WXOSX__)
    SetTitle(title);
#else
    wxUnusedVar(title);
#endif
}

void StatusItem::OnLeftButtonClick(wxTaskBarIconEvent&)
{
    PopupMenu(m_pMenu);
}
