#include <unordered_set>
#include <cpp_utils/macros_expected.h>

#include "PullRequestService.h"

#include "../preferences/settings/Config.h"
#include "../webrequests/CurrentUserRequest.h"
#include "../webrequests/DiffStatRequest.h"
#include "../webrequests/PullRequestRequest.h"
#include "../webrequests/PullRequestsRequest.h"
#include "../webrequests/StatusRequest.h"

GetPullRequestsResult PullRequestService::GetPullRequests()
{
    constexpr CurrentUserRequest currentUserRequest;
    const auto currentUserResult = currentUserRequest.GetCurrentUser();
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserResult);

    std::vector<PullRequestInfo> waitingForMyApprovalPullRequests;
    std::vector<PullRequestInfo> myPullRequests;

    constexpr PullRequestsRequest pullRequestsRequest;
    constexpr PullRequestRequest pullRequestRequest;
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(repository, currentUser.uuid);
        UNWRAP_OR_RETURN_ERROR(pullRequestItems, pullRequestsResult);

        std::vector<PullRequestItem> uniquePullRequests;
        std::unordered_set<decltype(PullRequestItem::id)> processedIds;

        for (const auto& pullRequestItem: pullRequestItems.values)
        {
            if (processedIds.insert(pullRequestItem.id).second)
                uniquePullRequests.push_back(pullRequestItem);
        }

        for (const auto& pullRequestItem: uniquePullRequests)
        {
            const auto pullRequestResult = pullRequestRequest.GetPullRequest(repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(pullRequest, pullRequestResult);

            if (
                !pullRequest.IsWaitingForUserApproval(currentUser)
                && !pullRequest.IsUserPullRequest(currentUser))
            {
                continue;
            }

            const auto statusesResult = statusRequest.GetStatuses(repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(statuses, statusesResult);

            const auto diffStatResult = diffStatRequest.GetDiffStat(repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(diffStat, diffStatResult);

            if (pullRequest.IsWaitingForUserApproval(currentUser))
                waitingForMyApprovalPullRequests.push_back({pullRequest, statuses.values, diffStat});
            else if (pullRequest.IsUserPullRequest(currentUser))
                myPullRequests.push_back({pullRequest, statuses.values, diffStat});
        }
    }

    std::ranges::sort(waitingForMyApprovalPullRequests,
                      [](const PullRequestInfo& a, const PullRequestInfo& b)
                      {
                          return a.pullRequest.created_on < b.pullRequest.created_on;
                      });

    std::ranges::sort(myPullRequests,
                      [](const PullRequestInfo& a, const PullRequestInfo& b)
                      {
                          return a.pullRequest.created_on < b.pullRequest.created_on;
                      });

    return PullRequestsInfo{currentUser, waitingForMyApprovalPullRequests, myPullRequests};
}
