#pragma once

#include <wx/event.h>

#include "PreferencesWindowBase.h"

class StatusItem;
class wxString;

class PreferencesWindow : public PreferencesWindowBase
{
    StatusItem* const m_pStatusItem;

public:
    explicit PreferencesWindow(StatusItem* pStatusItem);

protected:
    void OnShow(wxShowEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnSetupClicked(wxCommandEvent& event) override;
    void OnHideChangesRequestedChanged(wxCommandEvent& event) override;
    void OnUseTwoColumnLayoutChanged(wxCommandEvent& event) override;
    void UpdateTextBoxes();
    void OnInitDialog(wxInitDialogEvent& event) override;
};
