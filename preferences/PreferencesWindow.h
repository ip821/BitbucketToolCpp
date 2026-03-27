//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYDIALOG_H
#define WXWIDGETSPLAYGROUND_MYDIALOG_H

#include <wx/frame.h>
#include <wx/event.h>

class wxString;

class PreferencesWindow : public wxDialog {
public:
    explicit PreferencesWindow(const wxString &title);

protected:
    void OnShowWindow(wxShowEvent &event);
    void OnCloseWindow(wxCloseEvent &event);
    void OnSetupClicked(wxCommandEvent &event);

    wxDECLARE_EVENT_TABLE();
};


#endif //WXWIDGETSPLAYGROUND_MYDIALOG_H
