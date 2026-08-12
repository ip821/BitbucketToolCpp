#pragma once

#include <thread>
#include <wx/bmpbndl.h>
#include <wx/taskbar.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include "preferences/PreferencesWindow.h"
#include "pull_requests/PullRequestInfo.h"
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
    std::unique_ptr<wxMenu> m_pMenu;
    wxMenu *m_pCreatePullRequestsMenu{};
    std::unique_ptr<wxTimer> m_pTimer;

    PullRequestsInfo m_pullRequestsInfo;
    bool m_showAllPullRequests{};
    bool m_useTwoColumnLayout{};
    std::unordered_map<int, PullRequestInfo> m_menuItemIdToPullRequest;
    std::unordered_map<int, wxString> m_menuItemIdToRepository;

    wxMenu *GetPopupMenu() override;

    void SetStatusItemTitle(const wxString& title);

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuItemClick(wxCommandEvent&);
    void OnCreatePullRequestMenuItemClick(wxCommandEvent&);

    void ShowPreferencesDialog() const;
    void RefreshMenu();
    void UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories);
    void ShowErrorNotification(const wxString& message) const;
    void RebuildMenu(const RebuildMenuArgs& args);
    void UpdatePullRequests(const OnUpdatePullRequestsArgs& args);
    void UpdateProgress(const PullRequestUpdateProgressArgs& progress);
    void UpdateStatistics(size_t processedPullRequestsCount, size_t fetchedPullRequestsCount, std::chrono::seconds elapsedTime);
    void UpdateTitle(const PullRequestsInfo& pullRequestsInfo, int hiddenPullRequestsCount);

#ifdef __WXOSX__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif

#ifndef __WXOSX__
    wxBitmap m_statusBitmap{};
    wxBitmapBundle m_bitmapBundle{};
#endif

    std::jthread m_thread;

public:
    explicit StatusItem();
    void ConfigChanged();
};
