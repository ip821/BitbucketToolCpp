#include <wx/wx.h>
#include <wx/wizard.h>

#include "RepositoryPage.h"
#include "RepositoryView.h"
#include "SetupWizard.h"
#include "../settings/Config.h"

RepositoryPage::RepositoryPage(SetupWizard* pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_context(context)
{
    m_pRepositoryView = new RepositoryView(this);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(m_pRepositoryView, wxSizerFlags().Expand().Proportion(1));
    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this](wxWizardEvent&)
    {
        m_pRepositoryView->SetRepositories(m_context.m_repositories);
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [this](wxWizardEvent& event)
    {
        if (!event.GetDirection())
            return;

        wxArrayInt checkedItemIndexes;
        if (const auto checkedCount = m_pRepositoryView->GetCheckedItems(checkedItemIndexes);
            !checkedCount)
        {
            event.Veto();
        }
        else
        {
            std::vector<Repository> repositories;
            for (const auto& checkedIndex : checkedItemIndexes)
            {
                const auto& repository = m_context.m_repositories.at(checkedIndex);
                repositories.push_back(repository);
            }
            Config::SetRepositories(repositories);

            const auto reps = Config::GetRepositories();
            wxUnusedVar(reps);
        }
    });
}
