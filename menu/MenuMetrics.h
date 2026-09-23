#pragma once

#include <wx/display.h>
#include <wx/wx.h>

#include "PullRequestMenuEntry.h"

extern "C" int GetSystemMenuBarHeight();

class MenuMetrics
{
    static constexpr auto MenuItemVerticalPadding = 8;
    static constexpr auto MenuVerticalBorderAllowance = 8;

public:
    int itemHeight{};
    int separatorHeight{};
    int maximumHeight{};

    static MenuMetrics Measure()
    {
        const wxDisplay display;
        const auto displayScaleFactor = display.IsOk() ? display.GetScaleFactor() : 1.0;

        wxBitmap bitmap;
        bitmap.CreateWithLogicalSize(wxSize(1, 1), displayScaleFactor);
        wxMemoryDC dc(bitmap);
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        const auto textHeight = dc.GetTextExtent(wxS("Ag")).GetHeight();
        const auto itemVerticalPadding = dc.FromDIP(MenuItemVerticalPadding);
#if defined __WXOSX__
        const int systemMenuHeight = GetSystemMenuBarHeight() * 1.15;
#else
        const auto systemMenuHeight = wxSystemSettings::GetMetric(wxSYS_MENU_Y);
#endif
        const auto itemHeight = std::max({1, textHeight + itemVerticalPadding, systemMenuHeight});
        const auto separatorHeight = std::max(1, itemHeight / 2);

        const auto displayHeight = display.IsOk()
            ? display.GetClientArea().GetHeight()
            : wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        const auto menuVerticalBorderAllowance = dc.FromDIP(MenuVerticalBorderAllowance);

        return {
            .itemHeight = itemHeight,
            .separatorHeight = separatorHeight,
            .maximumHeight = std::max(itemHeight, displayHeight - menuVerticalBorderAllowance),
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
