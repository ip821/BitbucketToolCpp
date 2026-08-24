#include "RepositoryView.h"

#include <stdexcept>
#include <wx/xrc/xmlres.h>

RepositoryView::RepositoryView(wxWindow *parent) :
    wxPanel()
{
    if (!wxXmlResource::Get()->LoadPanel(this, parent, "RepositoryViewBase"))
        throw std::runtime_error("Failed to load RepositoryViewBase from XRC");

    m_pListBox = XRCCTRL(*this, "m_pListBox", wxCheckListBox);

    if (!m_pListBox)
        throw std::runtime_error("RepositoryViewBase XRC is missing a required control");
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
