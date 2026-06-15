#pragma once
#include "../webrequests/PullRequest.h"

struct PullRequestInfo
{
    const PullRequest pullRequest;
    // let statuses: Values<Status>
    // let diffStat: DiffStat
};

struct PullRequestsInfo
{
    const User currentUser;
    const std::vector<PullRequestInfo> pullRequests;
};
