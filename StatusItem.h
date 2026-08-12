#pragma once

#include <thread>
#include <vector>

#include <wx/bmpbndl.h>
#include <wx/taskbar.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include "bitbucket_api/include/bitbucket_api/Structs.h"
#include "pull_requests/PullRequestsInfo.h"

class PreferencesWindow;
class PullRequestsWindow;

struct OnUpdatePullRequestsArgs
{
    bool showNotification{};
    bool fullReload{};
};

class StatusItem : public wxTaskBarIcon
{
public:
    explicit StatusItem();
    ~StatusItem() override;

    void ConfigChanged();

private:
    PreferencesWindow* m_pDialog{};
    PullRequestsWindow* m_pPullRequestsWindow{};
    std::unique_ptr<wxMenu> m_pCommandMenu;
    std::unique_ptr<wxTimer> m_pTimer;

    PullRequestsInfo m_pullRequestsInfo;
    std::vector<Repository> m_repositories;
    bool m_hasPullRequests{};
    bool m_showAllPullRequests{};

    wxMenu* GetPopupMenu() override;

    void SetStatusItemTitle(const wxString& title);
    void ShowPullRequestsWindow();
    void ShowPreferencesDialog() const;
    void ShowCreatePullRequestDialog();
    void RefreshPullRequestsWindow();
    size_t GetHiddenPullRequestsCount() const;

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnRightButtonClick(wxTaskBarIconEvent&);
    void OnMenuItemClick(wxCommandEvent&);

    void ShowErrorNotification(const wxString& message) const;
    void UpdatePullRequests(const OnUpdatePullRequestsArgs& args);
    void UpdateStatistics(size_t processedPullRequestsCount, size_t fetchedPullRequestsCount, std::chrono::seconds elapsedTime);
    void UpdateTitle();

#ifdef __WXOSX__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#else
    wxBitmap m_statusBitmap{};
    wxBitmapBundle m_bitmapBundle{};
#endif

    std::jthread m_thread;
};
