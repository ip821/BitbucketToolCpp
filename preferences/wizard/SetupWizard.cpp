#include <wx/wx.h>

#include "SetupWizard.h"
#include "LoginPage.h"
#include "RepositoryPage.h"
#include "WorkspacePage.h"

extern "C" void ShowDockIcon();

SetupWizard::SetupWizard(wxWindow* pWindow) :
    wxWizard(pWindow, wxID_ANY, wxT("Setup"))
{
    m_pLoginPage = new LoginPage(this, m_context);
    m_pLoginPage->Chain(new WorkspacePage(this, m_context)).Chain(new RepositoryPage(this, m_context));

    const auto sizer = GetPageAreaSizer();
    sizer->SetMinSize(300, 200);
    sizer->Add(m_pLoginPage);

    Bind(wxEVT_SHOW, [](wxShowEvent&)
    {
        ShowDockIcon();
    });
}

void SetupWizard::Run()
{
    RunWizard(m_pLoginPage);
}
