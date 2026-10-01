#include "PullRequestsMenuLayout.h"

#include <format>

#include <cpp_utils/wx_string_format.h>
#include <wx/control.h>
#include <wx/dcmemory.h>
#include <wx/display.h>
#include <wx/menu.h>
#include <wx/settings.h>

#include "MenuBuilder.h"
#include "PullRequestsMenuBuilder.h"

namespace
{
    constexpr auto maxMenuWidth = 500;

    wxString FitMenuText(const wxString& text)
    {
        const wxDisplay display;
        const auto displayScaleFactor = display.IsOk() ? display.GetScaleFactor() : 1.0;

        wxBitmap bitmap;
        bitmap.CreateWithLogicalSize(wxSize(1, 1), displayScaleFactor);
        wxMemoryDC dc(bitmap);
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        return wxControl::Ellipsize(text, dc, wxELLIPSIZE_END, dc.FromDIP(maxMenuWidth));
    }
}

PullRequestsMenuLayout::PullRequestsMenuLayout(wxMenu& menu) : m_menu(menu)
{
}

wxMenuItem* PullRequestsMenuLayout::InsertPullRequestTitleMenuItem(MenuBuilder& menuBuilder, const PullRequestInfo& pullRequest) const
{
    return menuBuilder.InsertItem(FitMenuText(pullRequest.GetMainMenuItemTitle()));
}

void PullRequestsMenuLayout::InsertSecondaryPullRequestMenuItem(MenuBuilder& menuBuilder, const wxString& title) const
{
    const auto secondLineTitle = std::format(wxS("   {}"), title);
    menuBuilder.InsertDisabledItem(FitMenuText(secondLineTitle));
}

void PullRequestsMenuLayout::InsertEntry(
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

void PullRequestsMenuLayout::InsertAllEntries(
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
