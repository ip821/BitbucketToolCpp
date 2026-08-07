#include "bitbucket_api/PullRequest.h"

#include <algorithm>

bool PullRequest::IsWaitingForUserApproval(const User& user) const
{
    const auto userParticipant = std::ranges::find_if(
        participants,
        [&user](const auto& it) { return it.user.uuid == user.uuid; });

    return
        author.uuid != user.uuid
        && !draft
        && userParticipant != participants.end()
        && !userParticipant->approved;
}

bool PullRequest::IsUserPullRequest(const User& user) const
{
    return author.uuid == user.uuid && state == PullRequestState::Open;
}
