#include "StatusMenu.h"

#include <utility>

#include <wx/control.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/utils.h>

#include "PullRequestsMenuBuilder.h"

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

StatusMenu::StatusMenu(StatusMenuCallbacks callbacks) :
    m_callbacks(std::move(callbacks)),
    m_menu(std::make_unique<wxMenu>())
{
    m_firstStaticMenuItem = m_menu->AppendSeparator();

    m_createPullRequestMenu = new wxMenu();
    m_menu->AppendSubMenu(m_createPullRequestMenu, "&Create pull request");
    m_menu->AppendSeparator();

    m_statisticsMenuItem = m_menu->Append(wxID_ANY, "Statistics");
    m_statisticsMenuItem->Enable(false);
    m_showAllMenuItem = m_menu->Append(wxID_ANY, "&Show hidden items");
    m_updateMenuItem = m_menu->Append(wxID_ANY, "&Update");
    m_preferencesMenuItem = m_menu->Append(wxID_ANY, "&Preferences...");
    m_quitMenuItem = m_menu->Append(wxID_ANY, "&Quit");

    m_menu->Bind(wxEVT_MENU, [this](wxCommandEvent& event)
    {
        OnMenuItemClick(event);
    });
}

StatusMenu::~StatusMenu() = default;

wxMenu* StatusMenu::GetMenu() const
{
    return m_menu.get();
}

void StatusMenu::SetRepositories(const std::vector<Repository>& repositories)
{
    for (const auto menuItems = m_createPullRequestMenu->GetMenuItems();
         const auto& item: menuItems)
    {
        m_createPullRequestMenu->Delete(item);
    }

    m_menuItemIdToRepository.clear();

    for (const auto& repository: repositories)
    {
        const auto repositoryName = wxString::FromUTF8(repository.full_name);
        const auto* menuItem = m_createPullRequestMenu->Append(wxID_ANY, FitMenuText(repositoryName));
        m_menuItemIdToRepository[menuItem->GetId()] = repositoryName;
    }
}

int StatusMenu::RebuildPullRequests(
    const PullRequestsInfo& pullRequests,
    const bool hideChangesRequestedPullRequests,
    const bool useSubmenusOnMenuOverflow,
    const bool displayRepositoryNameLowercase
)
{
    const PullRequestsMenuBuilder menuBuilder(*m_menu);
    auto [menuItemIdToPullRequest, hiddenPullRequestsCount] = menuBuilder.Rebuild(
        *m_firstStaticMenuItem,
        pullRequests,
        hideChangesRequestedPullRequests,
        useSubmenusOnMenuOverflow,
        displayRepositoryNameLowercase
    );

    m_menuItemIdToPullRequest = std::move(menuItemIdToPullRequest);
    return hiddenPullRequestsCount;
}

void StatusMenu::SetUpdateEnabled(const bool enabled)
{
    m_updateMenuItem->Enable(enabled);
}

void StatusMenu::SetStatisticsLabel(const wxString& label)
{
    m_statisticsMenuItem->SetItemLabel(FitMenuText(label));
}

void StatusMenu::OnMenuItemClick(wxCommandEvent& event)
{
    const auto menuItemId = event.GetId();
    const bool isAltPressed = wxGetKeyState(WXK_ALT);

    if (menuItemId == m_updateMenuItem->GetId())
    {
        if (m_callbacks.onUpdateRequested)
            m_callbacks.onUpdateRequested(isAltPressed);
        return;
    }

    if (menuItemId == m_preferencesMenuItem->GetId())
    {
        if (m_callbacks.onPreferencesRequested)
            m_callbacks.onPreferencesRequested();
        return;
    }

    if (menuItemId == m_quitMenuItem->GetId())
    {
        if (m_callbacks.onQuitRequested)
            m_callbacks.onQuitRequested();
        return;
    }

    if (menuItemId == m_showAllMenuItem->GetId())
    {
        if (m_callbacks.onToggleHiddenRequested)
            m_callbacks.onToggleHiddenRequested();
        return;
    }

    if (const auto pullRequestIt = m_menuItemIdToPullRequest.find(menuItemId);
        pullRequestIt != m_menuItemIdToPullRequest.end())
    {
        if (m_callbacks.onOpenPullRequestRequested)
        {
            const auto href = wxString::FromUTF8(pullRequestIt->second.pullRequest.links.html.href);
            m_callbacks.onOpenPullRequestRequested(href, isAltPressed);
        }
        return;
    }

    if (const auto repositoryIt = m_menuItemIdToRepository.find(menuItemId);
        repositoryIt != m_menuItemIdToRepository.end() && m_callbacks.onCreatePullRequestRequested)
    {
        m_callbacks.onCreatePullRequestRequested(repositoryIt->second);
    }
}
