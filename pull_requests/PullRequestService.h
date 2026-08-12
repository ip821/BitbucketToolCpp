#pragma once

#include <expected>
#include <functional>

#include "PullRequestInfo.h"
#include "PullRequestsInfo.h"
#include "bitbucket_api/BitbucketResponse.h"

using GetPullRequestsResult = std::expected<PullRequestsInfo, BitbucketError>;

struct PullRequestUpdateProgress
{
    bool isFetchingDetails{};
    size_t completed{};
    size_t total{};
};

using PullRequestUpdateProgressCallback = std::function<void(const PullRequestUpdateProgress&)>;

class PullRequestService
{
public:
    GetPullRequestsResult GetPullRequests(const PullRequestUpdateProgressCallback& progressCallback = {});
};
