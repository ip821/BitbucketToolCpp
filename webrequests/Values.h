#pragma once
#include <vector>
#include <nlohmann/detail/macro_scope.hpp>

template<typename TItem>
struct Values
{
    std::vector<TItem> values;
};

template<typename T>
void to_json(nlohmann::json& j, const Values<T>& obj)
{
    j = nlohmann::json{
        {"values", obj.values}
    };
}

template<typename T>
void from_json(const nlohmann::json& j, Values<T>& obj)
{
    j.at("values").get_to(obj.values);
}
