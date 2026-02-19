#include "../../stdafx.h"
#include "SetupWizard.h"
#include "LoginPage.h"
#include "RepositoryPage.h"
#include "WorkspacePage.h"

wxBEGIN_EVENT_TABLE(SetupWizard, wxWizard)
    EVT_THREAD(wxID_LOGIN_THREAD_COMPLETED, SetupWizard::OnLoginCompleted)
wxEND_EVENT_TABLE()

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

void SetupWizard::OnLoginCompleted(wxThreadEvent& event)
{
    m_pLoginPage->OnLoginCompleted(event);
}
