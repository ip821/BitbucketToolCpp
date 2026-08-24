#include "WorkspaceView.h"

#include <stdexcept>
#include <wx/activityindicator.h>
#include <wx/xrc/xmlres.h>

WorkspaceView::WorkspaceView(wxWindow *parent) :
    wxPanel()
{
    if (!wxXmlResource::Get()->LoadPanel(this, parent, "WorkspaceViewBase"))
        throw std::runtime_error("Failed to load WorkspaceViewBase from XRC");

    m_pListBox = XRCCTRL(*this, "m_pListBox", wxCheckListBox);
    m_pActivityIndicator = XRCCTRL(*this, "m_pActivityIndicator", wxActivityIndicator);

    if (!m_pListBox || !m_pActivityIndicator)
        throw std::runtime_error("WorkspaceViewBase XRC is missing a required control");

    m_pActivityIndicator->Hide();

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
