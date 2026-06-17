#pragma once

#include <wx/event.h>

#include "PreferencesWindowBase.h"

class wxString;

class PreferencesWindow : public PreferencesWindowBase {
public:
    explicit PreferencesWindow();

protected:
    void OnShow(wxShowEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnSetupClicked(wxCommandEvent &event) override;
    void UpdateTextBoxes();
    void OnInitDialog(wxInitDialogEvent &event) override;
};
