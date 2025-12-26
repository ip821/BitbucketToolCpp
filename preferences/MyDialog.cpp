//
// Created by Igor Palkin on 19.12.2025.
//

#include "../preferences/MyDialog.h"

#include "wx/wx.h"
#include "wx/log.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"

extern "C" void ShowDockIcon();
extern "C" void HideDockIcon();

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_ABOUT, MyDialog::OnAbout)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
    EVT_SHOW(MyDialog::OnShowWindow)
    EVT_CLOSE(MyDialog::OnCloseWindow)
wxEND_EVENT_TABLE()


MyDialog::MyDialog(const wxString& title)
    : wxDialog(NULL, wxID_ANY, title)
{
    const auto pRootSizer = new wxBoxSizer(wxVERTICAL);
    pRootSizer->SetMinSize(300, 200);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pRootSizer->Add(pMainSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);

    pGridSizer->Add(
        new wxStaticText(this, wxID_ANY, wxT("Workspace:")),
        wxSizerFlags().Expand().Left()
    );

    pGridSizer->Add(
        new wxStaticText(this, wxID_ANY, wxT("mbsolutionsgroup")),
        wxSizerFlags().Right()
    );

    pGridSizer->Add(
        new wxStaticText(this, wxID_ANY, wxT("Repository:")),
        wxSizerFlags().Expand().Left()
    );

    pGridSizer->Add(
        new wxStaticText(this, wxID_ANY, wxT("cr")),
        wxSizerFlags().Right()
    );

    pMainSizer->Add(pGridSizer, wxSizerFlags().Expand().Top().Border(wxALL, 10));

    const auto pHelp = new wxStaticText(
        this, wxID_ANY,
        wxT("Use Setup butoon to enter credentials and to\nchoose Workspace and Repository to follow")
    );
    pHelp->SetFont(pHelp->GetFont().Scale(0.8));
    pHelp->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    pMainSizer->Add(pHelp, wxSizerFlags().Center().Border(wxALL, 10));

    const auto pSetupButton = new wxButton(this, wxID_ANY, wxT("Setup..."), wxDefaultPosition);
    pMainSizer->Add(pSetupButton, wxSizerFlags().Center().Border(wxALL, 10));

    const auto pLaunchAtLoginCheckbox = new wxCheckBox(this, wxID_ANY, wxT("Launch at login"));
    pMainSizer->Add(pLaunchAtLoginCheckbox, wxSizerFlags().Left().Border(wxALL, 10));

    SetSizerAndFit(pRootSizer);
    Centre();
}

MyDialog::~MyDialog()
{
}

void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const char* const title = "About wxWidgets Taskbar Sample";
    static const char* const message
        = "wxWidgets sample showing wxTaskBarIcon class\n"
        "\n"
        "(C) 1997 Julian Smart\n"
        "(C) 2007 Vadim Zeitlin";

    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
    Destroy();
}

void MyDialog::OnShowWindow(wxShowEvent& event)
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

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Show(false);
}
