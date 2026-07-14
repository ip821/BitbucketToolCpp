#pragma once

#include "bitbucket_api/Structs.h"

enum class ParticipantRole
{
    Reviewer = 1,
    Participant,
    Author,
};

enum class ParticipantState
{
    None = 1,
    ChangesRequested,
    Approved,
};

struct ParticipantUser
{
    ParticipantState state{};
    ParticipantRole role{};
    bool approved{};
    User user{};
};
