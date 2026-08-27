#pragma once

#include <span>
#include <unordered_map>

#include "../pull_requests/PullRequestInfo.h"
#include "../pull_requests/PullRequestsInfo.h"

class MenuBuilder;
class MenuMetrics;
struct PullRequestMenuEntry;
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
        const wxMenuItem& firstStaticMenuItem,
        const PullRequestsInfo& pullRequests,
        bool hideChangesRequestedPullRequests,
        bool useSubmenusOnMenuOverflow,
        bool displayRepositoryNameLowercase
    ) const;

private:
    wxMenu& m_menu;

    wxMenuItem* InsertPullRequestTitleMenuItem(MenuBuilder& menuBuilder, const PullRequestInfo& pullRequest) const;
    void InsertSecondaryPullRequestMenuItem(MenuBuilder& menuBuilder, const wxString& title) const;
    void InsertEntry(
        MenuBuilder& menuBuilder,
        const PullRequestMenuEntry& entry,
        PullRequestsMenuBuildResult& result
    ) const;
    void InsertAllEntries(
        MenuBuilder& menuBuilder,
        std::span<const PullRequestMenuEntry> entries,
        PullRequestsMenuBuildResult& result
    ) const;
    int InsertEntriesWithOverflow(
        MenuBuilder& menuBuilder,
        std::span<const PullRequestMenuEntry> entries,
        int availableHeight,
        const MenuMetrics& menuMetrics,
        PullRequestsMenuBuildResult& result
    ) const;
    void RemoveDynamicMenuItems(const wxMenuItem& firstStaticMenuItem) const;
};
