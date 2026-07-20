#pragma once

#include <algorithm>
#include <vector>

#include "PullRequestInfo.h"

struct PullRequestsInfo
{
    User currentUser{};
    std::vector<PullRequestInfo> skippedPullRequests{};
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

    const PullRequestInfo *GetPullRequestOrNull(int id) const
    {
        const auto findById = [id](const PullRequestInfo& info)
        {
            return info.pullRequest.id == id;
        };

        if (const auto it = std::ranges::find_if(waitingForMyApprovalPullRequests, findById);
            it != waitingForMyApprovalPullRequests.end())
            return &*it;

        if (const auto it = std::ranges::find_if(myPullRequests, findById);
            it != myPullRequests.end())
            return &*it;

        if (const auto it = std::ranges::find_if(skippedPullRequests, findById);
            it != skippedPullRequests.end())
            return &*it;

        return nullptr;
    }
};
