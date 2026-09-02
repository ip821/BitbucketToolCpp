#include "PullRequestUpdateProgressThreadEvent.h"

#include <utility>

const wxEventTypeTag<PullRequestUpdateProgressThreadEvent>
    PullRequestUpdateProgressThreadEvent::EventType(wxNewEventType());

PullRequestUpdateProgressThreadEvent::PullRequestUpdateProgressThreadEvent(
    std::stop_token stopToken,
    PullRequestUpdateProgressArgs progressArgs) :
    wxThreadEvent(EventType),
    m_stopToken(std::move(stopToken)),
    m_progressArgs(std::move(progressArgs))
{
}

wxEvent* PullRequestUpdateProgressThreadEvent::Clone() const
{
    return new PullRequestUpdateProgressThreadEvent(*this);
}

bool PullRequestUpdateProgressThreadEvent::IsCancelled() const
{
    return m_stopToken.stop_requested();
}

const PullRequestUpdateProgressArgs& PullRequestUpdateProgressThreadEvent::GetProgressArgs() const
{
    return m_progressArgs;
}
