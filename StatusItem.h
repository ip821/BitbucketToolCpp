#pragma once

#include <wx/taskbar.h>
#include <wx/bmpbndl.h>
#include <wx/timer.h>

#include "pull_requests/PullRequestInfo.h"
#include "webrequests/Repository.h"

struct OnUpdatePullRequestsArgs
{
    bool showNotification{};
};

class PreferencesWindow;

class StatusItem : public wxTaskBarIcon
{
    PreferencesWindow* m_pDialog{};
    wxMenu* m_pMenu{};
    wxMenu* m_pCreatePullRequestsMenu{};
    wxTimer* m_pTimer{};

    std::vector<Repository> m_repositories{};
    std::unordered_map<int, PullRequestInfo> m_menuItemIdToPullRequest{};

    wxMenu* GetPopupMenu() override;

    void SetStatusItemTitle(const wxString& title);

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuItemClick(wxCommandEvent&);
    void OnMenuCreatePr(wxCommandEvent&);

    void RemoveAllPrMenuItems();
    void ShowPreferencesDialog() const;
    void UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories);
    void ShowErrorNotification(const wxString& message) const;
    void OnUpdatePullRequests(const OnUpdatePullRequestsArgs& args);
    void UpdateTitle(const PullRequestsInfo& pullRequestsInfo);

#ifdef __WXOSX__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif

#ifndef __WXOSX__
    wxBitmap m_statusBitmap{};
    wxBitmapBundle m_bitmapBundle{};
#endif

public:
    explicit StatusItem();
};
