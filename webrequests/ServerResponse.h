#pragma once

#include <optional>

#include <nlohmann/detail/macro_scope.hpp>
#include <cpp_utils/wx_json.h>
#include <wx/string.h>

struct InternalServerError
{
    wxString message{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalServerError, message);

struct ServerResponse
{
    std::optional<InternalServerError> error;
};
inline void from_json(const nlohmann::json& json, ServerResponse& serverResponse)
{
    if (json.contains("error") && !json.at("error").is_null())
        serverResponse.error = json.at("error").get<InternalServerError>();
    else
        serverResponse.error = std::nullopt;
}
