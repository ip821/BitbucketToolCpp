#include <ranges>
#include <set>

#include <wx/wizard.h>
#include <wx/wx.h>

#include <cpp_utils/ranges.h>
#include <cpp_utils/wx_string_join.h>

#include "version.h"
#include "settings/Config.h"
#include "wizard/SetupWizard.h"
#include "../preferences/PreferencesWindow.h"

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

PreferencesWindow::PreferencesWindow() :
    PreferencesWindowBase(nullptr)
{
#if !defined(__WXOSX__)
    const auto statusBitmap = wxXmlResource::Get()->LoadBitmap("status32");
    wxIcon icon;
    icon.CopyFromBitmap(statusBitmap);
    SetIcon(icon);
#endif

    m_pHelpText->SetFont(m_pHelpText->GetFont().Scale(0.8));
    m_pVersionText->SetFont(m_pVersionText->GetFont().Scale(0.8));
    m_pVersionText->SetLabelText(std::format("Version: {}", APP_VERSION));

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
