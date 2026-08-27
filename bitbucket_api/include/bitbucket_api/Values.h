#pragma once

#include <optional>
#include <string>
#include <vector>

template<typename TItem>
struct Values
{
    std::vector<TItem> values{};
    std::optional<std::string> next{};
};
