//
// Created by Igor Palkin on 27.12.2025.
//

#include "../../stdafx.h"

LoginPage::LoginPage(wxWizard* pWindow)
{
    Create(pWindow);
    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);

    const auto pGridSizer = new wxFlexGridSizer(0, 2, 14, 22);
    pGridSizer->AddGrowableCol(1);
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Login")), wxSizerFlags().Left().Expand());
    pGridSizer->Add(new wxTextCtrl(this, wxID_ANY), wxSizerFlags().Right().Expand().Border(wxALL, 10));
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Password")));
    pGridSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD));
    pMainSizer->Add(pGridSizer, wxSizerFlags().Expand());

    SetSizerAndFit(pMainSizer);
}
