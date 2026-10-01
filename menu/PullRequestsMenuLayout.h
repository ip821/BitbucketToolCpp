#pragma once

#include <span>
#include <vector>

#include "PullRequestMenuEntry.h"

class MenuBuilder;
class MenuMetrics;
struct PullRequestsMenuBuildResult;
class wxMenu;
class wxMenuItem;
class wxString;

class PullRequestsMenuLayout
{
public:
    explicit PullRequestsMenuLayout(wxMenu& menu);

    wxMenuItem* BuildSections(
        std::vector<PullRequestMenuEntry> waitingForApprovalEntries,
        std::vector<PullRequestMenuEntry> myEntries,
        bool useSubmenusOnMenuOverflow,
        PullRequestsMenuBuildResult& result
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
};
