#include "RepositoryView.h"

RepositoryView::RepositoryView(wxWindow *parent) :
    RepositoryViewBase(parent)
{
}

void RepositoryView::SetRepositories(const std::vector<Repository>& repositories) const
{
    m_pListBox->Clear();

    for (const auto& ws: repositories)
    {
        m_pListBox->Append(ws.full_name);
    }
}

unsigned int RepositoryView::GetCheckedItems(wxArrayInt& checkedItemIndexes) const
{
    return m_pListBox->GetCheckedItems(checkedItemIndexes);
}
