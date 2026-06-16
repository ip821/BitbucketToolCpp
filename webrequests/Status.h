#pragma once
#include <nlohmann/detail/macro_scope.hpp>

enum StatusState
{
    Successful = 1,
    Failed,
    InProgress,
    Stopped,
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    StatusState,
    {
    {StatusState::Successful, "SUCCESSFUL"},
    {StatusState::Failed, "FAILED"},
    {StatusState::InProgress, "INPROGRESS"},
    {StatusState::Stopped, "STOPPED"},
    }
);

struct Status
{
    StatusState state{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Status, state);
