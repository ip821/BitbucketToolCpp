#pragma once

#include <chrono>

class Stopwatch
{
    using Clock = std::chrono::steady_clock;

    const Clock::time_point m_startedAt{Clock::now()};

public:
    [[nodiscard]] std::chrono::seconds GetElapsed() const;
};
