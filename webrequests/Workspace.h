#pragma once

#include <cpp_utils/wx_json.h>

struct Workspace
{
    wxString slug;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Workspace, slug);

struct WorkspaceAccess
{
    Workspace workspace;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceAccess, workspace);

struct WorkspacesResponse
{
    std::vector<WorkspaceAccess> values;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspacesResponse, values);
