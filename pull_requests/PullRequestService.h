#pragma once

#include <expected>
#include <functional>
#include <stop_token>
#include <string>
#include <variant>
#include <vector>

#include "PullRequestInfo.h"
#include "PullRequestsInfo.h"
#include "../preferences/Credentials.h"
#include "bitbucket_api/BitbucketResponse.h"
#include "bitbucket_api/Requests.h"

using GetPullRequestsResult = std::expected<PullRequestsInfo, BitbucketError>;

struct FetchingRepositoryPullRequests
{
    std::string repositoryName{};
};

struct FetchingPullRequestDetails
{
    std::string repositoryName{};
    size_t currentPullRequest{};
    size_t totalPullRequests{};
};

using PullRequestUpdateProgressArgs = std::variant<FetchingRepositoryPullRequests, FetchingPullRequestDetails>;

using PullRequestUpdateProgressCallback = std::function<void(const PullRequestUpdateProgressArgs&)>;

class PullRequestService
{
public:
    explicit PullRequestService(
        const PullRequestUpdateProgressCallback& progress_callback,
        const std::stop_token& stop_token
    ) :
        m_progress_callback(progress_callback),
        m_stop_token(stop_token)
    {

    }

    GetPullRequestsResult GetPullRequests(const std::vector<Repository>& repositories);

private:
    const PullRequestUpdateProgressCallback& m_progress_callback;
    const std::stop_token& m_stop_token;

    [[nodiscard]] bool Cancelled() const;

    struct PullRequestToProcess
    {
        std::reference_wrapper<const Repository> repository;
        PullRequest pullRequest{};
        bool isWaitingForUserApproval{};
    };

    struct FetchPullRequestsResult
    {
        size_t fetchedPullRequestsCount{};
        std::vector<PullRequestToProcess> pullRequestsToProcess{};
    };
    using FetchPullRequestsResultExpected = std::expected<FetchPullRequestsResult, BitbucketError>;
    FetchPullRequestsResultExpected FetchPullRequests(
        const std::string& credentials,
        const std::vector<Repository>& repositories,
        const User& currentUser);

    struct GetPullRequestDetailsResult
    {
        std::vector<PullRequestInfo> waitingForMyApprovalPullRequests{};
        std::vector<PullRequestInfo> myPullRequests{};
    };
    using GetPullRequestDetailsResultExpected = std::expected<GetPullRequestDetailsResult, BitbucketError>;
    GetPullRequestDetailsResultExpected GetPullRequestDetails(
        const std::string& credentials,
        std::vector<PullRequestToProcess>&& pullRequestsToProcess
        );
};
