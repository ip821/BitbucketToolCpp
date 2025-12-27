#include "../../stdafx.h"

SetupWizard::SetupWizard(wxWindow* pWindow)
{
    Create(pWindow);

    m_pLoginPage = new LoginPage(this);
    m_pLoginPage->Chain(new WorkspacePage(this)).Chain(new RepositoryPage(this));

    const auto sizer = GetPageAreaSizer();
    sizer->SetMinSize(300, 200);
    sizer->Add(m_pLoginPage);
}

void SetupWizard::Run()
{
    RunWizard(m_pLoginPage);
}
