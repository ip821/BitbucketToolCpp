#pragma once
#include <nlohmann/json.hpp>

struct DiffStat
{
    int size{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiffStat, size);
