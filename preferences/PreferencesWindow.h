//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYDIALOG_H
#define WXWIDGETSPLAYGROUND_MYDIALOG_H

#include "../stdafx.h"

class PreferencesWindow : public wxFrame {
public:
    explicit PreferencesWindow(const wxString &title);

protected:
    void OnAbout(wxCommandEvent &event);
    void OnOK(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnShowWindow(wxShowEvent &event);
    void OnCloseWindow(wxCloseEvent &event);
    void OnSetupClicked(wxCommandEvent &event);

    wxDECLARE_EVENT_TABLE();
};


#endif //WXWIDGETSPLAYGROUND_MYDIALOG_H
