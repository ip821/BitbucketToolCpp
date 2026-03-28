//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYTASKBARICON_H
#define WXWIDGETSPLAYGROUND_MYTASKBARICON_H

#include <wx/taskbar.h>
#include <wx/bmpbndl.h>
#include <wx/timer.h>

#include "webrequests/Repository.h"

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

    std::vector<Repository> m_repositories;

    wxMenu* GetPopupMenu() override;

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuPreferences(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuUpdate(wxCommandEvent&);
    void OnMenuCreatePr(wxCommandEvent&);

    void RemoveAllPrMenuItems();
    void ShowPreferencesDialog() const;
    void UpdateCreatePullRequestsMenu(const std::vector<Repository>& repositories);
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
