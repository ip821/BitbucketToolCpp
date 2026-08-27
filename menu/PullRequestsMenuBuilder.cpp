#include "PullRequestsMenuBuilder.h"

#include <algorithm>
#include <iterator>
#include <span>
#include <vector>
#include <format>

#include <cpp_utils/wx_string_format.h>
#include <wx/control.h>
#include <wx/dcmemory.h>
#include <wx/display.h>
#include <wx/menu.h>
#include <wx/settings.h>

#include "MenuBuilder.h"
#include "MenuMetrics.h"
#include "PullRequestMenuEntry.h"

namespace
{
    constexpr auto maxMenuWidth = 500;

    wxString FitMenuText(const wxString& text)
    {
        wxBitmap bitmap(1, 1);
        wxMemoryDC dc(bitmap);
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        return wxControl::Ellipsize(text, dc, wxELLIPSIZE_END, maxMenuWidth);
    }

    void SplitOversizedEntries(std::vector<PullRequestMenuEntry>& entries, const MenuMetrics& metrics)
    {
        const auto maximumRowsPerMenu = std::max(2, metrics.maximumHeight / metrics.itemHeight);
        const auto maximumRowsPerChunk = maximumRowsPerMenu - 1;
        const auto maximumChunkHeight = maximumRowsPerChunk * metrics.itemHeight;

        std::vector<PullRequestMenuEntry> splitEntries;
        splitEntries.reserve(entries.size());

        for (auto& entry: entries)
        {
            if (metrics.MeasureEntryHeight(entry) <= maximumChunkHeight)
            {
                splitEntries.push_back(std::move(entry));
                continue;
            }

            auto secondaryTitleIndex = std::size_t{};
            auto includesTitle = true;

            while (includesTitle || secondaryTitleIndex < entry.secondaryTitles.size())
            {
                const auto titleRows = includesTitle ? std::size_t{1} : std::size_t{};
                const auto secondaryTitleCount = std::min(
                    maximumRowsPerChunk - titleRows,
                    entry.secondaryTitles.size() - secondaryTitleIndex);

                PullRequestMenuEntry chunk{
                    .pullRequest = entry.pullRequest,
                    .includesTitle = includesTitle,
                    .secondaryTitles = {},
                };
                chunk.secondaryTitles.insert(
                    chunk.secondaryTitles.end(),
                    std::make_move_iterator(entry.secondaryTitles.begin() + secondaryTitleIndex),
                    std::make_move_iterator(entry.secondaryTitles.begin() + secondaryTitleIndex + secondaryTitleCount));
                splitEntries.push_back(std::move(chunk));

                secondaryTitleIndex += secondaryTitleCount;
                includesTitle = false;
            }
        }

        entries = std::move(splitEntries);
    }
}

PullRequestsMenuBuilder::PullRequestsMenuBuilder(wxMenu& menu) : m_menu(menu)
{
}

wxMenuItem* PullRequestsMenuBuilder::InsertPullRequestTitleMenuItem(MenuBuilder& menuBuilder, const PullRequestInfo& pullRequest) const
{
    return menuBuilder.InsertItem(FitMenuText(pullRequest.GetMainMenuItemTitle()));
}

void PullRequestsMenuBuilder::InsertSecondaryPullRequestMenuItem(MenuBuilder& menuBuilder, const wxString& title) const
{
    const auto secondLineTitle = std::format(wxS("   {}"), title);
    menuBuilder.InsertDisabledItem(FitMenuText(secondLineTitle));
}

void PullRequestsMenuBuilder::InsertEntry(
    MenuBuilder& menuBuilder,
    const PullRequestMenuEntry& entry,
    PullRequestsMenuBuildResult& result
) const
{
    if (entry.includesTitle)
    {
        const auto* pullRequest = entry.pullRequest;
        const auto pMenuItem = InsertPullRequestTitleMenuItem(menuBuilder, *pullRequest);
        result.menuItemIdToPullRequest[pMenuItem->GetId()] = *pullRequest;
    }

    for (const auto& title: entry.secondaryTitles)
    {
        InsertSecondaryPullRequestMenuItem(menuBuilder, title);
    }
}

void PullRequestsMenuBuilder::InsertAllEntries(
    MenuBuilder& menuBuilder,
    const std::span<const PullRequestMenuEntry> entries,
    PullRequestsMenuBuildResult& result
) const
{
    for (const auto& entry: entries)
    {
        InsertEntry(menuBuilder, entry, result);
    }
}

int PullRequestsMenuBuilder::InsertEntriesWithOverflow(
    MenuBuilder& menuBuilder,
    const std::span<const PullRequestMenuEntry> entries,
    const int availableHeight,
    const MenuMetrics& menuMetrics,
    PullRequestsMenuBuildResult& result
) const
{
    if (entries.empty())
        return 0;

    const auto visibleCount = menuMetrics.GetVisibleEntryCount(entries, availableHeight);

    auto usedHeight = 0;
    for (const auto& entry: entries.first(visibleCount))
    {
        InsertEntry(menuBuilder, entry, result);
        usedHeight += menuMetrics.MeasureEntryHeight(entry);
    }

    if (visibleCount < entries.size())
    {
        auto* overflowMenu = menuBuilder.InsertSubMenu(wxS("More..."));
        usedHeight += menuMetrics.itemHeight;

        MenuBuilder overflowMenuBuilder(*overflowMenu);
        InsertEntriesWithOverflow(
            overflowMenuBuilder,
            entries.subspan(visibleCount),
            menuMetrics.maximumHeight,
            menuMetrics,
            result);
    }

    return usedHeight;
}

PullRequestsMenuBuildResult PullRequestsMenuBuilder::Rebuild(
    const wxMenuItem& firstStaticMenuItem,
    const PullRequestsInfo& pullRequests,
    const bool hideChangesRequestedPullRequests,
    const bool useSubmenusOnMenuOverflow,
    const bool displayRepositoryNameLowercase
) const
{
    RemoveDynamicMenuItems(firstStaticMenuItem);

    const PullRequestMenuEntryFactory menuEntryFactory(pullRequests);

    PullRequestsMenuBuildResult result;

    auto waitingForApprovalEntriesResult = menuEntryFactory.GetWaitingMyApprovalMenuEntries({
        .hideChangesRequestedPullRequests = hideChangesRequestedPullRequests,
        .displayRepositoryNameLowercase = displayRepositoryNameLowercase,
    });
    result.hiddenPullRequestsCount += waitingForApprovalEntriesResult.hiddenPullRequestsCount;
    auto& waitingForApprovalMenuEntries = waitingForApprovalEntriesResult.entries;

    auto myMenuEntriesResult = menuEntryFactory.GetMyMenuEntries(
        displayRepositoryNameLowercase
    );
    result.hiddenPullRequestsCount += myMenuEntriesResult.hiddenPullRequestsCount;
    auto& myMenuEntries = myMenuEntriesResult.entries;

    const auto menuMetrics = MenuMetrics::Measure();
    SplitOversizedEntries(waitingForApprovalMenuEntries, menuMetrics);
    SplitOversizedEntries(myMenuEntries, menuMetrics);

    MenuBuilder menuBuilder(m_menu);

    const auto pFirstMenuItem = menuBuilder.InsertDisabledItem("Pull requests to review");

    if (useSubmenusOnMenuOverflow)
    {
        const auto staticPartMenuHeight = menuMetrics.MeasureMenuHeight(m_menu);

        auto availableHeight = std::max(0, menuMetrics.maximumHeight - staticPartMenuHeight);

        // Leave the room for a "More..." item in "my pull requests"
        const auto myPullRequestsMinimumHeight = myMenuEntries.empty() ? 0 : menuMetrics.itemHeight;
        const auto reviewAvailableHeight = std::max(0, availableHeight - myPullRequestsMinimumHeight);

        availableHeight -= InsertEntriesWithOverflow(
            menuBuilder,
            waitingForApprovalMenuEntries,
            reviewAvailableHeight,
            menuMetrics,
            result);

        menuBuilder.InsertSeparator();
        menuBuilder.InsertDisabledItem("Your pull requests");

        InsertEntriesWithOverflow(
            menuBuilder,
            myMenuEntries,
            std::max(0, availableHeight),
            menuMetrics,
            result);
    }
    else
    {
        InsertAllEntries(menuBuilder, waitingForApprovalMenuEntries, result);
        menuBuilder.InsertSeparator();
        menuBuilder.InsertDisabledItem("Your pull requests");
        InsertAllEntries(menuBuilder, myMenuEntries, result);
    }

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
