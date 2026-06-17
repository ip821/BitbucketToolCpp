#pragma once

#include "BitbucketRequest.h"
#include "DiffStat.h"
#include "Repository.h"

class DiffStatRequest : public BitbucketRequest<DiffStat>
{
public:
    TResponse GetDiffStat(const Repository& repository, int pullRequestId) const
    {
        return PerformRequest(BitbucketUrlBuilder::GetDiffStatUrl(repository, pullRequestId));
    }
};
