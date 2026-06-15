#pragma once
#include "BitbucketClient.h"
#include "PullRequest.h"
#include "../Constants.h"

class PullRequestRequest : public BitbucketClient<PullRequest>
{
public:
    TResponse GetPullRequest(const Repository& repository, int pullRequestId) const
    {
        const auto repoKey = repository.GetRepoKey();

        const auto url = BitBucketBaseUrl
                + wxS("/repositories/")
                + repoKey
                + wxS("/pullrequests/")
                + std::format(wxS("{}"), pullRequestId);

        return PerformRequest(url);
    }
};
