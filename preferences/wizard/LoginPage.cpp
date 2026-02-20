//
// Created by Igor Palkin on 27.12.2025.
//

#include "../../stdafx.h"
#include "LoginPage.h"
#include <wx/activityindicator.h>
#include <wx/wizard.h>
#include <wx/thread.h>
#include <wx/weakref.h>

#include "LoginThread.h"

LoginPage::LoginPage(wxWizard* pWindow) :
    wxWizardPageSimple(pWindow),
    m_staticBox(*new wxStaticBox(this, wxID_ANY, wxT(""))),
    m_activityIndicator(*CreateActivityIndicator(&m_staticBox))
{
    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);

    const auto pStaticBox = &m_staticBox;
    m_activityIndicator.Hide();

    const auto pStaticBoxSizer = new wxStaticBoxSizer(pStaticBox, wxVERTICAL);

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);

    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Login")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(new wxTextCtrl(pStaticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_THEME),
                    wxSizerFlags().Left().Expand());
    pGridSizer->Add(new wxStaticText(pStaticBox, wxID_ANY, wxT("Password")),
                    wxSizerFlags().Left().CenterVertical());
    pGridSizer->Add(new wxTextCtrl(pStaticBox, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD),
                    wxSizerFlags().Left().Expand());

    pStaticBoxSizer->Add(pGridSizer, wxSizerFlags().Expand().Border(wxALL, 15));
    pStaticBoxSizer->Add(&m_activityIndicator, wxSizerFlags().Center().Border(wxBOTTOM, 15));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this](wxWizardEvent& event)
    {
        if (!event.GetDirection()
            || m_loginCompleted
            || m_pLoginThread != nullptr
        )
            return;

        event.Veto();
        m_activityIndicator.Show();
        m_activityIndicator.Start();
        Layout();

        m_pLoginThread = new LoginThread(GetParent());
        m_pLoginThread->Run();
    });
}

wxActivityIndicator* LoginPage::CreateActivityIndicator(wxStaticBox* pStaticBox)
{
    const auto pLoader = new wxActivityIndicator(pStaticBox);
#ifdef __WXMSW__
    pLoader->SetDoubleBuffered(true);
#endif
    return pLoader;
}

void LoginPage::OnLoginCompleted(wxThreadEvent& event)
{
    m_activityIndicator.Stop();
    m_activityIndicator.Hide();
    Layout();
    m_loginCompleted = true;
    m_pLoginThread = nullptr;
}
