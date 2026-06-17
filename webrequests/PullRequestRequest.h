#pragma once

#include "BitbucketRequest.h"
#include "PullRequest.h"

class PullRequestRequest : public BitbucketRequest<PullRequest>
{
public:
    TResponse GetPullRequest(const Repository& repository, int pullRequestId) const
    {
        return PerformRequest(BitbucketUrlBuilder::GetPullRequestsUrl(repository, pullRequestId));
    }
};
