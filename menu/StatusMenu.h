#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wx/string.h>

#include "../bitbucket_api/include/bitbucket_api/Structs.h"
#include "../pull_requests/PullRequestInfo.h"
#include "../pull_requests/PullRequestsInfo.h"

class wxCommandEvent;
class wxMenu;
class wxMenuItem;

struct StatusMenuCallbacks
{
    std::function<void(bool fullReload)> onUpdateRequested;
    std::function<void()> onPreferencesRequested;
    std::function<void()> onQuitRequested;
    std::function<void()> onToggleHiddenRequested;
    std::function<void(const wxString& href, bool copyToClipboard)> onOpenPullRequestRequested;
    std::function<void(const wxString& repository)> onCreatePullRequestRequested;
};

class StatusMenu
{
public:
    explicit StatusMenu(StatusMenuCallbacks callbacks);
    ~StatusMenu();

    StatusMenu(const StatusMenu&) = delete;
    StatusMenu& operator=(const StatusMenu&) = delete;
    StatusMenu(StatusMenu&&) = delete;
    StatusMenu& operator=(StatusMenu&&) = delete;

    [[nodiscard]] wxMenu* GetMenu() const;

    void SetRepositories(const std::vector<Repository>& repositories);
    [[nodiscard]] int RebuildPullRequests(
        const PullRequestsInfo& pullRequests,
        bool hideChangesRequestedPullRequests,
        bool useSubmenusOnMenuOverflow,
        bool displayRepositoryNameLowercase
    );

    void SetUpdateEnabled(bool enabled);
    void SetStatisticsLabel(const wxString& label);

private:
    void OnMenuItemClick(wxCommandEvent& event);

    StatusMenuCallbacks m_callbacks;
    std::unique_ptr<wxMenu> m_menu;

    // Owned by m_menu.
    wxMenu* m_createPullRequestMenu{};
    wxMenuItem* m_firstStaticMenuItem{};
    wxMenuItem* m_statisticsMenuItem{};
    wxMenuItem* m_showAllMenuItem{};
    wxMenuItem* m_updateMenuItem{};
    wxMenuItem* m_preferencesMenuItem{};
    wxMenuItem* m_quitMenuItem{};

    std::unordered_map<int, PullRequestInfo> m_menuItemIdToPullRequest;
    std::unordered_map<int, wxString> m_menuItemIdToRepository;
};
