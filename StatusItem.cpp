#include "StatusItem.h"
#include "preferences/PreferencesWindow.h"

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
    m_pDialog = new PreferencesWindow("Preferences");

#ifdef WXDEBUG
    m_pDialog->Show(true);
#endif

#if defined(__WXOSX__)
    SetIcon("status32@2x");
    SetTitle("1/2");
#else
    m_statusBitmap = wxXmlResource::Get()->LoadBitmap("status32");
    if (!m_statusBitmap.IsOk())
    {
      wxMessageBox("Bitmap was loaded incorrectly");
    }
    m_bitmapBundle = wxBitmapBundle::FromBitmap(m_statusBitmap);
    if (!m_bitmapBundle.IsOk())
    {
      wxMessageBox("Could not load status image");
    }
    if (!IsAvailable())
    {
      wxMessageBox("System icon is not available");
    }
    SetIcon(m_bitmapBundle, "Tooltip");
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
