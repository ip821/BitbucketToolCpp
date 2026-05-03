#include "LoginView.h"

LoginView::LoginView(wxWindow *parent) :
    LoginViewBase(parent)
{
#ifdef __WXMSW__
    m_pActivityIndicator->SetDoubleBuffered(true);
#endif
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
