#pragma once

#include <optional>
#include <nlohmann/json.hpp>

struct InternalServerError
{
    std::string message{};
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
