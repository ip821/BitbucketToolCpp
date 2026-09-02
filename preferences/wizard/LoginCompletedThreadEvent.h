#pragma once

#include <stop_token>
#include <vector>
#include <wx/event.h>

#include "bitbucket_api/BitbucketResponse.h"
#include "bitbucket_api/Structs.h"

class LoginCompletedThreadEvent final : public wxThreadEvent
{
public:
    using Response = BitbucketResponse<std::vector<Workspace>>;

    static const wxEventTypeTag<LoginCompletedThreadEvent> EventType;

    LoginCompletedThreadEvent(std::stop_token stopToken, Response response);

    wxEvent* Clone() const override;

    [[nodiscard]] bool IsCancelled() const;
    [[nodiscard]] const Response& GetResponse() const;

private:
    std::stop_token m_stopToken;
    Response m_response;
};
