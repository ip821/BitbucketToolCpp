#pragma once

#include "WizardPages.h"

class LoginView : public LoginViewBase
{
private:
    void OnManageApiTokensClicked(wxCommandEvent& event) override;

public:
    explicit LoginView(wxWindow *parent);

    void SetErrorMessage(const wxString& message) const;
    void ClearErrorMessage() const;

    void ShowActivityIndicator() const;
    void HideActivityIndicator() const;
};
