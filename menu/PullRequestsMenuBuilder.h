#pragma once

#include <unordered_map>

#include "../pull_requests/PullRequestInfo.h"
#include "../pull_requests/PullRequestsInfo.h"

class MenuBuilder;
class wxMenu;
class wxMenuItem;
class wxString;

struct PullRequestsMenuBuildResult
{
    std::unordered_map<int, PullRequestInfo> menuItemIdToPullRequest;
    int hiddenPullRequestsCount{};
};

class PullRequestsMenuBuilder
{
public:
    explicit PullRequestsMenuBuilder(wxMenu& menu);

    [[nodiscard]] PullRequestsMenuBuildResult Rebuild(
        int firstDynamicMenuItemId,
        const PullRequestsInfo& pullRequests,
        bool hideChangesRequestedPullRequests,
        bool useTwoColumnLayout
    ) const;

private:
    wxMenu& m_menu;

    wxMenuItem* InsertPullRequestTitleMenuItem(MenuBuilder& menuBuilder, const PullRequestInfo& pullRequest) const;
    void InsertSecondaryPullRequestMenuItem(MenuBuilder& menuBuilder, const wxString& title) const;
    void RemoveDynamicMenuItems(int firstDynamicMenuItemId) const;
};
