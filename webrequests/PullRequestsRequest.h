#pragma once

#include <expected>
#include <wx/string.h>

#include "PullRequest.h"
#include "Repository.h"
#include "Workspace.h"
#include "../http/HttpConnection.h"

struct PullRequestsSuccess
{
    std::vector<PullRequestItem> pull_requests;
};

using PullRequestsResult = std::expected<PullRequestsSuccess, Error>;

class PullRequestsRequest
{
    const HttpConnection& m_connection;

public:
    explicit PullRequestsRequest(const HttpConnection& connection);
    [[nodiscard]] PullRequestsResult GetPullRequests(const Repository& repository, const wxString& userUuid) const;
};
