#pragma once

#include "../curl/CurlConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

struct WorkspacesSuccess
{
    std::vector<Workspace> workspaces;
};
using WorkspacesResult = std::expected<WorkspacesSuccess, Error>;

class WorkspacesRequest
{
    const CurlConnection& m_connection;

public:
    explicit WorkspacesRequest(const CurlConnection& connection);

    [[nodiscard]] WorkspacesResult GetWorkspaces() const;
};
