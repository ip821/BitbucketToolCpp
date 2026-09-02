#pragma once

#include <stop_token>
#include <vector>
#include <wx/event.h>

#include "bitbucket_api/Structs.h"

class RepositoriesFetchedThreadEvent final : public wxThreadEvent
{
public:
    static const wxEventTypeTag<RepositoriesFetchedThreadEvent> EventType;

    RepositoriesFetchedThreadEvent(
        std::stop_token stopToken,
        std::vector<Repository> repositories);

    wxEvent* Clone() const override;

    [[nodiscard]] bool IsCancelled() const;
    [[nodiscard]] const std::vector<Repository>& GetRepositories() const;

private:
    std::stop_token m_stopToken;
    std::vector<Repository> m_repositories;
};
