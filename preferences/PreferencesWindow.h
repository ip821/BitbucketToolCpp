//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYDIALOG_H
#define WXWIDGETSPLAYGROUND_MYDIALOG_H

#include <wx/event.h>

#include "PreferencesWindowBase.h"

class wxString;

class PreferencesWindow : public PreferencesWindowBase {
public:
    explicit PreferencesWindow();

protected:
    void OnShowWindow(wxShowEvent &event);
    void OnCloseWindow(wxCloseEvent &event);
    void OnSetupClicked(wxCommandEvent &event) override;
    void UpdateTextBoxes();
    void OnInitDialog(wxInitDialogEvent &event) override;
};


#endif //WXWIDGETSPLAYGROUND_MYDIALOG_H
