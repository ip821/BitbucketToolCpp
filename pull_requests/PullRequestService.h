#pragma once

#include <expected>
#include <functional>
#include <stop_token>
#include <string>
#include <variant>
#include <vector>

#include "PullRequestInfo.h"
#include "PullRequestsInfo.h"
#include "bitbucket_api/BitbucketResponse.h"

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
    GetPullRequestsResult GetPullRequests(
        const std::vector<Repository>& repositories,
        const PullRequestUpdateProgressCallback& progressCallback = {},
        std::stop_token stopToken = {});
};
