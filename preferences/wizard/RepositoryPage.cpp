#include <wx/wx.h>
#include <wx/wizard.h>

#include "RepositoryPage.h"
#include "SetupWizard.h"
#include "../settings/Config.h"

RepositoryPage::RepositoryPage(SetupWizard* pWizard, SetupWizardContext& context) :
    wxWizardPageSimple(pWizard),
    m_context(context)
{
    const auto pListBox = new wxCheckListBox(this, wxID_ANY);

    const auto pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pListBox, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));

    SetSizerAndFit(pMainSizer);

    Bind(wxEVT_WIZARD_PAGE_SHOWN, [this, pListBox](wxWizardEvent&)
    {
        pListBox->Clear();

        for (const auto& ws : m_context.m_repositories)
        {
            pListBox->Append(ws.full_name);
        }
    });

    Bind(wxEVT_WIZARD_PAGE_CHANGING, [pListBox, this](wxWizardEvent& event)
    {
        if (!event.GetDirection())
            return;

        wxArrayInt checkedItemIndexes;
        if (const auto checkedCount = pListBox->GetCheckedItems(checkedItemIndexes);
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
