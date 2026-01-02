//
// Created by Igor Palkin on 19.12.2025.
//

#include "../preferences/PreferencesWindow.h"

#include "wizard/SetupWizard.h"

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

wxBEGIN_EVENT_TABLE(PreferencesWindow, wxFrame)
    EVT_BUTTON(wxID_ABOUT, PreferencesWindow::OnAbout)
    EVT_BUTTON(wxID_OK, PreferencesWindow::OnOK)
    EVT_BUTTON(wxID_EXIT, PreferencesWindow::OnExit)
    EVT_SHOW(PreferencesWindow::OnShowWindow)
    EVT_CLOSE(PreferencesWindow::OnCloseWindow)
wxEND_EVENT_TABLE()


PreferencesWindow::PreferencesWindow(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title)
{
    wxMenuBar *menuBar = new wxMenuBar();
    SetMenuBar(menuBar);

    const auto pPanel = new wxPanel(this);
    const auto pRootSizer = new wxBoxSizer(wxVERTICAL);

#ifdef __WXGTK__
  pRootSizer->SetMinSize(400, 350);
#else
    pRootSizer->SetMinSize(300, 200);
#endif

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pRootSizer->Add(pMainSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);

    pGridSizer->Add(
        new wxStaticText(pPanel, wxID_ANY, wxT("Workspace:")),
        wxSizerFlags().Expand().Left()
    );

    pGridSizer->Add(
        new wxStaticText(pPanel, wxID_ANY, wxT("mbsolutionsgroup")),
        wxSizerFlags().Right()
    );

    pGridSizer->Add(
        new wxStaticText(pPanel, wxID_ANY, wxT("Repository:")),
        wxSizerFlags().Expand().Left()
    );

    pGridSizer->Add(
        new wxStaticText(pPanel, wxID_ANY, wxT("cr")),
        wxSizerFlags().Right()
    );

    pMainSizer->Add(pGridSizer, wxSizerFlags().Expand().Top().Border(wxALL, 10));

    const auto pHelp = new wxStaticText(
        pPanel, wxID_ANY,
        wxT("Use Setup butoon to enter credentials and to\nchoose Workspace and Repository to follow")
    );
    pHelp->SetFont(pHelp->GetFont().Scale(0.8));
    pHelp->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    pMainSizer->Add(pHelp, wxSizerFlags().Center().Border(wxALL, 10));

    const auto pSetupButton = new wxButton(pPanel, wxID_ANY, wxT("Setup..."), wxDefaultPosition);
    pSetupButton->Bind(wxEVT_BUTTON, &PreferencesWindow::OnSetupClicked, this);
    pMainSizer->Add(pSetupButton, wxSizerFlags().Center().Border(wxALL, 10));

    const auto pLaunchAtLoginCheckbox = new wxCheckBox(pPanel, wxID_ANY, wxT("Launch at login"));
    pMainSizer->Add(pLaunchAtLoginCheckbox, wxSizerFlags().Left().Border(wxALL, 10));

    pPanel->SetSizerAndFit(pRootSizer);
    Fit();
    Centre();
}

void PreferencesWindow::OnSetupClicked(wxCommandEvent& WXUNUSED(event))
{
    SetupWizard setupWizard(this);
    setupWizard.Run();
}

void PreferencesWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const char* const title = "About wxWidgets Taskbar Sample";
    static const char* const message
        = "wxWidgets sample showing wxTaskBarIcon class\n"
        "\n"
        "(C) 1997 Julian Smart\n"
        "(C) 2007 Vadim Zeitlin";

    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
}

void PreferencesWindow::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
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
#endif
}

void PreferencesWindow::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Show(false);
}
