#pragma once

#include <expected>
#include <wx/string.h>

#include "BitbucketClient.h"
#include "PullRequest.h"
#include "Repository.h"
#include "Workspace.h"
#include "../Constants.h"
#include <cpp_utils/wx_string_format.h>

class PullRequestsRequest : BitbucketClient<Values<PullRequestItem> >
{
public:
    TResponse GetPullRequests(const Repository& repository, const wxString& userUuid) const
    {
        const auto repoKey = repository.GetRepoKey();

        auto userId = userUuid;
        userId.Replace(wxS("{"), wxS(""));
        userId.Replace(wxS("}"), wxS(""));

        const auto url = BitBucketBaseUrl
                + wxS("/repositories/")
                + repoKey
                + wxS("/pullrequests/")
                + std::format(wxS("?pagelen=50&q=state%3D%22open%22%20AND%20(reviewers.uuid%3D%22{0}%22%20OR%20author.uuid%3D%22{0}%22)"), userId);

        return PerformRequest(url);
    }
};
