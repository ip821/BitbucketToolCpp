#pragma once

#include "wx/string.h"
#include <cpp_utils/wx_json.h>

struct User
{
    wxString display_name{};
    wxString uuid{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, display_name, uuid);