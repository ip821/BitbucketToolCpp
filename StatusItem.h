#pragma once

#include <thread>

#include <wx/taskbar.h>
#include <wx/bmpbndl.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include "preferences/PreferencesWindow.h"
#include "pull_requests/PullRequestInfo.h"
#include "webrequests/Repository.h"

struct OnUpdatePullRequestsArgs
{
    bool showNotification{};
};

struct IdAndIndex
{
    int id{};
    int index{};
};

class StatusItem : public wxTaskBarIcon
{
    PreferencesWindow *m_pDialog{};
    std::unique_ptr<wxMenu> m_pMenu;
    wxMenu *m_pCreatePullRequestsMenu{};
    std::unique_ptr<wxTimer> m_pTimer;

    std::unordered_map<int, PullRequestInfo> m_menuItemIdToPullRequest;

    wxMenu *GetPopupMenu() override;

    void SetStatusItemTitle(const wxString& title);

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuItemClick(wxCommandEvent&);
    void OnCreatePullRequestMenuItemClick(wxCommandEvent&);

    void RemoveAllPrMenuItems();
    void ShowPreferencesDialog() const;
    void UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories);
    void ShowErrorNotification(const wxString& message) const;
    void UpdatePullRequests(const OnUpdatePullRequestsArgs& args);
    void InsertPullRequestTitleMenuItem(IdAndIndex& menuItemId, const PullRequestInfo& pullRequest) const;
    void InsertSecondaryPullRequestMenuItem(IdAndIndex& menuItemId, const wxString& title) const;
    void UpdateTitle(const PullRequestsInfo& pullRequestsInfo);

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
};
