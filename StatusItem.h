#pragma once

#include <thread>
#include <wx/bmpbndl.h>
#include <wx/taskbar.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include "menu/StatusMenu.h"
#include "preferences/PreferencesWindow.h"
#include "pull_requests/PullRequestService.h"
#include "pull_requests/PullRequestsInfo.h"

struct OnUpdatePullRequestsArgs
{
    bool showNotification{};
    bool fullReload{};
};

struct RebuildMenuArgs
{
    const PullRequestsInfo& pullRequests;
    const bool showAll;
};

class StatusItem : public wxTaskBarIcon
{
    PreferencesWindow *m_pDialog{};
    StatusMenu m_menu;
    std::unique_ptr<wxTimer> m_pTimer;

    PullRequestsInfo m_pullRequestsInfo;
    bool m_showAllPullRequests{};

    wxMenu *GetPopupMenu() override;

    void SetStatusItemTitle(const wxString& title, bool hasAlert = false);

    void OnLeftButtonClick(wxTaskBarIconEvent&);

    void ShowPreferencesDialog() const;
    void RefreshMenu();
    static void OpenPullRequest(const wxString& href, bool copyToClipboard);
    static void CreatePullRequest(const wxString& repository);
    void ShowErrorNotification(const wxString& message) const;
    void RebuildMenu(const RebuildMenuArgs& args);
    void UpdatePullRequests(const OnUpdatePullRequestsArgs& args);
    void UpdateProgress(const PullRequestUpdateProgressArgs& progress);
    void UpdateStatistics(size_t processedPullRequestsCount, size_t fetchedPullRequestsCount, std::chrono::seconds elapsedTime);
    void UpdateTitle(const PullRequestsInfo& pullRequestsInfo, int hiddenPullRequestsCount);

#ifdef __WXOSX__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif

#if !defined(__WXOSX__) && !defined(__WXMSW__)
    wxBitmap m_statusBitmap{};
#endif

#ifndef __WXOSX__
    wxBitmapBundle m_bitmapBundle{};
#endif

    std::jthread m_thread;

public:
    explicit StatusItem();
    ~StatusItem() override;
    void ConfigChanged();
};
