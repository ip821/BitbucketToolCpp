#include "PullRequestUpdateCompletedThreadEvent.h"

#include <utility>

const wxEventTypeTag<PullRequestUpdateCompletedThreadEvent>
    PullRequestUpdateCompletedThreadEvent::EventType(wxNewEventType());

PullRequestUpdateCompletedThreadEvent::PullRequestUpdateCompletedThreadEvent(
    std::stop_token stopToken,
    const bool showNotification,
    const std::chrono::seconds elapsedTime,
    GetPullRequestsResult result) :
    wxThreadEvent(EventType),
    m_stopToken(std::move(stopToken)),
    m_showNotification(showNotification),
    m_elapsedTime(elapsedTime),
    m_result(std::move(result))
{
}

wxEvent* PullRequestUpdateCompletedThreadEvent::Clone() const
{
    return new PullRequestUpdateCompletedThreadEvent(*this);
}

bool PullRequestUpdateCompletedThreadEvent::IsCancelled() const
{
    return m_stopToken.stop_requested();
}

bool PullRequestUpdateCompletedThreadEvent::ShouldShowNotification() const
{
    return m_showNotification;
}

std::chrono::seconds PullRequestUpdateCompletedThreadEvent::GetElapsedTime() const
{
    return m_elapsedTime;
}

const GetPullRequestsResult& PullRequestUpdateCompletedThreadEvent::GetResult() const
{
    return m_result;
}
