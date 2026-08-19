#pragma once

#include <wx/display.h>
#include <wx/wx.h>

#include "PullRequestMenuEntry.h"

class MenuMetrics
{
    static constexpr auto HeightAdditionValue = 40;
    static constexpr auto menuVerticalBorderAllowance = 8;

public:
    int itemHeight{};
    int separatorHeight{};
    int maximumHeight{};

    static MenuMetrics Measure()
    {
        wxBitmap bitmap(1, 1);
        wxMemoryDC dc(bitmap);
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        const auto textHeight = dc.GetTextExtent(wxS("Ag")).GetHeight();
        const auto systemMenuHeight = wxSystemSettings::GetMetric(wxSYS_MENU_Y);
        const auto itemHeight = std::max({1, textHeight + 8, systemMenuHeight});
        const auto separatorHeight = std::max(1, itemHeight / 2);

        const wxDisplay display;
        const auto displayHeight = display.IsOk()
            ? display.GetClientArea().GetHeight() + HeightAdditionValue
            : wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

        return {
            .itemHeight = itemHeight,
            .separatorHeight = separatorHeight,
            .maximumHeight = std::max(itemHeight, displayHeight - menuVerticalBorderAllowance),
        };
    }

    int MeasureMenuHeight(const wxMenu& menu) const
    {
        auto height = 0;
        for (const auto* item: menu.GetMenuItems())
        {
            height += item->IsSeparator() ? separatorHeight : itemHeight;
        }
        return height;
    }

    int MeasureEntryHeight(const PullRequestMenuEntry& entry) const
    {
        return static_cast<int>(entry.secondaryTitles.size() + (entry.includesTitle ? 1 : 0)) * itemHeight;
    }

    std::size_t GetVisibleEntryCount(const std::span<const PullRequestMenuEntry> entries, const int availableHeight) const
    {
        auto usedHeight = 0;
        std::size_t visibleCount = 0;

        for (const auto& entry: entries)
        {
            const auto entryHeight = MeasureEntryHeight(entry);
            const auto hasOverflow = visibleCount + 1 < entries.size();
            const auto overflowItemHeight = hasOverflow ? itemHeight : 0;

            if (usedHeight + entryHeight + overflowItemHeight > availableHeight)
                break;

            usedHeight += entryHeight;
            ++visibleCount;
        }

        return visibleCount;
    }
};
