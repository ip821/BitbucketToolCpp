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

enum
{
    MENU_ITEM_PREFERENCES_ID = 10001,
    MENU_ITEM_QUIT_ID,
    MENU_ITEM_UPDATE_ID,
    MENU_ITEM_CREATE_PULL_REQUEST_ID,
};


wxBEGIN_EVENT_TABLE(StatusItem, wxTaskBarIcon)
    EVT_MENU(MENU_ITEM_PREFERENCES_ID, StatusItem::OnMenuPreferences)
    EVT_MENU(MENU_ITEM_QUIT_ID, StatusItem::OnMenuExit)
    EVT_MENU(MENU_ITEM_UPDATE_ID, StatusItem::OnMenuUpdate)
    EVT_MENU(MENU_ITEM_CREATE_PULL_REQUEST_ID, StatusItem::OnMenuCreatePr)
    EVT_TASKBAR_LEFT_DCLICK(StatusItem::OnLeftButtonDClick)
wxEND_EVENT_TABLE()

StatusItem::StatusItem() : wxTaskBarIcon(wxTBI_CUSTOM_STATUSITEM)
{
    m_pDialog = new MyDialog("Preferences");

#ifdef WXDEBUG
    m_pDialog->Show(true);
#endif

#if defined(__WXOSX__)
    SetIcon("status32@2x");
    SetTitle("1/2");
#else
    if (!m_bitmapBundle.IsOk())
    {
        wxMessageBox("Could not load status image");
    }
    SetIcon(m_bitmapBundle);
#endif
}

void StatusItem::ShowPreferencesDialog()
{
    if (!m_pDialog->IsVisible())
        m_pDialog->Show(true);

    m_pDialog->Raise();
}

void StatusItem::OnMenuPreferences(wxCommandEvent&)
{
    ShowPreferencesDialog();
}

void StatusItem::OnMenuExit(wxCommandEvent&)
{
    m_pDialog->Close(true);
    m_pDialog->Destroy();
}

void StatusItem::OnMenuUpdate(wxCommandEvent&)
{
}

void StatusItem::OnMenuCreatePr(wxCommandEvent&)
{
}

wxMenu* StatusItem::CreatePopupMenu()
{
    wxMenu* menu = new wxMenu;
    menu->AppendSeparator();
    menu->Append(MENU_ITEM_CREATE_PULL_REQUEST_ID, "&Create pull request...");
    menu->AppendSeparator();
    menu->Append(MENU_ITEM_UPDATE_ID, "&Update");
    menu->Append(MENU_ITEM_PREFERENCES_ID, "&Preferences...");
    menu->Append(MENU_ITEM_QUIT_ID, "&Quit");
    return menu;
}

void StatusItem::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    ShowPreferencesDialog();
}
