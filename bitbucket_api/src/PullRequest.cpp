#include "bitbucket_api/PullRequest.h"

#include <algorithm>
#include <ranges>

#include "cpp_utils/views_any_of.h"
#include "cpp_utils/views_first_or_none.h"

bool PullRequest::IsWaitingForUserApproval(const User& user) const
{
    const auto userParticipant = participants
        | std::views::filter([&user](const auto& it) { return it.user.uuid == user.uuid; })
        | ip::views::first_or_none;

    return
        author.uuid != user.uuid
        && !draft
        && userParticipant
        && !userParticipant->get().approved;
}

bool PullRequest::IsUserPullRequest(const User& user) const
{
    return author.uuid == user.uuid && state == PullRequestState::Open;
}
