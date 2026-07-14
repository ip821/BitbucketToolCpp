#pragma once

#include <expected>

#include "PullRequestInfo.h"
#include "../webrequests/BitbucketRequest.h"

using GetPullRequestsResult = std::expected<PullRequestsInfo, BitbucketError>;

class PullRequestService
{
public:
    GetPullRequestsResult GetPullRequests();
};
