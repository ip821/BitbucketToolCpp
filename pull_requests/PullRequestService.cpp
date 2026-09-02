#include "PullRequestService.h"

#include <cpp_utils/macros_expected.h>
#include <cpp_utils/ranges_distinct_by.h>

#include "PullRequestsInfo.h"
#include "../bitbucket_api/include/bitbucket_api/Requests.h"
#include "../bitbucket_api/include/bitbucket_api/Structs.h"
#include "../preferences/Credentials.h"

namespace
{
    const auto cancellationError = [] { return std::unexpected(BitbucketError{.message = "Operation cancelled"}); };
}

bool PullRequestService::Cancelled() const
{
    return m_stop_token.stop_requested();
};

GetPullRequestsResult PullRequestService::GetPullRequests(const std::vector<Repository>& repositories)
{
    if (Cancelled())
        return cancellationError();

    const auto credentials = Credentials::GetCredentialsBase64().ToStdString();

    constexpr CurrentUserRequest currentUserRequest;
    UNWRAP_OR_RETURN_ERROR(currentUser, currentUserRequest.GetCurrentUser(credentials));

    if (Cancelled())
        return cancellationError();

    UNWRAP_OR_RETURN_ERROR(fetchResult, FetchPullRequests(credentials, repositories, currentUser));
    const auto fetchedPullRequestsCount = fetchResult.fetchedPullRequestsCount;
    const auto processedPullRequestsCount = fetchResult.pullRequestsToProcess.size();

    UNWRAP_OR_RETURN_ERROR(
        detailsResult,
        GetPullRequestDetails(credentials, std::move(fetchResult.pullRequestsToProcess)));

    auto result = PullRequestsInfo{
        .fetchedPullRequestsCount = fetchedPullRequestsCount,
        .processedPullRequestsCount = processedPullRequestsCount,
        .currentUser = std::move(currentUser),
        .waitingForMyApprovalPullRequests = std::move(detailsResult.waitingForMyApprovalPullRequests),
        .myPullRequests = std::move(detailsResult.myPullRequests),
    };

    result.Sort();
    return result;
}

PullRequestService::FetchPullRequestsResultExpected PullRequestService::FetchPullRequests(
    const std::string& credentials,
    const std::vector<Repository>& repositories,
    const User& currentUser)
{
    size_t fetchedPullRequestsCount = 0;
    std::vector<PullRequestToProcess> pullRequestsToProcess{};

    constexpr PullRequestsRequest pullRequestsRequest;

    for (const auto& repository: repositories)
    {
        if (Cancelled())
            return cancellationError();

        m_progress_callback(FetchingRepositoryPullRequests{
            .repositoryName = repository.full_name,
        });

        UNWRAP_OR_RETURN_ERROR(
            pullRequests,
            pullRequestsRequest.GetPullRequests(credentials, repository, currentUser.uuid));

        if (Cancelled())
            return cancellationError();

        fetchedPullRequestsCount += pullRequests.values.size();
        pullRequestsToProcess.reserve(pullRequestsToProcess.size() + pullRequests.values.size());

        for (const auto uniquePullRequests = pullRequests.values | ip::ranges::distinct_by(&PullRequest::id);
             const auto& pullRequest: uniquePullRequests)
        {
            const auto isWaitingForUserApproval = pullRequest.IsWaitingForUserApproval(currentUser);
            const auto isUserPullRequest = pullRequest.IsUserPullRequest(currentUser);
            if (!isWaitingForUserApproval && !isUserPullRequest)
                continue;

            pullRequestsToProcess.push_back({
                .repository = std::cref(repository),
                .pullRequest = std::move(pullRequest),
                .isWaitingForUserApproval = isWaitingForUserApproval,
            });
        }
    }

    return FetchPullRequestsResult{
        .fetchedPullRequestsCount = fetchedPullRequestsCount,
        .pullRequestsToProcess = std::move(pullRequestsToProcess)
    };
}

PullRequestService::GetPullRequestDetailsResultExpected PullRequestService::GetPullRequestDetails(
    const std::string& credentials,
    std::vector<PullRequestToProcess>&& pullRequestsToProcess)
{
    constexpr DiffStatRequest diffStatRequest;
    constexpr StatusRequest statusRequest;

    std::vector<PullRequestInfo> waitingForMyApprovalPullRequests{};
    std::vector<PullRequestInfo> myPullRequests{};

    const auto pull_requests_to_process_count = pullRequestsToProcess.size();
    for (size_t pullRequestIndex = 0; pullRequestIndex < pull_requests_to_process_count; ++pullRequestIndex)
    {
        if (Cancelled())
            return cancellationError();

        auto& [repositoryReference, pullRequest, isWaitingForUserApproval] = pullRequestsToProcess[pullRequestIndex];
        const auto& repository = repositoryReference.get();
        m_progress_callback(FetchingPullRequestDetails{
            .repositoryName = repository.full_name,
            .currentPullRequest = pullRequestIndex + 1,
            .totalPullRequests = pull_requests_to_process_count,
        });

        UNWRAP_OR_RETURN_ERROR(
            diffStat,
            diffStatRequest.GetDiffStat(credentials, repository, pullRequest.id));

        if (Cancelled())
            return cancellationError();

        UNWRAP_OR_RETURN_ERROR(
            statuses,
            statusRequest.GetStatuses(credentials, repository, pullRequest.id));

        if (Cancelled())
            return cancellationError();

        PullRequestInfo pullRequestInfo{
            .pullRequest = std::move(pullRequest),
            .statuses = std::move(statuses.values),
            .diffStat = diffStat
        };

        if (isWaitingForUserApproval)
            waitingForMyApprovalPullRequests.push_back(std::move(pullRequestInfo));
        else
            myPullRequests.push_back(std::move(pullRequestInfo));
    }

    return GetPullRequestDetailsResult{
        .waitingForMyApprovalPullRequests = std::move(waitingForMyApprovalPullRequests),
        .myPullRequests = std::move(myPullRequests),
    };
}
