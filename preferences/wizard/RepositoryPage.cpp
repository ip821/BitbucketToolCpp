#include <wx/wx.h>
#include <wx/wizard.h>

#include "RepositoryPage.h"
#include "SetupWizard.h"

RepositoryPage::RepositoryPage(SetupWizard* pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    //m_wizard(*pWizard),
    m_context(context),
    m_staticBox(*new wxStaticBox(this, wxID_ANY, wxT("")))
{
    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    const auto pStaticBoxSizer = new wxStaticBoxSizer(&m_staticBox, wxVERTICAL);

    const auto pListBox = new wxCheckListBox(this, wxID_ANY);

    pStaticBoxSizer->Add(pListBox, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));
    pMainSizer->Add(pStaticBoxSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));

    SetSizerAndFit(pMainSizer);
    Fit();

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this, pListBox](wxWizardEvent&)
    {
        pListBox->Clear();

        for (const auto& ws : m_context.m_repositories)
        {
            pListBox->Append(ws.m_name);
        }
    });
}
