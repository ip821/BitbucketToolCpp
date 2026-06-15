#pragma once

#include <expected>

#include "BitbucketClient.h"
#include "Values.h"
#include "../Constants.h"
#include "../http/HttpConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

class RepositoriesRequest : public BitbucketClient<Values<Repository> >
{
public:
    [[nodiscard]] TResponse GetRepositories(const Workspace& workspace) const
    {
        return PerformRequest(BitBucketBaseUrl + "/repositories/" + workspace.slug);
    }
};
