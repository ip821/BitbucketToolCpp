//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYDIALOG_H
#define WXWIDGETSPLAYGROUND_MYDIALOG_H

#include "wx/taskbar.h"
#include "wx/string.h"
#include "wx/dialog.h"

class MyDialog : public wxDialog {
public:
    explicit MyDialog(const wxString &title);

    ~MyDialog() override;

protected:
    void OnAbout(wxCommandEvent &event);
    void OnOK(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnShowWindow(wxShowEvent &event);
    void OnCloseWindow(wxCloseEvent &event);

    wxDECLARE_EVENT_TABLE();
};


#endif //WXWIDGETSPLAYGROUND_MYDIALOG_H
