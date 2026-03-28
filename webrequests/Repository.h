#pragma once

#include <wx/string.h>
#include <nlohmann/json.hpp>

#include "Workspace.h"
#include "../wxJson.h"

struct Repository
{
    Workspace workspace;
    wxString full_name;
    wxString slug;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Repository, full_name, slug, workspace);

struct RepositoriesResponse
{
    std::vector<Repository> values;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RepositoriesResponse, values);
