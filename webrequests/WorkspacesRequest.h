#pragma once

#include "CurlConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"
#include "wx/arrstr.h"

struct WorkspacesSuccess
{
    std::vector<Workspace> workspaces;
};
using WorkspacesResult = std::variant<WorkspacesSuccess, Error>;

class WorkspacesRequest
{
    const CurlConnection& m_connection;

public:
    explicit WorkspacesRequest(const CurlConnection& connection);

    WorkspacesResult GetWorkspaces() const;
};
