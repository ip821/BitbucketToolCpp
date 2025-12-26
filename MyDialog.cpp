//
// Created by Igor Palkin on 19.12.2025.
//

#include "MyDialog.h"

#include "wx/taskbar.h"
#include "wx/artprov.h"
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
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    sizerTop->Add(new wxStaticText(
                      this,
                      wxID_ANY,
                      "Press 'Hide me' to hide this window, Exit to quit."
                  ), flags);

    sizerTop->Add(new wxStaticText(
                      this,
                      wxID_ANY,
                      "Double-click on the taskbar icon to show me again."
                  ), flags);

    sizerTop->AddStretchSpacer()->SetMinSize(200, 50);

    wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_ABOUT, "&About"), flags);
    sizerBtns->Add(new wxButton(this, wxID_OK, "&Hide"), flags);
    sizerBtns->Add(new wxButton(this, wxID_EXIT, "E&xit"), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
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

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
    m_taskBarIcon->ShowBalloon(title, message, 15000,
                               wxICON_INFORMATION,
                               wxBitmapBundle::FromSVGFile("info.svg", wxSize(64, 64))
    );
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
#endif // __WXMSW__/!__WXMSW__
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
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
    Destroy();
}
