#include "../menu/PullRequestsMenuLayout.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include <wx/menu.h>

#include "../menu/MenuBuilder.h"
#include "../menu/MenuMetrics.h"
#include "../menu/PullRequestsMenuBuilder.h"

namespace
{
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

int PullRequestsMenuLayout::InsertEntriesWithOverflow(
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

wxMenuItem* PullRequestsMenuLayout::BuildSections(
    std::vector<PullRequestMenuEntry> waitingForApprovalEntries,
    std::vector<PullRequestMenuEntry> myEntries,
    const bool useSubmenusOnMenuOverflow,
    PullRequestsMenuBuildResult& result
) const
{
    MenuBuilder menuBuilder(m_menu);
    auto* firstMenuItem = menuBuilder.InsertDisabledItem("Pull requests to review");
    const auto menuMetrics = MenuMetrics::Measure();
    SplitOversizedEntries(waitingForApprovalEntries, menuMetrics);
    SplitOversizedEntries(myEntries, menuMetrics);

    if (useSubmenusOnMenuOverflow)
    {
        const auto staticPartMenuHeight = menuMetrics.MeasureMenuHeight(m_menu);

        auto availableHeight = std::max(0, menuMetrics.maximumHeight - staticPartMenuHeight);

        // Leave the room for a "More..." item in "my pull requests"
        const auto myPullRequestsMinimumHeight = myEntries.empty() ? 0 : menuMetrics.itemHeight;
        const auto reviewAvailableHeight = std::max(0, availableHeight - myPullRequestsMinimumHeight);

        availableHeight -= InsertEntriesWithOverflow(
            menuBuilder,
            waitingForApprovalEntries,
            reviewAvailableHeight,
            menuMetrics,
            result);

        menuBuilder.InsertSeparator();
        menuBuilder.InsertDisabledItem("Your pull requests");

        InsertEntriesWithOverflow(
            menuBuilder,
            myEntries,
            std::max(0, availableHeight),
            menuMetrics,
            result);
    }
    else
    {
        InsertAllEntries(menuBuilder, waitingForApprovalEntries, result);
        menuBuilder.InsertSeparator();
        menuBuilder.InsertDisabledItem("Your pull requests");
        InsertAllEntries(menuBuilder, myEntries, result);
    }

    return firstMenuItem;
}
