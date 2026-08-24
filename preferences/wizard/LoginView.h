#pragma once

#include <wx/panel.h>
#include <wx/string.h>

class wxActivityIndicator;
class wxStaticText;
class wxTextCtrl;

class LoginView : public wxPanel
{
    wxTextCtrl* m_pLoginTextCtrl;
    wxTextCtrl* m_pPasswordTextCtrl;
    wxStaticText* m_pErrorStaticText;
    wxActivityIndicator* m_pActivityIndicator;

public:
    wxString m_email;
    wxString m_password;

    explicit LoginView(wxWindow *parent);

    void SetErrorMessage(const wxString& message) const;
    void ClearErrorMessage() const;

    void ShowActivityIndicator() const;
    void HideActivityIndicator() const;
};
