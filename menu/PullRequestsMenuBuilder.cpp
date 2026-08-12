#include "PullRequestsMenuBuilder.h"

#include <ranges>

#include <cpp_utils/wx_string_format.h>
#include <wx/control.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/settings.h>

#include "MenuBuilder.h"

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

PullRequestsMenuBuildResult PullRequestsMenuBuilder::Rebuild(
    const int firstDynamicMenuItemId,
    const PullRequestsInfo& pullRequests,
    const bool hideChangesRequestedPullRequests,
    const bool useTwoColumnLayout
) const
{
    RemoveDynamicMenuItems(firstDynamicMenuItemId);

    PullRequestsMenuBuildResult result;

    MenuBuilder menuBuilder(m_menu, firstDynamicMenuItemId);
    const auto pFirstMenuItem = menuBuilder.InsertDisabledItem("Pull requests to review");

    const auto& currentUser = pullRequests.currentUser;

    for (const auto& pullRequest: pullRequests.waitingForMyApprovalPullRequests)
    {
        const auto participantsRequestedChangesWithoutCurrentUser = pullRequest.GetParticipantsRequestedChangesWithout(currentUser);

        if (hideChangesRequestedPullRequests && !participantsRequestedChangesWithoutCurrentUser.empty())
        {
            ++result.hiddenPullRequestsCount;
            continue;
        }

        const auto pMenuItem = InsertPullRequestTitleMenuItem(menuBuilder, pullRequest);
        result.menuItemIdToPullRequest[pMenuItem->GetId()] = pullRequest;

        InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetAuthorAndBranchMenuItemTitle());
        InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetPullRequestDetailsMenuItemTitle());

        for (const auto& participant: pullRequest.GetParticipantsRequestedChanges())
        {
            InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetParticipantMenuItemTitle(participant));
        }
    }

    if (result.hiddenPullRequestsCount)
    {
        const auto firstMenuItemTitle = std::format(wxS("{} [{} hidden]"), pFirstMenuItem->GetItemLabel(), result.hiddenPullRequestsCount);
        pFirstMenuItem->SetItemLabel(firstMenuItemTitle);
    }

    if (useTwoColumnLayout)
    {
        menuBuilder.StartNewColumnAtEnd();
    }
    else
    {
        menuBuilder.InsertSeparator();
    }

    menuBuilder.InsertDisabledItem("Your pull requests");

    for (const auto& pullRequest: pullRequests.myPullRequests)
    {
        const auto pMenuItem = InsertPullRequestTitleMenuItem(menuBuilder, pullRequest);
        result.menuItemIdToPullRequest[pMenuItem->GetId()] = pullRequest;

        InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetMyPullRequestBranchMenuItemTitle());
        InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetPullRequestDetailsMenuItemTitle());

        const auto participants = pullRequest.pullRequest.participants
                                  | std::views::filter([](const auto& it) { return it.role == ParticipantRole::Reviewer || it.approved; })
                                  | std::ranges::to<std::vector>();

        for (const auto& participant: participants)
        {
            InsertSecondaryPullRequestMenuItem(menuBuilder, pullRequest.GetParticipantMenuItemTitle(participant));
        }
    }

    return result;
}

void PullRequestsMenuBuilder::RemoveDynamicMenuItems(const int firstDynamicMenuItemId) const
{
    for (const auto menuItems = m_menu.GetMenuItems();
         const auto& item: menuItems)
    {
        if (item->GetId() >= firstDynamicMenuItemId)
        {
            m_menu.Delete(item);
        }
    }
}
