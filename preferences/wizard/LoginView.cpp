#include "LoginView.h"

#include <wx/utils.h>

namespace
{
constexpr auto ManageApiTokensUrl = "https://id.atlassian.com/manage-profile/security/api-tokens";
}

LoginView::LoginView(wxWindow *parent) :
    LoginViewBase(parent)
{
#ifdef __WXMSW__
    m_pActivityIndicator->SetDoubleBuffered(true);
#endif
}

void LoginView::OnManageApiTokensClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLaunchDefaultBrowser(ManageApiTokensUrl);
}

void LoginView::SetErrorMessage(const wxString& message) const
{
    m_pErrorStaticText->SetLabelText(message);
    m_pErrorStaticText->Show();
}

void LoginView::ClearErrorMessage() const
{
    m_pErrorStaticText->SetLabelText("");
    m_pErrorStaticText->Hide();
}

void LoginView::ShowActivityIndicator() const
{
    m_pActivityIndicator->Show();
    m_pActivityIndicator->Start();
}

void LoginView::HideActivityIndicator() const
{
    m_pActivityIndicator->Stop();
    m_pActivityIndicator->Hide();
}
