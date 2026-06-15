#pragma once

#include <wx/string.h>
#include <nlohmann/json.hpp>
#include <cpp_utils/wx_json.h>
#include <cpp_utils/wx_string_format.h>

#include "Workspace.h"

struct Repository
{
    Workspace workspace{};
    wxString full_name{};
    wxString slug{};

    wxString GetRepoKey() const
    {
        return std::format(wxS("{0}/{1}"), workspace.slug, slug);
    }
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Repository, full_name, slug, workspace);
