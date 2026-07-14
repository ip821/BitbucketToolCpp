#pragma once

enum class StatusState
{
    Successful = 1,
    Failed,
    InProgress,
    Stopped,
};

struct Status
{
    StatusState state{};
};

