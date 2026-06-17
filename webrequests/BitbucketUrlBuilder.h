#pragma once

#include <wx/string.h>

const wxString BitBucketApiBaseUrl = "https://api.bitbucket.org/2.0";

class BitbucketUrlBuilder
{
public:
    static wxString GetPullRequestsUrl(const Repository& repository)
    {
        const auto repoKey = repository.GetRepoKey();
        return BitBucketApiBaseUrl
                + wxS("/repositories/")
                + repoKey
                + wxS("/pullrequests/");
    }

    static wxString GetPullRequestsUrl(const Repository& repository, int pullRequestId)
    {
        return
                GetPullRequestsUrl(repository)
                + std::format(wxS("{}"), pullRequestId);
    }

    static wxString GetCurrentUserUrl()
    {
        return BitBucketApiBaseUrl + wxS("/user/");
    }

    static wxString GetDiffStatUrl(const Repository& repository, int pullRequestId)
    {
        return
                GetPullRequestsUrl(repository, pullRequestId)
                + wxS("/diffstat");
    }

    static wxString GetQueryPullRequestsUrl(const Repository& repository, const wxString& userUuid)
    {
        auto userId = userUuid;
        userId.Replace(wxS("{"), wxS(""));
        userId.Replace(wxS("}"), wxS(""));

        return
                GetPullRequestsUrl(repository)
                + std::format(wxS("?pagelen=50&q=state%3D%22open%22%20AND%20(reviewers.uuid%3D%22{0}%22%20OR%20author.uuid%3D%22{0}%22)"), userId);
    }

    static wxString GetRepositoriesUrl(const Workspace& workspace)
    {
        return BitBucketApiBaseUrl + wxS("/repositories/") + workspace.slug;
    }

    static wxString GetStatusesUrl(const Repository& repository, int pullRequestId)
    {
        return
                GetPullRequestsUrl(repository, pullRequestId)
                + wxS("/statuses");
    }

    static wxString GetWorkspacesUrl()
    {
        return BitBucketApiBaseUrl + wxS("/user/workspaces");
    }
};
