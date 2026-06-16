#pragma once

#include "wx/string.h"
#include <cpp_utils/wx_json.h>

#include "User.h"

enum ParticipantRole
{
    Reviewer = 1,
    Participant,
    Author,
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    ParticipantRole,
    {
    {ParticipantRole::Reviewer, "REVIEWER"},
    {ParticipantRole::Participant, "PARTICIPANT"},
    {ParticipantRole::Author, "AUTHOR"},
    }
);

enum ParticipantState
{
    None = 1,
    ChangesRequested,
    Approved,
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    ParticipantState,
    {
    {ParticipantState::None, nullptr},
    {ParticipantState::ChangesRequested, "changes_requested"},
    {ParticipantState::Approved, "approved"}
    }
);

struct ParticipantUser
{
    ParticipantState state{};
    ParticipantRole role{};
    bool approved{};
    User user{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ParticipantUser, state, role, approved, user);
