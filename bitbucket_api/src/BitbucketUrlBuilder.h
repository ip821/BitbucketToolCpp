#pragma once

#include <cpp_curl/CurlUrl.h>

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

        const auto query = std::format(
            R"(state="open" AND ((reviewers.uuid="{0}" AND draft=false) OR author.uuid="{0}"))",
            userId);

        CurlUrl url(GetPullRequestsUrl(repository));
        url.AppendQueryParameter("pagelen", "50");
        url.AppendQueryParameter("q", query);
        url.AppendQueryParameter("fields", "+values.participants");
        return url.GetUrl();
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
