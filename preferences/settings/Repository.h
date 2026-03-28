#pragma once

#include <wx/string.h>
#include <nlohmann/json.hpp>

#include "../../wxJson.h"

struct Workspace
{
    wxString slug;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Workspace, slug);

struct Repository
{
    Workspace workspace;
    wxString full_name;
    wxString slug;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Repository, full_name, slug, workspace);
