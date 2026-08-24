#include "LoginView.h"

#include <stdexcept>
#include <wx/activityindicator.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/xrc/xmlres.h>

LoginView::LoginView(wxWindow *parent) :
    wxPanel()
{
    if (!wxXmlResource::Get()->LoadPanel(this, parent, "LoginViewBase"))
        throw std::runtime_error("Failed to load LoginViewBase from XRC");

    m_pLoginTextCtrl = XRCCTRL(*this, "m_pLoginTextCtrl", wxTextCtrl);
    m_pPasswordTextCtrl = XRCCTRL(*this, "m_pPasswordTextCtrl", wxTextCtrl);
    m_pErrorStaticText = XRCCTRL(*this, "m_pErrorStaticText", wxStaticText);
    m_pActivityIndicator = XRCCTRL(*this, "m_pActivityIndicator", wxActivityIndicator);

    if (!m_pLoginTextCtrl || !m_pPasswordTextCtrl || !m_pErrorStaticText || !m_pActivityIndicator)
        throw std::runtime_error("LoginViewBase XRC is missing a required control");

    m_pLoginTextCtrl->SetValidator(wxTextValidator(wxFILTER_NONE, &m_email));
    m_pPasswordTextCtrl->SetValidator(wxTextValidator(wxFILTER_NONE, &m_password));
    m_pErrorStaticText->Hide();
    m_pActivityIndicator->Hide();

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
