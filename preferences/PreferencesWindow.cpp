//
// Created by Igor Palkin on 19.12.2025.
//

#include <wx/wizard.h>
#include <wx/wx.h>

#include "../preferences/PreferencesWindow.h"

#include "wizard/SetupWizard.h"

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

wxBEGIN_EVENT_TABLE(PreferencesWindow, wxFrame)
    EVT_BUTTON(wxID_EXIT, PreferencesWindow::OnExit)
    EVT_SHOW(PreferencesWindow::OnShowWindow)
    EVT_CLOSE(PreferencesWindow::OnCloseWindow)
wxEND_EVENT_TABLE()

PreferencesWindow::PreferencesWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    const auto menuBar = new wxMenuBar();
    wxFrameBase::SetMenuBar(menuBar);

    const auto pWorkspaceStaticText = new wxStaticText(this, wxID_ANY, wxT("mbsolutionsgroup"));

    const auto pRepositoryStaticText = new wxStaticText(this, wxID_ANY, wxT("cr"));

    const auto pHelp = new wxStaticText(
        this,
        wxID_ANY,
        wxT("Use Setup button to enter credentials and to\nchoose Workspace and Repository to follow")
    );
    pHelp->SetFont(pHelp->GetFont().Scale(0.8));
    pHelp->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));

    const auto pSetupButton = new wxButton(this, wxID_ANY, wxT("Setup..."), wxDefaultPosition);
    pSetupButton->Bind(wxEVT_BUTTON, &PreferencesWindow::OnSetupClicked, this);

    const auto pLaunchAtLoginCheckbox = new wxCheckBox(this, wxID_ANY, wxT("Launch at login"));

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Workspace:")), wxSizerFlags().Expand().Left());
    pGridSizer->Add(pWorkspaceStaticText, wxSizerFlags().Right());
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Repository:")), wxSizerFlags().Expand().Left());
    pGridSizer->Add(pRepositoryStaticText, wxSizerFlags().Right());

    const auto pSizer = new wxBoxSizer(wxVERTICAL);
    pSizer->Add(pHelp, wxSizerFlags().Center().Border(wxALL, 10));
    pSizer->Add(pSetupButton, wxSizerFlags().Center().Border(wxALL, 10));
    pSizer->Add(pLaunchAtLoginCheckbox, wxSizerFlags().Left().Border(wxALL, 10));

    const auto pRootSizer = new wxBoxSizer(wxVERTICAL);
    pRootSizer->Add(pGridSizer, wxSizerFlags().Expand().Top().Border(wxALL, 10));
    pRootSizer->Add(pSizer, wxSizerFlags().Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));

    SetSizerAndFit(pRootSizer);
    Centre();
}

void PreferencesWindow::OnSetupClicked(wxCommandEvent& WXUNUSED(event))
{
    SetupWizard setupWizard(this);
    setupWizard.Run();
}

void PreferencesWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
    Destroy();
}

void PreferencesWindow::OnShowWindow(wxShowEvent& event)
{
#if defined(__WXOSX__)
    if (event.IsShown())
    {
        ShowDockIcon();
    }
    else
    {
        HideDockIcon();
    }
#else
    wxUnusedVar(event);
#endif
}

void PreferencesWindow::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Show(false);
}
