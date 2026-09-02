#pragma once

#include <stop_token>
#include <wx/event.h>

#include "PullRequestService.h"

class PullRequestUpdateProgressThreadEvent final : public wxThreadEvent
{
public:
    static const wxEventTypeTag<PullRequestUpdateProgressThreadEvent> EventType;

    PullRequestUpdateProgressThreadEvent(
        std::stop_token stopToken,
        PullRequestUpdateProgressArgs progressArgs);

    wxEvent* Clone() const override;

    [[nodiscard]] bool IsCancelled() const;
    [[nodiscard]] const PullRequestUpdateProgressArgs& GetProgressArgs() const;

private:
    std::stop_token m_stopToken;
    PullRequestUpdateProgressArgs m_progressArgs;
};
