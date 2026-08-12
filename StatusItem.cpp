#include "StatusItem.h"

#include <algorithm>
#include <ranges>
#include <thread>

#include <wx/choicdlg.h>
#include <wx/notifmsg.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "bitbucket_api/include/bitbucket_api/Requests.h"
#include "preferences/PreferencesWindow.h"
#include "preferences/settings/Config.h"
#include "pull_requests/PullRequestService.h"
#include "pull_requests/PullRequestsWindow.h"
#include "Stopwatch.h"

#if defined(__WXOSX__)
extern "C" void ActivateAppWithoutDockIcon();
#endif

enum
{
    MENU_ITEM_FIRST_ID = 10000,
    MENU_ITEM_OPEN_PULL_REQUESTS,
    MENU_ITEM_CREATE_PULL_REQUEST,
    MENU_ITEM_UPDATE,
    MENU_ITEM_PREFERENCES,
    MENU_ITEM_QUIT,
};

constexpr auto tenSeconds = 10 * 1000;
constexpr auto fiveMinutes = 5 * 60 * 1000;

StatusItem::StatusItem() :
    wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM)
{
    m_pDialog = new PreferencesWindow(this);
    m_pPullRequestsWindow = new PullRequestsWindow({
        .createPullRequest = [this]
        {
            ShowCreatePullRequestDialog();
        },
        .update = [this]
        {
            UpdatePullRequests({.showNotification = true, .fullReload = wxGetKeyState(WXK_ALT)});
        },
        .showPreferences = [this]
        {
            ShowPreferencesDialog();
        },
        .showAllPullRequests = [this](const bool showAll)
        {
            m_showAllPullRequests = showAll;
            RefreshPullRequestsWindow();
            UpdateTitle();
        },
    });

#if defined(__WXOSX__)
    SetIcon("status32@2x");
#else
    m_statusBitmap = wxXmlResource::Get()->LoadBitmap("status32");
    if (!m_statusBitmap.IsOk())
        wxMessageBox("Bitmap was loaded incorrectly");

    m_bitmapBundle = wxBitmapBundle::FromBitmap(m_statusBitmap);
    if (!m_bitmapBundle.IsOk())
        wxMessageBox("Could not load status image");

    if (!IsAvailable())
        wxMessageBox("System icon is not available");

    SetIcon(m_bitmapBundle, "Pull requests have not been loaded yet");
#endif

    m_pCommandMenu = std::make_unique<wxMenu>();
    m_pCommandMenu->Append(MENU_ITEM_OPEN_PULL_REQUESTS, "&Open pull requests...");
    m_pCommandMenu->Append(MENU_ITEM_CREATE_PULL_REQUEST, "&Create pull request...")->Enable(false);
    m_pCommandMenu->AppendSeparator();
    m_pCommandMenu->Append(MENU_ITEM_UPDATE, "&Update");
    m_pCommandMenu->AppendSeparator();
    m_pCommandMenu->Append(MENU_ITEM_PREFERENCES, "&Preferences...");
    m_pCommandMenu->Append(MENU_ITEM_QUIT, "&Quit");
    m_pCommandMenu->Bind(wxEVT_MENU, &StatusItem::OnMenuItemClick, this);

#ifdef __WXMSW__
    Bind(wxEVT_TASKBAR_LEFT_UP, &StatusItem::OnLeftButtonClick, this);
#else
    Bind(wxEVT_TASKBAR_LEFT_DOWN, &StatusItem::OnLeftButtonClick, this);
#endif
    Bind(wxEVT_TASKBAR_CLICK, &StatusItem::OnRightButtonClick, this);

    m_pTimer = std::make_unique<wxTimer>(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        UpdatePullRequests({.showNotification = false});
    });

    m_repositories = Config::GetRepositories();
    m_pPullRequestsWindow->SetRepositories(m_repositories);
    m_pCommandMenu->Enable(MENU_ITEM_CREATE_PULL_REQUEST, !m_repositories.empty());
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
    if (m_pPullRequestsWindow)
        m_pPullRequestsWindow->Destroy();
}

void StatusItem::ShowPullRequestsWindow()
{
#if defined(__WXOSX__)
    if (!m_pPullRequestsWindow->IsShown())
        ActivateAppWithoutDockIcon();
#endif
    m_pPullRequestsWindow->ToggleNear(wxGetMousePosition());
}

void StatusItem::ShowPreferencesDialog() const
{
    if (!m_pDialog->IsVisible())
        m_pDialog->Show(true);

    m_pDialog->Raise();
}

void StatusItem::ShowCreatePullRequestDialog()
{
    if (m_repositories.empty())
        return;

    size_t repositoryIndex{};
    if (m_repositories.size() > 1)
    {
        wxArrayString repositoryNames;
        repositoryNames.Alloc(m_repositories.size());
        for (const auto& repository: m_repositories)
            repositoryNames.Add(repository.full_name);

        auto* parent = m_pPullRequestsWindow->IsShown() ? m_pPullRequestsWindow : nullptr;
        wxSingleChoiceDialog dialog(
            parent,
            "Choose the repository for the new pull request:",
            "Create pull request",
            repositoryNames);
        if (dialog.ShowModal() != wxID_OK)
            return;

        const auto selection = dialog.GetSelection();
        if (selection == wxNOT_FOUND)
            return;

        repositoryIndex = static_cast<size_t>(selection);
    }

    const auto& repository = m_repositories.at(repositoryIndex);
    const auto url = std::format(
        wxS("https://bitbucket.org/{}/pull-requests/new"),
        wxString::FromUTF8(repository.full_name));
    wxLaunchDefaultBrowser(url);
}

void StatusItem::RefreshPullRequestsWindow()
{
    if (!m_hasPullRequests)
        return;

    m_pPullRequestsWindow->SetPullRequests(
        m_pullRequestsInfo,
        m_showAllPullRequests,
        Config::GetHideChangesRequestedPullRequests(),
        Config::GetUseTwoColumnLayout());
}

size_t StatusItem::GetHiddenPullRequestsCount() const
{
    if (!Config::GetHideChangesRequestedPullRequests() || m_showAllPullRequests)
        return 0;

    return std::ranges::count_if(
        m_pullRequestsInfo.waitingForMyApprovalPullRequests,
        [this](const PullRequestInfo& pullRequest)
        {
            return !pullRequest.GetParticipantsRequestedChangesWithout(m_pullRequestsInfo.currentUser).empty();
        });
}

void StatusItem::OnLeftButtonClick(wxTaskBarIconEvent&)
{
    ShowPullRequestsWindow();
}

void StatusItem::OnRightButtonClick(wxTaskBarIconEvent&)
{
    PopupMenu(m_pCommandMenu.get());
}

void StatusItem::OnMenuItemClick(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case MENU_ITEM_OPEN_PULL_REQUESTS:
            ShowPullRequestsWindow();
            return;

        case MENU_ITEM_CREATE_PULL_REQUEST:
            ShowCreatePullRequestDialog();
            return;

        case MENU_ITEM_UPDATE:
            UpdatePullRequests({.showNotification = true, .fullReload = wxGetKeyState(WXK_ALT)});
            return;

        case MENU_ITEM_PREFERENCES:
            ShowPreferencesDialog();
            return;

        case MENU_ITEM_QUIT:
            wxExit();
            return;

        default:
            return;
    }
}

void StatusItem::ShowErrorNotification(const wxString& message) const
{
    wxNotificationMessage notification("BitbucketTool", message);
    notification.Show();
}

void StatusItem::UpdatePullRequests(const OnUpdatePullRequestsArgs& args)
{
    m_pCommandMenu->Enable(MENU_ITEM_UPDATE, false);
    m_pPullRequestsWindow->SetUpdating(true);

    m_repositories = Config::GetRepositories();
    m_pPullRequestsWindow->SetRepositories(m_repositories);
    m_pCommandMenu->Enable(MENU_ITEM_CREATE_PULL_REQUEST, !m_repositories.empty());

    wxWeakRef isWindowValid(this);
    m_thread = std::jthread([this, args, isWindowValid]
    {
        const Stopwatch fetchStopwatch;

        PullRequestService pullRequestService;
        const auto pullRequestsResult = pullRequestService.GetPullRequests(
            [isWindowValid, this](const PullRequestUpdateProgress& progress)
            {
                wxTheApp->CallAfter([isWindowValid, this, progress]
                {
                    if (!isWindowValid)
                        return;

                    m_pPullRequestsWindow->SetUpdateProgress(
                        progress.isFetchingDetails,
                        progress.completed,
                        progress.total);
                });
            });
        const auto elapsedTime = fetchStopwatch.GetElapsed();

        wxTheApp->CallAfter([isWindowValid, args, elapsedTime, this, pullRequestsResult]
        {
            if (!isWindowValid)
                return;

            m_pTimer->StartOnce(fiveMinutes);
            m_pCommandMenu->Enable(MENU_ITEM_UPDATE, true);
            m_pPullRequestsWindow->SetUpdating(false);

            if (!pullRequestsResult)
            {
                const auto message = pullRequestsResult.error().message;
                m_pPullRequestsWindow->SetUpdateStatusText(
                    std::format(wxS("Update failed: {}"), wxString::FromUTF8(message)));
                ShowErrorNotification(message);
                return;
            }

            m_pullRequestsInfo = pullRequestsResult.value();
            m_hasPullRequests = true;
            m_showAllPullRequests = false;

            UpdateStatistics(
                m_pullRequestsInfo.processedPullRequestsCount,
                m_pullRequestsInfo.fetchedPullRequestsCount,
                elapsedTime);
            RefreshPullRequestsWindow();
            UpdateTitle();

            if (args.showNotification)
            {
                wxNotificationMessage notification("BitbucketTool", "Pull requests were updated");
                notification.Show();
            }
        });
    });
}

void StatusItem::UpdateStatistics(
    const size_t processedPullRequestsCount,
    const size_t fetchedPullRequestsCount,
    const std::chrono::seconds elapsedTime)
{
    const auto elapsedSeconds = elapsedTime.count();
    const auto nextUpdate = wxDateTime::Now() + wxTimeSpan::Milliseconds(fiveMinutes);
    const auto statistics = std::format(
        wxS("PRs fetched: {}/{}. Update took: {:02}:{:02}. Next update at: {}"),
        processedPullRequestsCount,
        fetchedPullRequestsCount,
        elapsedSeconds / 60,
        elapsedSeconds % 60,
        nextUpdate.Format(wxS("%d.%m.%Y %H:%M")));
    m_pPullRequestsWindow->SetUpdateStatusText(statistics);
}

void StatusItem::UpdateTitle()
{
    const auto hiddenPullRequestsCount = GetHiddenPullRequestsCount();
    const auto waitingCount = m_pullRequestsInfo.waitingForMyApprovalPullRequests.size() - hiddenPullRequestsCount;
    const auto myCount = m_pullRequestsInfo.myPullRequests.size();
    if (waitingCount || myCount)
    {
        if (myCount)
        {
            auto title = std::format(wxS("{}/{}"), waitingCount, myCount);

            const auto hasFailedBuilds = std::ranges::any_of(
                m_pullRequestsInfo.myPullRequests,
                [](const auto& pullRequest)
                {
                    return pullRequest.HasBuildsWithStatus(StatusState::Failed);
                });
            const auto hasSomeoneRequestedChanges = std::ranges::any_of(
                m_pullRequestsInfo.myPullRequests,
                [](const auto& pullRequest)
                {
                    return std::ranges::any_of(
                        pullRequest.pullRequest.participants,
                        [](const auto& participant)
                        {
                            return participant.state == ParticipantState::ChangesRequested;
                        });
                });

            if (hasFailedBuilds || hasSomeoneRequestedChanges)
                title += wxS(" (!)");

            SetStatusItemTitle(title);
        }
        else
        {
            SetStatusItemTitle(std::format(wxS("{}"), waitingCount));
        }
    }
    else
    {
        SetStatusItemTitle(wxEmptyString);
    }
}

void StatusItem::ConfigChanged()
{
    if (!Config::GetHideChangesRequestedPullRequests())
        m_showAllPullRequests = false;

    RefreshPullRequestsWindow();
    if (m_hasPullRequests)
        UpdateTitle();
}

wxMenu* StatusItem::GetPopupMenu()
{
    return m_pCommandMenu.get();
}

void StatusItem::SetStatusItemTitle(const wxString& title)
{
#if defined(__WXOSX__)
    SetTitle(title);
#else
    const auto tooltipTitle = title.IsEmpty() ? wxS("none") : title;
    SetIcon(m_bitmapBundle, std::format(wxS("Pull requests: {}"), tooltipTitle));
#endif
}
