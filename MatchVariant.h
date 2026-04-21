#pragma once

#include <variant>

template <class... Ts>
struct Match : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Match(Ts...) -> Match<Ts...>;

template <class Variant, class... Ts>
decltype(auto) MatchVariant(Variant&& v, Ts&&... ts)
{
    return std::visit(
        Match{std::forward<Ts>(ts)...},
        std::forward<Variant>(v)
        );
}