#pragma once

#include <algorithm>
#include <vector>

#include "PullRequestInfo.h"

struct PullRequestsInfo
{
    size_t fetchedPullRequestsCount{};
    size_t processedPullRequestsCount{};
    User currentUser{};
    std::vector<PullRequestInfo> waitingForMyApprovalPullRequests{};
    std::vector<PullRequestInfo> myPullRequests{};

    void Sort()
    {
        std::ranges::sort(waitingForMyApprovalPullRequests,
                          [](const PullRequestInfo& a, const PullRequestInfo& b)
                          {
                              return a.pullRequest.created_on < b.pullRequest.created_on;
                          });

        std::ranges::sort(myPullRequests,
                          [](const PullRequestInfo& a, const PullRequestInfo& b)
                          {
                              return a.pullRequest.created_on < b.pullRequest.created_on;
                          });
    }
};
