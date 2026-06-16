#pragma once
#include "BitbucketClient.h"
#include "Repository.h"
#include "Status.h"
#include "../Constants.h"

class StatusRequest : public BitbucketClient<Values<Status>>
{
public:
    TResponse GetStatuses(const Repository& repository, int pullRequestId) const
    {
        const auto repoKey = repository.GetRepoKey();

        const auto url = BitBucketBaseUrl
                + wxS("/repositories/")
                + repoKey
                + wxS("/pullrequests/")
                + std::format(wxS("{}"), pullRequestId)
                + wxS("/statuses");

        return PerformRequest(url);
    }
};
