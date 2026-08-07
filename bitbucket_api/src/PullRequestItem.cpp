#include "bitbucket_api/PullRequest.h"

#include <algorithm>

bool PullRequestItem::IsWaitingForUserApproval(const User& user) const
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

bool PullRequestItem::IsUserPullRequest(const User& user) const
{
    return author.uuid == user.uuid && state == PullRequestState::Open;
}
