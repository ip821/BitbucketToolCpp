#include <unordered_set>
#include <cpp_utils/macros_expected.h>

#include "PullRequestService.h"

#include <algorithm>

#include "../preferences/settings/Config.h"
#include "../bitbucket_api/include/bitbucket_api/Requests.h"
#include "../bitbucket_api/include/bitbucket_api/Structs.h"
#include "../preferences/Credentials.h"

GetPullRequestsResult PullRequestService::GetPullRequests()
{
    const auto credentials = Credentials::GetCredentialsBase64().ToStdString();

    constexpr CurrentUserRequest currentUserRequest;
    const auto currentUserResult = currentUserRequest.GetCurrentUser(credentials);
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserResult);

    std::vector<PullRequestInfo> waitingForMyApprovalPullRequests;
    std::vector<PullRequestInfo> myPullRequests;

    constexpr PullRequestsRequest pullRequestsRequest;
    constexpr PullRequestRequest pullRequestRequest;
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(credentials, repository, currentUser.uuid);
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
            const auto pullRequestResult = pullRequestRequest.GetPullRequest(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(pullRequest, pullRequestResult);

            if (
                !pullRequest.IsWaitingForUserApproval(currentUser)
                && !pullRequest.IsUserPullRequest(currentUser))
            {
                continue;
            }

            const auto statusesResult = statusRequest.GetStatuses(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(statuses, statusesResult);

            const auto diffStatResult = diffStatRequest.GetDiffStat(credentials, repository, pullRequestItem.id);
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
