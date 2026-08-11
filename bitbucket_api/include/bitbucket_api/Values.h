#pragma once

#include <vector>
#include <unordered_set>
#include <optional>

template<typename T>
concept HasId = requires(const T& item)
{
    item.id;
};

template<typename TItem>
struct Values
{
    std::vector<TItem> values{};
    std::optional<std::string> next{};

    std::vector<TItem> DistinctById() const
        requires HasId<TItem>
    {
        std::vector<TItem> uniqueItems;
        std::unordered_set<decltype(TItem::id)> processedIds;

        for (const auto& item: values)
        {
            if (processedIds.insert(item.id).second)
                uniqueItems.push_back(item);
        }
        return uniqueItems;
    }
};
