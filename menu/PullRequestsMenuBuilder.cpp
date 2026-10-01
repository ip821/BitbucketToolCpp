#include "PullRequestsMenuBuilder.h"

#include <format>
#include <utility>

#include <cpp_utils/wx_string_format.h>
#include <wx/menu.h>

#include "PullRequestMenuEntry.h"
#include "PullRequestsMenuLayout.h"

PullRequestsMenuBuilder::PullRequestsMenuBuilder(wxMenu& menu) : m_menu(menu)
{
}

PullRequestsMenuBuildResult PullRequestsMenuBuilder::Rebuild(
    const wxMenuItem& firstStaticMenuItem,
    const PullRequestsInfo& pullRequests,
    const RebuildOptions& options
) const
{
    RemoveDynamicMenuItems(firstStaticMenuItem);

    const PullRequestMenuEntryFactory menuEntryFactory(pullRequests);

    PullRequestsMenuBuildResult result;

    auto waitingForApprovalEntriesResult = menuEntryFactory.GetWaitingMyApprovalMenuEntries({
        .hideChangesRequestedPullRequests = options.hideChangesRequestedPullRequests,
        .hideFeaturePullRequests = options.hideFeaturePullRequests,
        .displayRepositoryNameLowercase = options.displayRepositoryNameLowercase,
    });
    result.hiddenPullRequestsCount += waitingForApprovalEntriesResult.hiddenPullRequestsCount;
    auto& waitingForApprovalMenuEntries = waitingForApprovalEntriesResult.entries;

    auto myMenuEntriesResult = menuEntryFactory.GetMyMenuEntries(options.displayRepositoryNameLowercase);
    result.hiddenPullRequestsCount += myMenuEntriesResult.hiddenPullRequestsCount;
    auto& myMenuEntries = myMenuEntriesResult.entries;

    const PullRequestsMenuLayout layout(m_menu);
    auto* pFirstMenuItem = layout.BuildSections(
        std::move(waitingForApprovalMenuEntries),
        std::move(myMenuEntries),
        options.useSubmenusOnMenuOverflow,
        result);

    if (result.hiddenPullRequestsCount)
    {
        const auto firstMenuItemTitle = std::format(wxS("{} [{} hidden]"), pFirstMenuItem->GetItemLabel(), result.hiddenPullRequestsCount);
        pFirstMenuItem->SetItemLabel(firstMenuItemTitle);
    }

    return result;
}

void PullRequestsMenuBuilder::RemoveDynamicMenuItems(const wxMenuItem& firstStaticMenuItem) const
{
    for (const auto menuItems = m_menu.GetMenuItems();
         const auto& item: menuItems)
    {
        if (item == &firstStaticMenuItem)
            break;

        m_menu.Delete(item);
    }
}
