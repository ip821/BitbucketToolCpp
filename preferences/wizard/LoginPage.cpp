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

LoginPage::LoginPage(wxWizard* pWindow)
{
    Create(pWindow);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);

    const auto pStaticBox = new wxStaticBox(this, wxID_ANY, wxT(""));
    const auto pLoader = new wxActivityIndicator(pStaticBox);
    pLoader->Hide();
    m_pLoader = pLoader;

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
    pStaticBoxSizer->Add(pLoader, wxSizerFlags().Center().Border(wxBOTTOM, 15));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this, pLoader](wxWizardEvent& event)
    {
        if (!event.GetDirection()
            || m_loginCompleted
            || m_pLoginThread != nullptr
        )
            return;

        event.Veto();
        pLoader->Show();
        pLoader->Start();
        Layout();

        m_pLoginThread = new LoginThread(GetParent());
        m_pLoginThread->Run();
    });
}

void LoginPage::OnLoginCompleted(wxThreadEvent& event)
{
    m_pLoader->Stop();
    m_pLoader->Hide();
    Layout();
    m_loginCompleted = true;
    m_pLoginThread = nullptr;
}
