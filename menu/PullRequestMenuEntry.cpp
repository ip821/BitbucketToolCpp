#include "PullRequestMenuEntry.h"

PullRequestMenuEntryFactory::PullRequestMenuEntryFactory(const PullRequestsInfo& pullRequests)
    : m_pullRequests(pullRequests)
{
}

PullRequestMenuEntryResult PullRequestMenuEntryFactory::GetWaitingMyApprovalMenuEntries(const GetWaitingMyApprovalMenuEntriesArgs args) const
{
    int hiddenPullRequestsCount{};
    const auto& currentUser = m_pullRequests.currentUser;
    std::vector<PullRequestMenuEntry> reviewEntries;
    for (const auto& pullRequest: m_pullRequests.waitingForMyApprovalPullRequests)
    {
        const auto participantsRequestedChangesWithoutCurrentUser = pullRequest.GetParticipantsRequestedChangesWithout(currentUser);

        if (args.hideChangesRequestedPullRequests && !participantsRequestedChangesWithoutCurrentUser.empty())
        {
            ++hiddenPullRequestsCount;
            continue;
        }

        PullRequestMenuEntry entry{
            .pullRequest = &pullRequest,
            .secondaryTitles = {
                pullRequest.GetAuthorAndBranchMenuItemTitle(args.displayRepositoryNameLowercase),
                pullRequest.GetPullRequestDetailsMenuItemTitle(),
            },
        };

        for (const auto& participant: pullRequest.GetParticipantsRequestedChanges())
        {
            entry.secondaryTitles.push_back(pullRequest.GetParticipantMenuItemTitle(participant));
        }

        reviewEntries.push_back(std::move(entry));
    }

    return {.hiddenPullRequestsCount = hiddenPullRequestsCount, .entries = std::move(reviewEntries)};
}

PullRequestMenuEntryResult PullRequestMenuEntryFactory::GetMyMenuEntries(const bool displayRepositoryNameLowercase) const
{
    std::vector<PullRequestMenuEntry> myPullRequestEntries;

    for (const auto& pullRequest: m_pullRequests.myPullRequests)
    {
        PullRequestMenuEntry entry{
            .pullRequest = &pullRequest,
            .secondaryTitles = {
                pullRequest.GetMyPullRequestBranchMenuItemTitle(displayRepositoryNameLowercase),
                pullRequest.GetPullRequestDetailsMenuItemTitle(),
            },
        };

        for (const auto& participant: pullRequest.pullRequest.participants)
        {
            if (participant.role == ParticipantRole::Reviewer || participant.approved)
            {
                entry.secondaryTitles.push_back(pullRequest.GetParticipantMenuItemTitle(participant));
            }
        }

        myPullRequestEntries.push_back(std::move(entry));
    }

    return {.hiddenPullRequestsCount = 0, .entries = std::move(myPullRequestEntries)};
}
