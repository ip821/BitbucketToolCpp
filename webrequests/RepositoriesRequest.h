#pragma once

#include <expected>

#include "BitbucketRequest.h"
#include "BitbucketUrlBuilder.h"
#include "Values.h"
#include "../preferences/wizard/SetupWizardContext.h"

class RepositoriesRequest : public BitbucketRequest<Values<Repository> >
{
public:
    [[nodiscard]] TResponse GetRepositories(const Workspace& workspace) const
    {
        return PerformRequest(BitbucketUrlBuilder::GetRepositoriesUrl(workspace));
    }
};
