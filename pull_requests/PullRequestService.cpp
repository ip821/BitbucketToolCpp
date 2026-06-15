#include "PullRequestService.h"

#include <cpp_utils/macros_expected.h>

#include "../preferences/settings/Config.h"
#include "../webrequests/CurrentUserRequest.h"
#include "../webrequests/PullRequestRequest.h"
#include "../webrequests/PullRequestsRequest.h"

GetPullRequestsResult PullRequestService::GetPullRequests()
{
    constexpr CurrentUserRequest currentUserRequest;
    const auto currentUserResult = currentUserRequest.GetCurrentUser();
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserResult);

    std::vector<PullRequestInfo> pullRequestInfos;

    constexpr PullRequestsRequest pullRequestsRequest;
    constexpr PullRequestRequest pullRequestRequest;
    for (const auto& repository: Config::GetRepositories())
    {
        const auto pullRequestsResult = pullRequestsRequest.GetPullRequests(repository, currentUser.uuid);
        UNWRAP_OR_RETURN_ERROR(pullRequestItems, pullRequestsResult);

        for (const auto& pullRequestItem: pullRequestItems.values)
        {
            const auto pullRequestResult = pullRequestRequest.GetPullRequest(repository, pullRequestItem.id);
            UNWRAP_OR_RETURN_ERROR(pullRequest, pullRequestResult);

            pullRequestInfos.push_back({pullRequest});
        }
    }

    return PullRequestsInfo{currentUser, pullRequestInfos};
}
