#pragma once

#include <wx/display.h>
#include <wx/wx.h>

#include "PullRequestMenuEntry.h"

extern "C" int GetSystemMenuBarHeight();

class MenuMetrics
{
    static constexpr auto MenuVerticalBorderAllowance = 8;

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
#if defined __WXOSX__
        const auto systemMenuHeight = GetSystemMenuBarHeight();
#else
        const auto systemMenuHeight = wxSystemSettings::GetMetric(wxSYS_MENU_Y);
#endif
        const auto itemHeight = std::max({1, textHeight + 8, systemMenuHeight});
        const auto separatorHeight = std::max(1, itemHeight / 2);

        const wxDisplay display;
        const auto displayHeight = display.IsOk()
            ? display.GetClientArea().GetHeight()
            : wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

        return {
            .itemHeight = itemHeight,
            .separatorHeight = separatorHeight,
            .maximumHeight = std::max(itemHeight, displayHeight - MenuVerticalBorderAllowance),
        };
    }

    [[nodiscard]] int MeasureMenuHeight(const wxMenu& menu) const
    {
        auto height = 0;
        for (const auto* item: menu.GetMenuItems())
        {
            height += item->IsSeparator() ? separatorHeight : itemHeight;
        }
        return height;
    }

    [[nodiscard]] int MeasureEntryHeight(const PullRequestMenuEntry& entry) const
    {
        return static_cast<int>(entry.secondaryTitles.size() + (entry.includesTitle ? 1 : 0)) * itemHeight;
    }

    [[nodiscard]] std::size_t GetVisibleEntryCount(const std::span<const PullRequestMenuEntry> entries, const int availableHeight) const
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
