#pragma once
#include "BitbucketClient.h"
#include "DiffStat.h"
#include "Repository.h"
#include "../Constants.h"

class DiffStatRequest : public BitbucketClient<DiffStat>
{
public:
    TResponse GetDiffStat(const Repository& repository, int pullRequestId) const
    {
        const auto repoKey = repository.GetRepoKey();

        const auto url = BitBucketBaseUrl
                + wxS("/repositories/")
                + repoKey
                + wxS("/pullrequests/")
                + std::format(wxS("{}"), pullRequestId)
                + wxS("/diffstat");

        return PerformRequest(url);
    }
};
