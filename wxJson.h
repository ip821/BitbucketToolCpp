#pragma once

#include <wx/string.h>
#include <nlohmann/json.hpp>

namespace nlohmann
{
    template <>
    struct adl_serializer<wxString>
    {
        static void to_json(json& j, const wxString& value)
        {
            j = value.ToStdString();
        }

        static void from_json(const json& j, wxString& value)
        {
            value = wxString::FromUTF8(j.get<std::string>());
        }
    };
}