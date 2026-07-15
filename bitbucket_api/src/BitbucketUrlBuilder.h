#pragma once

#include "cpp_utils/strings.h"
#include "bitbucket_api/Structs.h"

const std::string BitBucketApiBaseUrl = "https://api.bitbucket.org/2.0";

class BitbucketUrlBuilder
{
public:
    static std::string GetPullRequestsUrl(const Repository& repository)
    {
        const auto repoKey = repository.GetRepoKey();
        return BitBucketApiBaseUrl
                + "/repositories/"
                + repoKey
                + "/pullrequests/";
    }

    static std::string GetPullRequestsUrl(const Repository& repository, int pullRequestId)
    {
        return
                GetPullRequestsUrl(repository)
                + std::format("{}", pullRequestId);
    }

    static std::string GetCurrentUserUrl()
    {
        return BitBucketApiBaseUrl + "/user/";
    }

    static std::string GetDiffStatUrl(const Repository& repository, int pullRequestId)
    {
        return GetPullRequestsUrl(repository, pullRequestId) + "/diffstat";
    }

    static std::string GetQueryPullRequestsUrl(const Repository& repository, const std::string& userUuid)
    {
        auto userId = userUuid;
        ip::strings::replace_all(userId, "{", "");
        ip::strings::replace_all(userId, "}", "");

        return
                GetPullRequestsUrl(repository)
                + std::format("?pagelen=50&q=state%3D%22open%22%20AND%20(reviewers.uuid%3D%22{0}%22%20OR%20author.uuid%3D%22{0}%22)", userId);
    }

    static std::string GetRepositoriesUrl(const Workspace& workspace)
    {
        return BitBucketApiBaseUrl + "/repositories/" + workspace.slug;
    }

    static std::string GetStatusesUrl(const Repository& repository, const int pullRequestId)
    {
        return GetPullRequestsUrl(repository, pullRequestId) + "/statuses";
    }

    static std::string GetWorkspacesUrl()
    {
        return BitBucketApiBaseUrl + "/user/workspaces";
    }
};
