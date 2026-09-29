#include "StatusItem.h"

#include <algorithm>
#include <memory>
#include <ranges>
#include <thread>
#include <utility>
#include <cpp_utils/match_variant.h>
#include <cpp_utils/wx_string_format.h>
#include <wx/clipbrd.h>
#include <wx/notifmsg.h>
#include <wx/wx.h>

#include "bitbucket_api/include/bitbucket_api/Requests.h"
#include "preferences/PreferencesWindow.h"
#include "preferences/settings/Config.h"
#include "pull_requests/PullRequestUpdateCompletedThreadEvent.h"
#include "pull_requests/PullRequestUpdateProgressThreadEvent.h"
#include "pull_requests/PullRequestService.h"
#include "Stopwatch.h"

constexpr auto tenSeconds = 10 * 1000;
constexpr auto fiveMinutes = 5 * 60 * 1000;
constexpr auto immediateUpdateDelay = 1;

StatusItem::StatusItem() :
    wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM),
    m_menu({
        .onUpdateRequested = [this](const bool fullReload)
        {
            UpdatePullRequests({.showNotification = true, .fullReload = fullReload});
        },
        .onPreferencesRequested = [this]
        {
            ShowPreferencesDialog();
        },
        .onQuitRequested = []
        {
            wxExit();
        },
        .onToggleHiddenRequested = [this]
        {
            RefreshMenu();
        },
        .onOpenPullRequestRequested = [](const wxString& href, const bool copyToClipboard)
        {
            OpenPullRequest(href, copyToClipboard);
        },
        .onCreatePullRequestRequested = [](const wxString& repository)
        {
            CreatePullRequest(repository);
        },
    })
{
    m_pDialog = new PreferencesWindow(this);

    m_platform.Initialize(*this, [this]
    {
        PopupMenu(m_menu.GetMenu());
    });
    Bind(PullRequestUpdateProgressThreadEvent::EventType, &StatusItem::OnPullRequestUpdateProgress, this);
    Bind(PullRequestUpdateCompletedThreadEvent::EventType, &StatusItem::OnPullRequestUpdateCompleted, this);

    m_pTimer = std::make_unique<wxTimer>(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        UpdatePullRequests({.showNotification = false});
    });

    ApplyConfig();

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

void StatusItem::ApplyConfig()
{
    m_showAllPullRequests = Config::GetHideChangesRequestedPullRequests() || Config::GetHideFeaturePullRequests();
    RefreshMenu();
}

void StatusItem::OpenPullRequest(const wxString& href, const bool copyToClipboard)
{
    if (copyToClipboard)
    {
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(new wxTextDataObject(href));
            wxTheClipboard->Close();
        }
        return;
    }

    wxLaunchDefaultBrowser(href);
}

void StatusItem::CreatePullRequest(const wxString& repository)
{
    const wxString bitbucketHostname = wxS("https://bitbucket.org");
    const auto url = bitbucketHostname +
        wxS("/") + repository +
        wxS("/pull-requests/new");

    wxLaunchDefaultBrowser(url);
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
    const auto hideFeaturePullRequests = !showAll && Config::GetHideFeaturePullRequests();
    const auto useSubmenusOnMenuOverflow = Config::GetUseSubmenusOnMenuOverflow();
    const auto displayRepositoryNameLowercase = Config::GetDisplayRepositoryNameLowercase();

    const auto hiddenPullRequestsCount = m_menu.RebuildPullRequests(
        pullRequestsInfo,
        RebuildOptions{
            .hideChangesRequestedPullRequests = hideChangesRequestedPullRequests,
            .hideFeaturePullRequests = hideFeaturePullRequests,
            .useSubmenusOnMenuOverflow = useSubmenusOnMenuOverflow,
            .displayRepositoryNameLowercase = displayRepositoryNameLowercase,
        });

    UpdateTitle(pullRequestsInfo, hiddenPullRequestsCount);
}

void StatusItem::UpdatePullRequests(const OnUpdatePullRequestsArgs& args)
{
    m_menu.SetUpdateEnabled(false);

    const auto repositories = Config::GetRepositories();

    m_menu.SetRepositories(repositories);

    m_thread = std::jthread([this, args, repositories](const std::stop_token stopToken)
    {
        const Stopwatch fetchStopwatch;

        const auto progressCallback = [this, stopToken](const PullRequestUpdateProgressArgs& progressArgs)
        {
            if (stopToken.stop_requested())
                return;

            const auto event = new PullRequestUpdateProgressThreadEvent(stopToken, progressArgs);
            QueueEventToMessageLoop(event);
        };

        PullRequestService pullRequestService(progressCallback, stopToken);
        auto pullRequestsResult = pullRequestService.GetPullRequests(repositories);

        if (stopToken.stop_requested())
            return;

        const auto elapsedTime = fetchStopwatch.GetElapsed();

        const auto event = new PullRequestUpdateCompletedThreadEvent(
            stopToken,
            args.showNotification,
            elapsedTime,
            std::move(pullRequestsResult));

        QueueEventToMessageLoop(event);
    });
}

void StatusItem::OnPullRequestUpdateProgress(wxThreadEvent& event)
{
    const auto& updateEvent = static_cast<PullRequestUpdateProgressThreadEvent&>(event);
    if (!updateEvent.IsCancelled())
        UpdateProgress(updateEvent.GetProgressArgs());
}

void StatusItem::OnPullRequestUpdateCompleted(wxThreadEvent& event)
{
    const auto& updateEvent = static_cast<PullRequestUpdateCompletedThreadEvent&>(event);
    if (updateEvent.IsCancelled())
        return;

    m_pTimer->StartOnce(fiveMinutes);
    m_menu.SetUpdateEnabled(true);

    const auto& result = updateEvent.GetResult();
    if (!result)
    {
        const auto message = result.error().message;
        m_menu.SetStatisticsLabel(std::format("Update failed: {}", message));
        ShowErrorNotification(message);
        return;
    }

    const auto& pullRequestsInfo = result.value();
    m_pullRequestsInfo = pullRequestsInfo;

    UpdateStatistics(
        pullRequestsInfo.processedPullRequestsCount,
        pullRequestsInfo.fetchedPullRequestsCount,
        updateEvent.GetElapsedTime());
    RebuildMenu({.pullRequests = pullRequestsInfo, .showAll = false});

    if (updateEvent.ShouldShowNotification())
    {
        wxNotificationMessage notification("BitbucketTool", "Pull requests were updated");
        notification.Show();
    }
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

    m_menu.SetStatisticsLabel(label);
}

void StatusItem::UpdateStatistics(size_t processedPullRequestsCount, size_t fetchedPullRequestsCount, std::chrono::seconds elapsedTime)
{
    const auto elapsedSeconds = elapsedTime.count();
    const auto nextUpdate = wxDateTime::Now() + wxTimeSpan::Milliseconds(fiveMinutes);
    m_menu.SetStatisticsLabel(
        std::format(
            wxS("PRs fetched: {}/{}. Took: {:02}:{:02}. Next: {}"),
            processedPullRequestsCount,
            fetchedPullRequestsCount,
            elapsedSeconds / 60,
            elapsedSeconds % 60,
            nextUpdate.Format(wxS("%d.%m.%Y %H:%M"))));
}

void StatusItem::UpdateTitle(const PullRequestsInfo& pullRequestsInfo, const int hiddenPullRequestsCount)
{
    const auto waitingCount = pullRequestsInfo.waitingForMyApprovalPullRequests.size() - hiddenPullRequestsCount;

    const auto hasFailedBuilds = pullRequestsInfo.myPullRequests
        | ip::ranges::any_of([](const auto& it) { return it.HasBuildsWithStatus(StatusState::Failed); });

    const auto hasSomeoneRequestedChanges = pullRequestsInfo.myPullRequests
        | std::views::transform([](const auto& it) { return it.pullRequest.participants; })
        | std::views::join
        | ip::ranges::any_of([](const auto& p) { return p.state == ParticipantState::ChangesRequested; });

    const auto hasAlert = hasFailedBuilds || hasSomeoneRequestedChanges;

    const auto myCount = pullRequestsInfo.myPullRequests.size();
    m_platform.UpdateTitle(*this, waitingCount, myCount, hasAlert);
}

void StatusItem::QueueEventToMessageLoop(wxEvent* event)
{
    wxQueueEvent(this, event);
    m_platform.OnEventQueued();
}

void StatusItem::ConfigChanged()
{
    ApplyConfig();
    m_pTimer->StartOnce(immediateUpdateDelay);
}

wxMenu* StatusItem::GetPopupMenu()
{
    return m_menu.GetMenu();
}
