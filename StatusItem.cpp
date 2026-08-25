#include "StatusItem.h"

#include <algorithm>
#include <ranges>
#include <thread>
#include <cpp_utils/match_variant.h>
#include <cpp_utils/wx_string_format.h>
#include <wx/clipbrd.h>
#include <wx/control.h>
#include <wx/dcmemory.h>
#include <wx/notifmsg.h>
#include <wx/settings.h>
#include <wx/wx.h>

#include "bitbucket_api/include/bitbucket_api/Requests.h"
#include "preferences/PreferencesWindow.h"
#include "preferences/settings/Config.h"
#include "menu/PullRequestsMenuBuilder.h"
#include "pull_requests/PullRequestService.h"
#include "Stopwatch.h"

#ifdef __WXMSW__
#include "windows/CustomIcon.h"
#elif !defined(__WXOSX__)
#include "images/StatusImage.h"
#endif

enum
{
    MENU_ITEM_FIRST_STATIC_ID = 10000,

    MENU_ITEM_STATISTICS,
    MENU_ITEM_PREFERENCES_ID,
    MENU_ITEM_SHOW_ALL,
    MENU_ITEM_QUIT_ID,
    MENU_ITEM_UPDATE_ID,
    MENU_ITEM_LAST_SEPARATOR,

    MENU_ITEM_CREATE_PULL_REQUEST_ID,
    MENU_ITEM_LAST_PULL_REQUEST_ID = MENU_ITEM_CREATE_PULL_REQUEST_ID + 100,
    MENU_ITEM_LAST_STATIC_ID,
};

constexpr auto tenSeconds = 10 * 1000;
constexpr auto fiveMinutes = 5 * 60 * 1000;

namespace
{
    constexpr auto maxMenuWidth = 500;

    wxString FitMenuText(const wxString& text)
    {
        wxBitmap bitmap(1, 1);
        wxMemoryDC dc(bitmap);
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        return wxControl::Ellipsize(text, dc, wxELLIPSIZE_END, maxMenuWidth);
    }

#ifdef __WXMSW__
    wxBitmapBundle CreateReviewCountIcon(const wxString& text)
    {
        return wxBitmapBundle::FromBitmaps(
            CustomIcon::CreateReviewCountBitmap(text, 16),
            CustomIcon::CreateReviewCountBitmap(text, 32));
    }
#endif
}

StatusItem::StatusItem() :
    wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM)
{
    m_pDialog = new PreferencesWindow(this);

#if defined(__WXOSX__)
    SetIcon("status32@2x");
#elif defined(__WXMSW__)
    if (!IsAvailable())
    {
        wxMessageBox("System icon is not available");
    }
    SetStatusItemTitle(wxS("0"));
    wxNotificationMessage::UseTaskBarIcon(this);
    wxNotificationMessage::MSWUseToasts(
        wxS("PRToolForBitbucket"),
        wxS("ip.PRToolForBitbucket"));
#else
    m_statusBitmap = LoadEmbeddedStatusBitmap();
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
    m_pCreatePullRequestsMenu = new wxMenu();
    m_pCreatePullRequestsMenu->Bind(wxEVT_MENU, &StatusItem::OnCreatePullRequestMenuItemClick, this);

    m_pMenu = std::make_unique<wxMenu>();
    m_pMenu->AppendSeparator()->SetId(MENU_ITEM_LAST_SEPARATOR);
    m_pMenu->AppendSubMenu(m_pCreatePullRequestsMenu, "&Create pull request");
    m_pMenu->AppendSeparator();
    m_pMenu->Append(MENU_ITEM_STATISTICS, "Statistics")->Enable(false);
    m_pMenu->Append(MENU_ITEM_SHOW_ALL, "&Show hidden items");
    m_pMenu->Append(MENU_ITEM_UPDATE_ID, "&Update");
    m_pMenu->Append(MENU_ITEM_PREFERENCES_ID, "&Preferences...");
    m_pMenu->Append(MENU_ITEM_QUIT_ID, "&Quit");
    m_pMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuItemClick, this);

#ifdef __WXMSW__
    Bind(wxEVT_TASKBAR_LEFT_UP, &StatusItem::OnLeftButtonClick, this);
#endif

    m_pTimer = std::make_unique<wxTimer>(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        UpdatePullRequests({.showNotification = false});
    });

    ConfigChanged();

#if defined(WXDEBUG)
    wxUnusedVar(tenSeconds);
    m_pTimer->StartOnce(1000);
#else
    m_pTimer->StartOnce(tenSeconds);
#endif
}

StatusItem::~StatusItem()
{
    m_thread.request_stop();
    if (m_thread.joinable())
        m_thread.join();

#ifdef __WXMSW__
    wxNotificationMessage::UseTaskBarIcon(nullptr);
#endif
}

void StatusItem::ShowPreferencesDialog() const
{
    if (!m_pDialog->IsVisible())
        m_pDialog->Show(true);

    m_pDialog->Raise();
}

void StatusItem::RefreshMenu()
{
    RebuildMenu({.pullRequests = m_pullRequestsInfo, .showAll = !m_showAllPullRequests});
}

void StatusItem::OnMenuItemClick(wxCommandEvent& e)
{
    const bool isAltPressed = wxGetKeyState(WXK_ALT);

    switch (e.GetId())
    {
        case MENU_ITEM_UPDATE_ID:
            UpdatePullRequests({.showNotification = true, .fullReload = isAltPressed});
            return;

        case MENU_ITEM_PREFERENCES_ID:
            ShowPreferencesDialog();
            return;

        case MENU_ITEM_QUIT_ID:
            wxExit();
            return;

        case MENU_ITEM_SHOW_ALL:
            RefreshMenu();
            return;

        default:
            break;
    }

    if (const auto it = m_menuItemIdToPullRequest.find(e.GetId());
        it != m_menuItemIdToPullRequest.end())
    {
        const auto href = it->second.pullRequest.links.html.href;
        if (isAltPressed)
        {
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(href));
                wxTheClipboard->Close();
            }
        } else
        {
            wxLaunchDefaultBrowser(href);
        }
    }
}

void StatusItem::OnCreatePullRequestMenuItemClick(wxCommandEvent& event)
{
    const auto menuItemId = event.GetId();
    const auto repositoryIt = m_menuItemIdToRepository.find(menuItemId);
    if (repositoryIt == m_menuItemIdToRepository.end())
        return;

    const auto& repository = repositoryIt->second;
    const auto parts = wxSplit(repository, '/');
    const auto workspace = parts[0];
    const auto repo = parts[1];

    const wxString bitbucketHostname = wxS("https://bitbucket.org");
    const auto url = bitbucketHostname +
            wxS("/") + workspace +
            wxS("/") + repo +
            wxS("/pull-requests/new");

    wxLaunchDefaultBrowser(url);
}

void StatusItem::UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories)
{
    for (const auto menuItems = m_pCreatePullRequestsMenu->GetMenuItems();
         const auto& item: menuItems)
    {
        m_pCreatePullRequestsMenu->Delete(item);
    }

    m_menuItemIdToRepository.clear();

    auto index = 0;
    for (const auto& repository: repositories)
    {
        const auto menuItemId = MENU_ITEM_CREATE_PULL_REQUEST_ID + index++;
        m_menuItemIdToRepository[menuItemId] = repository.full_name;
        m_pCreatePullRequestsMenu->Append(menuItemId, FitMenuText(repository.full_name));
    }
}

void StatusItem::ShowErrorNotification(const wxString& message) const
{
    wxNotificationMessage notification("BitbucketTool", message);
    notification.Show();
}

void StatusItem::RebuildMenu(const RebuildMenuArgs& args)
{
    const auto& [pullRequestsInfo, showAll] = args;
    m_showAllPullRequests = showAll;

    const auto hideChangesRequestedPullRequests = !showAll && Config::GetHideChangesRequestedPullRequests();
    const auto useSubmenusOnMenuOverflow = Config::GetUseSubmenusOnMenuOverflow();
    const auto displayRepositoryNameLowercase = Config::GetDisplayRepositoryNameLowercase();

    const PullRequestsMenuBuilder menuBuilder(*m_pMenu);
    auto [menuItemIdToPullRequest, hiddenPullRequestsCount] = menuBuilder.Rebuild(
        MENU_ITEM_LAST_STATIC_ID + 1,
        pullRequestsInfo,
        hideChangesRequestedPullRequests,
        useSubmenusOnMenuOverflow,
        displayRepositoryNameLowercase
    );

    m_menuItemIdToPullRequest = std::move(menuItemIdToPullRequest);
    UpdateTitle(pullRequestsInfo, hiddenPullRequestsCount);
}

void StatusItem::UpdatePullRequests(const OnUpdatePullRequestsArgs& args)
{
    m_pMenu->Enable(MENU_ITEM_UPDATE_ID, false);

    const auto repositories = Config::GetRepositories();
    const auto statisticsLabel = m_pMenu->GetLabel(MENU_ITEM_STATISTICS);

    UpdateCreatePullRequestsMenu(repositories);

    m_thread = std::jthread([this, args, repositories, statisticsLabel](const std::stop_token stopToken)
    {
        const Stopwatch fetchStopwatch;

        PullRequestService pullRequestService;
        const auto progressCallback = [this, stopToken](const PullRequestUpdateProgressArgs& progressArgs)
        {
            if (stopToken.stop_requested())
                return;

            this->CallAfter([this, stopToken, progressArgs]
            {
                if (!stopToken.stop_requested())
                    UpdateProgress(progressArgs);
            });
#ifdef __WXMSW__
            wxTheApp->MSWProcessPendingEventsIfNeeded();
#endif
        };
        const auto pullRequestsResult = pullRequestService.GetPullRequests(repositories, progressCallback, stopToken);

        if (stopToken.stop_requested())
            return;

        const auto elapsedTime = fetchStopwatch.GetElapsed();

        this->CallAfter([args, elapsedTime, this, pullRequestsResult, statisticsLabel, stopToken]
        {
            if (stopToken.stop_requested())
                return;

            m_pTimer->StartOnce(fiveMinutes);
            m_pMenu->Enable(MENU_ITEM_UPDATE_ID, true);

            if (!pullRequestsResult)
            {
                const auto message = pullRequestsResult.error().message;
                m_pMenu->SetLabel(
                    MENU_ITEM_STATISTICS,
                    FitMenuText(std::format("Update failed: {}", message))
                );
                ShowErrorNotification(message);
                return;
            }

            const auto& pullRequestsInfo = pullRequestsResult.value();
            m_pullRequestsInfo = pullRequestsInfo;

            UpdateStatistics(pullRequestsInfo.processedPullRequestsCount, pullRequestsInfo.fetchedPullRequestsCount, elapsedTime);
            RebuildMenu({.pullRequests = pullRequestsInfo, .showAll = false});

            if (args.showNotification)
            {
                wxNotificationMessage notification("BitbucketTool", "Pull requests were updated");
                notification.Show();
            }
        });
#ifdef __WXMSW__
        wxTheApp->MSWProcessPendingEventsIfNeeded();
#endif
    });
}

void StatusItem::UpdateProgress(const PullRequestUpdateProgressArgs& progressArgs)
{
    const wxString label = ip::match_variant(
        progressArgs,
        [](const FetchingRepositoryPullRequests& args)
        {
            return wxS("Querying [") + wxString::FromUTF8(args.repositoryName) + wxS("]");
        },
        [](const FetchingPullRequestDetails& args)
        {
            const auto percentage = args.totalPullRequests == 0
                ? 0
                : args.currentPullRequest * 100 / args.totalPullRequests;

            return wxString(std::format(wxS("Fetching — {}%"), percentage));
        }
    );

    m_pMenu->SetLabel(MENU_ITEM_STATISTICS, FitMenuText(label));
}

void StatusItem::UpdateStatistics(size_t processedPullRequestsCount, size_t fetchedPullRequestsCount, std::chrono::seconds elapsedTime)
{
    const auto elapsedSeconds = elapsedTime.count();
    const auto nextUpdate = wxDateTime::Now() + wxTimeSpan::Milliseconds(fiveMinutes);
    m_pMenu->SetLabel(
        MENU_ITEM_STATISTICS,
        FitMenuText(
            std::format(
                wxS("PRs fetched: {}/{}. Update took: {:02}:{:02}. Next update at: {}"),
                processedPullRequestsCount,
                fetchedPullRequestsCount,
                elapsedSeconds / 60,
                elapsedSeconds % 60,
                nextUpdate.Format(wxS("%d.%m.%Y %H:%M")))));
}

void StatusItem::UpdateTitle(const PullRequestsInfo& pullRequestsInfo, const int hiddenPullRequestsCount)
{
    const auto waitingCount = pullRequestsInfo.waitingForMyApprovalPullRequests.size() - hiddenPullRequestsCount;

#ifdef __WXMSW__
    SetStatusItemTitle(std::format(wxS("{}"), waitingCount));
#else
    const auto myCount = pullRequestsInfo.myPullRequests.size();
    if (waitingCount || myCount)
    {
        if (myCount)
        {
            auto title = std::format(wxS("{}/{}"), waitingCount, myCount);

            const auto hasFailedBuilds = std::ranges::any_of(
                pullRequestsInfo.myPullRequests,
                [](const auto& it) { return it.HasBuildsWithStatus(StatusState::Failed); });

            const auto hasSomeoneRequestedChanges = std::ranges::any_of(
                pullRequestsInfo.myPullRequests,
                [](const auto& it)
                {
                    return std::ranges::any_of(it.pullRequest.participants, [](const auto& p) { return p.state == ParticipantState::ChangesRequested; });
                });

            if (hasFailedBuilds || hasSomeoneRequestedChanges)
                title += wxS(" (!)");

            SetStatusItemTitle(title);
        } else
        {
            SetStatusItemTitle(std::format(wxS("{}"), waitingCount));
        }
    } else
    {
        SetStatusItemTitle(wxS(""));
    }
#endif
}

void StatusItem::ConfigChanged()
{
    m_showAllPullRequests = Config::GetHideChangesRequestedPullRequests();
    RefreshMenu();
}

wxMenu *StatusItem::GetPopupMenu()
{
    return m_pMenu.get();
}

void StatusItem::SetStatusItemTitle(const wxString& title)
{
#if defined(__WXOSX__)
    SetTitle(title);
#elif defined(__WXMSW__)
    m_bitmapBundle = CreateReviewCountIcon(title);
    if (!m_bitmapBundle.IsOk())
        return;

    SetIcon(
        m_bitmapBundle,
        wxS("Pull requests to review: ") + title);
#else
    auto tooltipTitle = title.IsEmpty() ? wxS("none") : title;
    SetIcon(m_bitmapBundle, std::format(wxS("Pull requests: {}"), tooltipTitle));
#endif
}

void StatusItem::OnLeftButtonClick(wxTaskBarIconEvent&)
{
    PopupMenu(m_pMenu.get());
}
