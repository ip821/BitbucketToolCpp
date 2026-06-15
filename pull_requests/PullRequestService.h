#pragma once
#include <expected>

#include "PullRequestInfo.h"
#include "../http/HttpConnection.h"

using GetPullRequestsResult = std::expected<PullRequestsInfo, Error>;

class PullRequestService
{
public:
    GetPullRequestsResult GetPullRequests();
};
