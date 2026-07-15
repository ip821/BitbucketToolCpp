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

BitbucketResponse<Values<PullRequestItem> > PullRequestsRequest::GetPullRequests(const std::string& authToken, const Repository& repository, const std::string& userUuid) const
{
    const BitbucketRequest<Values<PullRequestItem> > request(BitbucketUrlBuilder::GetQueryPullRequestsUrl(repository, userUuid));
    return request.Perform(authToken);
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
    const BitbucketRequest<Values<WorkspaceAccess>> request(BitbucketUrlBuilder::GetWorkspacesUrl());
    const auto result = request.Perform(authToken);
    UNWRAP_OR_RETURN_ERROR(repositories, result);

    const auto workspaces = repositories.values
                            | std::views::transform([](const auto& it) { return it.workspace; })
                            | std::views::filter([](const auto& it) { return !it.slug.empty(); })
                            | std::ranges::to<std::vector>();

    return workspaces;
}
