#include <string>
#include <cpp_utils/macros_expected.h>

#include "bitbucket_api/Requests.h"
#include "BitbucketRequest.h"
#include "BitbucketUrlBuilder.h"
#include "bitbucket_api/Structs.h"
#include "bitbucket_api/StructsSerialization.h"

BitbucketResponse<User> CurrentUserRequest::GetCurrentUser(const std::string& authToken) const
{
    const BitbucketRequest<User> request(BitbucketUrlBuilder::GetCurrentUserUrl());
    return request.Perform(authToken);
}

BitbucketResponse<DiffStat> DiffStatRequest::GetDiffStat(const std::string& authToken, const Repository& repository, const int pullRequestId) const
{
    const BitbucketRequest<DiffStat> request(BitbucketUrlBuilder::GetDiffStatUrl(repository, pullRequestId));
    return request.Perform(authToken);
}

BitbucketResponse<PullRequest> PullRequestRequest::GetPullRequest(const std::string& authToken, const Repository& repository, int pullRequestId) const
{
    const BitbucketRequest<PullRequest> request(BitbucketUrlBuilder::GetPullRequestsUrl(repository, pullRequestId));
    return request.Perform(authToken);
}

BitbucketResponse<Values<PullRequest> > PullRequestsRequest::GetPullRequests(const std::string& authToken, const Repository& repository, const std::string& userUuid) const
{
    auto url = BitbucketUrlBuilder::GetQueryPullRequestsUrl(repository, userUuid);
    std::vector<PullRequest> allPullRequests;

    while (url != "")
    {
        const BitbucketRequest<Values<PullRequest> > request(url);
        UNWRAP_OR_RETURN_ERROR(pullRequests, request.Perform(authToken));
        allPullRequests.append_range(pullRequests.values);

        if (pullRequests.next.has_value())
            url = pullRequests.next.value();
        else
            url = "";
    }

    return Values{.values = std::move(allPullRequests)};
}

BitbucketResponse<Values<Repository> > RepositoriesRequest::GetRepositories(const std::string& authToken, const Workspace& workspace) const
{
    const BitbucketRequest<Values<Repository> > request(BitbucketUrlBuilder::GetRepositoriesUrl(workspace));
    return request.Perform(authToken);
}

BitbucketResponse<Values<Status> > StatusRequest::GetStatuses(const std::string& authToken, const Repository& repository, int pullRequestId) const
{
    const BitbucketRequest<Values<Status> > request(BitbucketUrlBuilder::GetStatusesUrl(repository, pullRequestId));
    return request.Perform(authToken);
}

BitbucketResponse<std::vector<Workspace> > WorkspacesRequest::GetWorkspaces(const std::string& authToken) const
{
    const BitbucketRequest<Values<WorkspaceAccess> > request(BitbucketUrlBuilder::GetWorkspacesUrl());
    UNWRAP_OR_RETURN_ERROR(repositories, request.Perform(authToken));

    auto workspaces = repositories.values
        | std::views::transform([](const auto& it) { return it.workspace; })
        | std::views::filter([](const auto& it) { return !it.slug.empty(); })
        | std::ranges::to<std::vector>();

    return workspaces;
}
