#pragma once

#include "BitbucketRequest.h"
#include "Repository.h"
#include "Status.h"

class StatusRequest : public BitbucketRequest<Values<Status>>
{
public:
    TResponse GetStatuses(const Repository& repository, int pullRequestId) const
    {
        return PerformRequest(BitbucketUrlBuilder::GetStatusesUrl(repository, pullRequestId));
    }
};
