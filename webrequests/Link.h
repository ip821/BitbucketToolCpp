#pragma once

#include "wx/string.h"
#include <cpp_utils/wx_json.h>

struct Link
{
    wxString href{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Link, href);
