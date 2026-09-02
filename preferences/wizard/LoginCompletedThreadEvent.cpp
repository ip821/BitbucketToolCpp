#include "LoginCompletedThreadEvent.h"

#include <utility>

const wxEventTypeTag<LoginCompletedThreadEvent>
    LoginCompletedThreadEvent::EventType(wxNewEventType());

LoginCompletedThreadEvent::LoginCompletedThreadEvent(
    std::stop_token stopToken,
    Response response) :
    wxThreadEvent(EventType),
    m_stopToken(std::move(stopToken)),
    m_response(std::move(response))
{
}

wxEvent* LoginCompletedThreadEvent::Clone() const
{
    return new LoginCompletedThreadEvent(*this);
}

bool LoginCompletedThreadEvent::IsCancelled() const
{
    return m_stopToken.stop_requested();
}

const LoginCompletedThreadEvent::Response& LoginCompletedThreadEvent::GetResponse() const
{
    return m_response;
}
