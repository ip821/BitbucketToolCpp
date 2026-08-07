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
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;
    size_t fetchedPullRequestsCount = 0;
    size_t processedPullRequestsCount = 0;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(credentials, repository, currentUser.uuid);
        UNWRAP_OR_RETURN_ERROR(pullRequests, pullRequestsResult);

        fetchedPullRequestsCount += pullRequests.values.size();

        for (const auto uniquePullRequests = pullRequests.DistinctById();
             const auto& pullRequest: uniquePullRequests)
        {
            const auto isWaitingForUserApproval = pullRequest.IsWaitingForUserApproval(currentUser);
            const auto isUserPullRequest = pullRequest.IsUserPullRequest(currentUser);
            if (!isWaitingForUserApproval && !isUserPullRequest)
                continue;

            processedPullRequestsCount++;
            const auto diffStatResult = diffStatRequest.GetDiffStat(credentials, repository, pullRequest.id);
            UNWRAP_OR_RETURN_ERROR(diffStat, diffStatResult);

            PullRequestInfo pullRequestInfo{.pullRequest = pullRequest, .statuses = {}, .diffStat = diffStat};

            const auto statusesResult = statusRequest.GetStatuses(credentials, repository, pullRequest.id);
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
