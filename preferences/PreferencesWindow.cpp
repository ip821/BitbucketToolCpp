#include "../preferences/PreferencesWindow.h"

#include <set>
#include <stdexcept>
#include <cpp_utils/ranges.h>
#include <cpp_utils/wx_string_join.h>
#include <wx/wizard.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "version.h"
#include "../StatusItem.h"
#include "settings/Config.h"
#include "wizard/SetupWizard.h"

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

PreferencesWindow::PreferencesWindow(StatusItem* pStatusItem) :
    wxDialog(),
    m_pStatusItem(pStatusItem)
{
    if (!wxXmlResource::Get()->LoadDialog(this, nullptr, "PreferencesWindowBase"))
        throw std::runtime_error("Failed to load PreferencesWindowBase from XRC");

    m_workspaceText = XRCCTRL(*this, "m_workspaceText", wxStaticText);
    m_repositoryText = XRCCTRL(*this, "m_repositoryText", wxStaticText);
    m_pHelpText = XRCCTRL(*this, "m_pHelpText", wxStaticText);
    m_pCheckBoxHideChangesRequested = XRCCTRL(*this, "m_pCheckBoxHideChangesRequested", wxCheckBox);
    m_pCheckBoxUseSubmenusOnMenuOverflow = XRCCTRL(*this, "m_pCheckBoxUseSubmenusOnMenuOverflow", wxCheckBox);
    m_pCheckBoxDisplayRepositoryNameLowercase =
        XRCCTRL(*this, "m_pCheckBoxDisplayRepositoryNameLowercase", wxCheckBox);
    m_pVersionText = XRCCTRL(*this, "m_pVersionText", wxStaticText);

    if (!m_workspaceText || !m_repositoryText || !m_pHelpText ||
        !m_pCheckBoxHideChangesRequested || !m_pCheckBoxUseSubmenusOnMenuOverflow ||
        !m_pCheckBoxDisplayRepositoryNameLowercase || !m_pVersionText)
    {
        throw std::runtime_error("PreferencesWindowBase XRC is missing a required control");
    }

#if !defined(__WXOSX__)
    const auto statusBitmap = wxXmlResource::Get()->LoadBitmap("status32");
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

    Bind(wxEVT_INIT_DIALOG, &PreferencesWindow::OnInitDialog, this);
    Bind(wxEVT_BUTTON, &PreferencesWindow::OnSetupClicked, this, XRCID("m_pButtonSetup"));
    Bind(wxEVT_CHECKBOX, &PreferencesWindow::OnHideChangesRequestedChanged, this,
         XRCID("m_pCheckBoxHideChangesRequested"));
    Bind(wxEVT_CHECKBOX, &PreferencesWindow::OnUseSubmenusOnMenuOverflowChanged, this,
         XRCID("m_pCheckBoxUseSubmenusOnMenuOverflow"));
    Bind(wxEVT_CHECKBOX, &PreferencesWindow::OnDisplayRepositoryNameLowercaseChanged, this,
         XRCID("m_pCheckBoxDisplayRepositoryNameLowercase"));
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
