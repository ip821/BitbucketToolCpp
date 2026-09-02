#pragma once

#include <chrono>
#include <stop_token>
#include <wx/event.h>

#include "PullRequestService.h"

class PullRequestUpdateCompletedThreadEvent final : public wxThreadEvent
{
public:
    static const wxEventTypeTag<PullRequestUpdateCompletedThreadEvent> EventType;

    PullRequestUpdateCompletedThreadEvent(
        std::stop_token stopToken,
        bool showNotification,
        std::chrono::seconds elapsedTime,
        GetPullRequestsResult result);

    wxEvent* Clone() const override;

    [[nodiscard]] bool IsCancelled() const;
    [[nodiscard]] bool ShouldShowNotification() const;
    [[nodiscard]] std::chrono::seconds GetElapsedTime() const;
    [[nodiscard]] const GetPullRequestsResult& GetResult() const;

private:
    std::stop_token m_stopToken;
    bool m_showNotification;
    std::chrono::seconds m_elapsedTime;
    GetPullRequestsResult m_result;
};
