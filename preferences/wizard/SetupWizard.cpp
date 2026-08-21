#include "SetupWizard.h"

#include <wx/wx.h>

#include "LoginPage.h"
#include "RepositoryPage.h"
#include "WorkspacePage.h"

extern "C" void ShowDockIcon();

SetupWizard::SetupWizard(wxWindow* pWindow) :
    wxWizard(pWindow, wxID_ANY, wxT("Setup"))
{
    m_pLoginPage = new LoginPage(this, m_context);
    m_pWorkspacePage = new WorkspacePage(this, m_context);
    m_pLoginPage->Chain(m_pWorkspacePage).Chain(new RepositoryPage(this, m_context));

    const auto sizer = GetPageAreaSizer();
    sizer->SetMinSize(300, 200);
    sizer->Add(m_pLoginPage);

    Bind(wxEVT_SHOW, [](wxShowEvent&)
    {
#ifdef __WXOSX__
        ShowDockIcon();
#endif
    });
}

SetupWizard::~SetupWizard()
{
    m_pLoginPage->StopAsyncOperation();
    m_pWorkspacePage->StopAsyncOperation();
}

void SetupWizard::Run()
{
    RunWizard(m_pLoginPage);
}
