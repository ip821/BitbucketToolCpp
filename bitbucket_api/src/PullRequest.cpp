#include "bitbucket_api/PullRequest.h"

#include <ranges>
#include <nlohmann/json.hpp>

std::optional<ParticipantUser> PullRequest::GetParticipantForUser(const User& user) const
{
    const auto filtered = participants
                          | std::views::filter([&user](const auto& it) { return it.user.uuid == user.uuid; })
                          | std::ranges::to<std::vector>();

    if (filtered.empty())
        return std::nullopt;

    return *filtered.cbegin();
}

bool PullRequest::IsWaitingForUserApproval(const User& user) const
{
    const auto& userParticipant = GetParticipantForUser(user);
    return
        author.uuid != user.uuid
        && !draft
        && userParticipant.has_value()
        && !userParticipant.value().approved;
}

bool PullRequest::IsUserPullRequest(const User& user) const
{
    return author.uuid == user.uuid && state == PullRequestState::Open;
}
