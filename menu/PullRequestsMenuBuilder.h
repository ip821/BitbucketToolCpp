#pragma once

#include <unordered_map>

#include "../pull_requests/PullRequestInfo.h"
#include "../pull_requests/PullRequestsInfo.h"

class wxMenu;
class wxMenuItem;

struct PullRequestsMenuBuildResult
{
    std::unordered_map<int, PullRequestInfo> menuItemIdToPullRequest;
    int hiddenPullRequestsCount{};
};

struct RebuildOptions
{
    bool hideChangesRequestedPullRequests{};
    bool hideFeaturePullRequests{};
    bool useSubmenusOnMenuOverflow{};
    bool displayRepositoryNameLowercase{};
};

class PullRequestsMenuBuilder
{
public:
    explicit PullRequestsMenuBuilder(wxMenu& menu);

    [[nodiscard]] PullRequestsMenuBuildResult Rebuild(
        const wxMenuItem& firstStaticMenuItem,
        const PullRequestsInfo& pullRequests,
        const RebuildOptions& options
    ) const;

private:
    wxMenu& m_menu;

    void RemoveDynamicMenuItems(const wxMenuItem& firstStaticMenuItem) const;
};
