#pragma once

#include <expected>

#include <wx/string.h>
#include <cpp_utils/wx_string_format.h>

#include "BitbucketRequest.h"
#include "PullRequest.h"
#include "Repository.h"
#include "Workspace.h"

class PullRequestsRequest : BitbucketRequest<Values<PullRequestItem> >
{
public:
    TResponse GetPullRequests(const Repository& repository, const wxString& userUuid) const
    {
        return PerformRequest(BitbucketUrlBuilder::GetQueryPullRequestsUrl(repository, userUuid));
    }
};
