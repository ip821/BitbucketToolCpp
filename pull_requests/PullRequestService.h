#pragma once

#include <expected>

#include "PullRequestInfo.h"
#include "bitbucket_api/BitbucketResponse.h"

using GetPullRequestsResult = std::expected<PullRequestsInfo, BitbucketError>;

class PullRequestService
{
public:
    GetPullRequestsResult GetPullRequests();
};
