#include "PullRequestService.h"

#include <cpp_utils/macros_expected.h>

#include "PullRequestsInfo.h"
#include "../bitbucket_api/include/bitbucket_api/Requests.h"
#include "../bitbucket_api/include/bitbucket_api/Structs.h"
#include "../preferences/Credentials.h"
#include "../preferences/settings/Config.h"

GetPullRequestsResult PullRequestService::GetPullRequests()
{
    const auto credentials = Credentials::GetCredentialsBase64().ToStdString();

    constexpr CurrentUserRequest currentUserRequest;
    const auto currentUserResult = currentUserRequest.GetCurrentUser(credentials);
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserResult);

    PullRequestsInfo result{.currentUser = currentUser};

    constexpr PullRequestsRequest pullRequestsRequest;
    constexpr PullRequestRequest pullRequestRequest;
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;
    size_t fetchedPullRequestsCount = 0;
    size_t processedPullRequestsCount = 0;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(credentials, repository, currentUser.uuid);
        UNWRAP_OR_RETURN_ERROR(pullRequestItems, pullRequestsResult);

        fetchedPullRequestsCount += pullRequestItems.values.size();

        for (const auto uniquePullRequests = pullRequestItems.DistinctById();
             const auto& pullRequestItem: uniquePullRequests)
        {
            const auto isWaitingForUserApproval = pullRequestItem.IsWaitingForUserApproval(currentUser);
            const auto isUserPullRequest = pullRequestItem.IsUserPullRequest(currentUser);
            if (!isWaitingForUserApproval && !isUserPullRequest)
                continue;

            processedPullRequestsCount++;
            const auto pullRequestResult = pullRequestRequest.GetPullRequest(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(pullRequest, pullRequestResult);

            const auto diffStatResult = diffStatRequest.GetDiffStat(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(diffStat, diffStatResult);

            PullRequestInfo pullRequestInfo{.pullRequest = pullRequest, .statuses = {}, .diffStat = diffStat};

            const auto statusesResult = statusRequest.GetStatuses(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(statuses, statusesResult);
            pullRequestInfo.statuses = statuses.values;

            if (isWaitingForUserApproval)
                result.waitingForMyApprovalPullRequests.push_back(pullRequestInfo);
            else if (isUserPullRequest)
                result.myPullRequests.push_back(pullRequestInfo);
        }
    }

    result.fetchedPullRequestsCount = fetchedPullRequestsCount;
    result.processedPullRequestsCount = processedPullRequestsCount;
    result.Sort();

    return result;
}
