//
// Created by Igor Palkin on 19.12.2025.
//

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "StatusItem.h"

#include "smile.xpm"
#include "smile2.xpm"
#include "wx/bmpbndl.h"
#include "wx/menu.h"
#include "wx/artprov.h"

#include "wx/msgdlg.h"
#include "wx/taskbar.h"

extern "C" void InitStatusImage();
extern "C" WXImage* GetStatusImage();

enum
{
    PU_RESTORE = 10001,
    PU_NEW_ICON,
    PU_EXIT,
    PU_CHECKMARK,
    PU_SUB1,
    PU_SUB2,
    PU_SUBMAIN
};


wxBEGIN_EVENT_TABLE(StatusItem, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, StatusItem::OnMenuRestore)
    EVT_MENU(PU_EXIT, StatusItem::OnMenuExit)
    EVT_MENU(PU_NEW_ICON, StatusItem::OnMenuSetNewIcon)
    EVT_MENU(PU_CHECKMARK, StatusItem::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK, StatusItem::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK(StatusItem::OnLeftButtonDClick)
    EVT_MENU(PU_SUB1, StatusItem::OnMenuSub)
    EVT_MENU(PU_SUB2, StatusItem::OnMenuSub)
wxEND_EVENT_TABLE()

StatusItem::StatusItem() : wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM)
{
    m_pDialog = new MyDialog("Preferences");

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    if (!m_bitmapBundle.IsOk())
    {
        wxMessageBox("Could not load status image");
    }
    //if (!SetIcon(wxArtProvider::GetBitmapBundle(wxART_WX_LOGO, wxART_OTHER, wxSize(32, 32))))
    if (!SetIcon(m_bitmapBundle))
    {
        wxLogError("Could not set icon.");
    }
#endif
}

void StatusItem::OnMenuRestore(wxCommandEvent&)
{
    m_pDialog->Show(true);
}

void StatusItem::OnMenuExit(wxCommandEvent&)
{
    m_pDialog->Close(true);
}

static bool check = true;

void StatusItem::OnMenuCheckmark(wxCommandEvent&)
{
    check = !check;
}

void StatusItem::OnMenuUICheckmark(wxUpdateUIEvent& event)
{
    event.Check(check);
}

void StatusItem::OnMenuSetNewIcon(wxCommandEvent&)
{
    wxIcon icon(smile_xpm);

    if (!SetIcon(wxBitmapBundle::FromBitmaps(
                     wxBitmap(smile_xpm),
                     wxBitmap(smile2_xpm)
                 ),
                 "wxTaskBarIcon Sample - a different icon"))
        wxMessageBox("Could not set new icon.");
}

void StatusItem::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox("You clicked on a submenu!");
}

// Overridables
wxMenu* StatusItem::CreatePopupMenu()
{
    wxMenu* menu = new wxMenu;
    menu->Append(PU_RESTORE, "&Restore main window");
    menu->AppendSeparator();
    menu->Append(PU_NEW_ICON, "&Set New Icon");
    menu->AppendSeparator();
    menu->AppendCheckItem(PU_CHECKMARK, "Test &check mark");
    menu->AppendSeparator();
    wxMenu* submenu = new wxMenu;
    submenu->Append(PU_SUB1, "One submenu");
    submenu->AppendSeparator();
    submenu->Append(PU_SUB2, "Another submenu");
    menu->Append(PU_SUBMAIN, "Submenu", submenu);
    /* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__
    if (OSXIsStatusItem())
#endif
    {
        menu->AppendSeparator();
        menu->Append(PU_EXIT, "E&xit");
    }
    return menu;
}

void StatusItem::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    m_pDialog->Show(true);
}
