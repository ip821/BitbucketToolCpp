#include "../preferences/PreferencesWindow.h"

#include <set>
#include <cpp_utils/wx_string_join.h>
#include <wx/wizard.h>
#include <wx/wx.h>

#include "version.h"
#include "../StatusItem.h"
#include "settings/Config.h"
#include "wizard/SetupWizard.h"

#if !defined(__WXOSX__) && !defined(__WXMSW__)
#include "../images/StatusImage.h"
#endif

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

PreferencesWindow::PreferencesWindow(StatusItem* pStatusItem) :
    PreferencesWindowBase(nullptr),
    m_pStatusItem(pStatusItem)
{
#if defined(__WXMSW__)
    SetIcon(wxICON(app_icon));
#elif !defined(__WXOSX__)
    const auto statusBitmap = LoadEmbeddedStatusBitmap();
    wxIcon icon;
    icon.CopyFromBitmap(statusBitmap);
    SetIcon(icon);
#endif

    m_pHelpText->SetFont(m_pHelpText->GetFont().Scale(0.8));
    m_pVersionText->SetFont(m_pVersionText->GetFont().Scale(0.8));
    m_pVersionText->SetLabelText(std::format("Version: {}", APP_VERSION));

    m_pCheckBoxHideChangesRequested->SetValue(Config::GetHideChangesRequestedPullRequests());
    m_pCheckBoxUseSubmenusOnMenuOverflow->SetValue(Config::GetUseSubmenusOnMenuOverflow());
    m_pCheckBoxDisplayRepositoryNameLowercase->SetValue(Config::GetDisplayRepositoryNameLowercase());

    Bind(wxEVT_SHOW, &PreferencesWindow::OnShow, this);
    Bind(wxEVT_CLOSE_WINDOW, &PreferencesWindow::OnClose, this);
}

void PreferencesWindow::OnInitDialog(wxInitDialogEvent&)
{
    UpdateTextBoxes();
}

void PreferencesWindow::OnSetupClicked(wxCommandEvent& WXUNUSED(event))
{
    SetupWizard setupWizard(this);
    setupWizard.Run();
    UpdateTextBoxes();
}

void PreferencesWindow::OnHideChangesRequestedChanged(wxCommandEvent& event)
{
    Config::SetHideChangesRequestedPullRequests(event.IsChecked());
    m_pStatusItem->ConfigChanged();
}

void PreferencesWindow::OnUseSubmenusOnMenuOverflowChanged(wxCommandEvent& event)
{
    Config::SetUseSubmenusOnMenuOverflow(event.IsChecked());
    m_pStatusItem->ConfigChanged();
}

void PreferencesWindow::OnDisplayRepositoryNameLowercaseChanged(wxCommandEvent& event)
{
    Config::SetDisplayRepositoryNameLowercase(event.IsChecked());
    m_pStatusItem->ConfigChanged();
}

void PreferencesWindow::UpdateTextBoxes()
{
    const auto repositories = Config::GetRepositories();

    std::set<wxString> workspaceNames;
    std::set<wxString> repositoryNames;

    for (const auto& repository: repositories)
    {
        workspaceNames.insert(repository.workspace.slug);
        repositoryNames.insert(repository.slug);
    }

    m_workspaceText->SetLabelText(ip::wxJoin(workspaceNames, wxS(", ")));
    m_repositoryText->SetLabelText(ip::wxJoin(repositoryNames, wxS(", ")));
    Layout();
}

void PreferencesWindow::OnShow(wxShowEvent& event)
{
#if defined(__WXOSX__)
    if (event.IsShown())
    {
        ShowDockIcon();
    } else
    {
        HideDockIcon();
    }
#else
    wxUnusedVar(event);
#endif
}

void PreferencesWindow::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Show(false);
}
