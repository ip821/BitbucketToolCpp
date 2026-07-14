#pragma once

#include <expected>

#include "PullRequest.h"
#include "Status.h"
#include "Structs.h"
#include "bitbucket_api/BitbucketResponse.h"

class CurrentUserRequest
{
public:
    [[nodiscard]] BitbucketResponse<User> GetCurrentUser(const std::string& authToken) const;
};

class DiffStatRequest
{
public:
    [[nodiscard]] BitbucketResponse<DiffStat> GetDiffStat(const std::string& authToken, const Repository& repository, const int pullRequestId) const;
};

class PullRequestRequest
{
public:
    [[nodiscard]] BitbucketResponse<PullRequest> GetPullRequest(const std::string& authToken, const Repository& repository, int pullRequestId) const;
};

class PullRequestsRequest
{
public:
    [[nodiscard]] BitbucketResponse<Values<PullRequestItem>> GetPullRequests(const std::string& authToken, const Repository& repository, const std::string& userUuid) const;
};

class RepositoriesRequest
{
public:
    [[nodiscard]] BitbucketResponse<Values<Repository>> GetRepositories(const std::string& authToken, const Workspace& workspace) const;
};

class StatusRequest
{
public:
    [[nodiscard]] BitbucketResponse<Values<Status>> GetStatuses(const std::string& authToken, const Repository& repository, int pullRequestId) const;
};

class WorkspacesRequest
{
public:
    [[nodiscard]] BitbucketResponse<std::vector<Workspace>> GetWorkspaces(const std::string& authToken) const;
};
