#include "../../menu/PullRequestsMenuLayout.h"

#include <wx/menu.h>

#include "../../menu/MenuBuilder.h"
#include "../../menu/PullRequestsMenuBuilder.h"

wxMenuItem* PullRequestsMenuLayout::BuildSections(
    std::vector<PullRequestMenuEntry> waitingForApprovalEntries,
    std::vector<PullRequestMenuEntry> myEntries,
    [[maybe_unused]] const bool useSubmenusOnMenuOverflow,
    PullRequestsMenuBuildResult& result
) const
{
    const auto insertSection = [this, &result](
        const std::size_t index,
        const wxString& label,
        const std::vector<PullRequestMenuEntry>& entries)
    {
        if (entries.empty())
        {
            auto* item = m_menu.Insert(index, wxID_ANY, label);
            item->Enable(false);
            return item;
        }

        auto* item = m_menu.Insert(index, wxID_ANY, label, new wxMenu());
        MenuBuilder submenuBuilder(*item->GetSubMenu());
        InsertAllEntries(submenuBuilder, entries, result);
        return item;
    };

    auto* firstMenuItem = insertSection(0, "Pull requests to review", waitingForApprovalEntries);
    insertSection(1, "Your pull requests", myEntries);
    return firstMenuItem;
}
