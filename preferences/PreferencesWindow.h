#pragma once

#include <wx/dialog.h>
#include <wx/event.h>

class StatusItem;
class wxCheckBox;
class wxStaticText;

class PreferencesWindow : public wxDialog
{
    StatusItem* const m_pStatusItem;
    wxStaticText* m_workspaceText;
    wxStaticText* m_repositoryText;
    wxStaticText* m_pHelpText;
    wxCheckBox* m_pCheckBoxHideChangesRequested;
    wxCheckBox* m_pCheckBoxUseSubmenusOnMenuOverflow;
    wxCheckBox* m_pCheckBoxDisplayRepositoryNameLowercase;
    wxStaticText* m_pVersionText;

public:
    explicit PreferencesWindow(StatusItem* pStatusItem);

protected:
    void OnShow(wxShowEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnSetupClicked(wxCommandEvent& event);
    void OnHideChangesRequestedChanged(wxCommandEvent& event);
    void OnUseSubmenusOnMenuOverflowChanged(wxCommandEvent& event);
    void OnDisplayRepositoryNameLowercaseChanged(wxCommandEvent& event);
    void UpdateTextBoxes();
    void OnInitDialog(wxInitDialogEvent& event);
};
