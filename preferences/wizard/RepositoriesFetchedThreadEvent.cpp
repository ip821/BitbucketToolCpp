#include "RepositoriesFetchedThreadEvent.h"

#include <utility>

const wxEventTypeTag<RepositoriesFetchedThreadEvent>
    RepositoriesFetchedThreadEvent::EventType(wxNewEventType());

RepositoriesFetchedThreadEvent::RepositoriesFetchedThreadEvent(
    std::stop_token stopToken,
    std::vector<Repository> repositories) :
    wxThreadEvent(EventType),
    m_stopToken(std::move(stopToken)),
    m_repositories(std::move(repositories))
{
}

wxEvent* RepositoriesFetchedThreadEvent::Clone() const
{
    return new RepositoriesFetchedThreadEvent(*this);
}

bool RepositoriesFetchedThreadEvent::IsCancelled() const
{
    return m_stopToken.stop_requested();
}

const std::vector<Repository>& RepositoriesFetchedThreadEvent::GetRepositories() const
{
    return m_repositories;
}
