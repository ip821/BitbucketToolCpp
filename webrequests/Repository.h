#pragma once

#include <wx/string.h>
#include <nlohmann/json.hpp>
#include <cpp_utils/wx_json.h>

#include "Workspace.h"

struct Repository
{
    Workspace workspace{};
    wxString full_name{};
    wxString slug{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Repository, full_name, slug, workspace);
