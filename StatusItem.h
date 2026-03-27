//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYTASKBARICON_H
#define WXWIDGETSPLAYGROUND_MYTASKBARICON_H

#include <wx/taskbar.h>
#include <wx/bmpbndl.h>
#include <wx/timer.h>

struct OnUpdatePullRequestsArgs
{
    bool showNotification;
};

class PreferencesWindow;

class StatusItem : public wxTaskBarIcon
{
    PreferencesWindow* m_pDialog;
    wxMenu* m_pMenu;
    wxMenu* m_pCreatePullRequestsMenu;
    wxTimer* m_pTimer;

    wxMenu* GetPopupMenu() override;
    void ShowPreferencesDialog() const;

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuPreferences(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuUpdate(wxCommandEvent&);
    void OnMenuCreatePr(wxCommandEvent&);
    void RemoveAllPrMenuItems();

    void UpdateCreatePullRequestsMenu(const wxArrayString& repositories);
    void OnUpdatePullRequests(const OnUpdatePullRequestsArgs& args);

#ifdef __WXOSX__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif

#ifndef __WXOSX__
    wxBitmap m_statusBitmap;
    wxBitmapBundle m_bitmapBundle;
#endif

public:
    explicit StatusItem();

    wxDECLARE_EVENT_TABLE();
};

#endif // WXWIDGETSPLAYGROUND_MYTASKBARICON_H
