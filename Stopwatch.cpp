#include "Stopwatch.h"

std::chrono::seconds Stopwatch::GetElapsed() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_startedAt);
}
