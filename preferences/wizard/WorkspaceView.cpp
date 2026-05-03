#include "WorkspaceView.h"

WorkspaceView::WorkspaceView(wxWindow *parent) :
    WorkspaceViewBase(parent)
{
#ifdef __WXMSW__
    m_pActivityIndicator->SetDoubleBuffered(true);
#endif
}

void WorkspaceView::SetWorkspaces(const std::vector<Workspace>& workspaces) const
{
    m_pListBox->Clear();
    for (const auto& ws: workspaces)
    {
        m_pListBox->Append(ws.slug);
    }
}

unsigned int WorkspaceView::GetCheckedItems(wxArrayInt& checkedItemsIndexes) const
{
    return m_pListBox->GetCheckedItems(checkedItemsIndexes);
}

void WorkspaceView::ShowActivityIndicator() const
{
    m_pActivityIndicator->Show();
    m_pActivityIndicator->Start();
}

void WorkspaceView::HideActivityIndicator() const
{
    m_pActivityIndicator->Stop();
    m_pActivityIndicator->Hide();
}
