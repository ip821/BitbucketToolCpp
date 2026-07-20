#include "PullRequestService.h"

#include <cpp_utils/macros_expected.h>

#include "PullRequestsInfo.h"
#include "../bitbucket_api/include/bitbucket_api/Requests.h"
#include "../bitbucket_api/include/bitbucket_api/Structs.h"
#include "../preferences/Credentials.h"
#include "../preferences/settings/Config.h"

GetPullRequestsResult PullRequestService::GetPullRequests(const PullRequestsInfo& previousPullRequests)
{
    const auto credentials = Credentials::GetCredentialsBase64().ToStdString();

    constexpr CurrentUserRequest currentUserRequest;
    const auto currentUserResult = currentUserRequest.GetCurrentUser(credentials);
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserResult);

    PullRequestsInfo result{};

    constexpr PullRequestsRequest pullRequestsRequest;
    constexpr PullRequestRequest pullRequestRequest;
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(credentials, repository, currentUser.uuid);
        UNWRAP_OR_RETURN_ERROR(pullRequestItems, pullRequestsResult);

        for (const auto uniquePullRequests = pullRequestItems.DistinctById();
             const auto& pullRequestItem: uniquePullRequests)
        {
            PullRequestInfo pullRequestInfo;

            if (const auto pPreviousPullRequest = previousPullRequests.GetSkippedPullRequestOrNull(pullRequestItem.id);
                pPreviousPullRequest.has_value()
                && pPreviousPullRequest.value()->pullRequest.updated_on >= pullRequestItem.updated_on)
            {
                pullRequestInfo = *pPreviousPullRequest.value();
            }
            else
            {
                const auto pullRequestResult = pullRequestRequest.GetPullRequest(credentials, repository, pullRequestItem.id);
                UNWRAP_OR_RETURN_ERROR(pullRequest, pullRequestResult);

                const auto diffStatResult = diffStatRequest.GetDiffStat(credentials, repository, pullRequestItem.id);
                UNWRAP_OR_RETURN_ERROR(diffStat, diffStatResult);

                pullRequestInfo = {.pullRequest = pullRequest, .statuses = {}, .diffStat = diffStat};
            }

            if (
                !pullRequestInfo.pullRequest.IsWaitingForUserApproval(currentUser)
                && !pullRequestInfo.pullRequest.IsUserPullRequest(currentUser))
            {
                result.skippedPullRequests.push_back(pullRequestInfo);
                continue;
            }

            const auto statusesResult = statusRequest.GetStatuses(credentials, repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(statuses, statusesResult);
            pullRequestInfo.statuses = statuses.values;

            if (pullRequestInfo.pullRequest.IsWaitingForUserApproval(currentUser))
                result.waitingForMyApprovalPullRequests.push_back(pullRequestInfo);
            else if (pullRequestInfo.pullRequest.IsUserPullRequest(currentUser))
                result.myPullRequests.push_back(pullRequestInfo);
        }
    }

    result.Sort();

    return result;
}
