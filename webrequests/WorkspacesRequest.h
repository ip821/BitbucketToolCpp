#pragma once

#include "../http/HttpConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

struct WorkspacesSuccess
{
    std::vector<Workspace> workspaces;
};
using WorkspacesResult = std::expected<WorkspacesSuccess, Error>;

class WorkspacesRequest
{
    const HttpConnection& m_connection;

public:
    explicit WorkspacesRequest(const HttpConnection& connection);

    [[nodiscard]] WorkspacesResult GetWorkspaces() const;
};
