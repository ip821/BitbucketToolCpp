#pragma once

#include <variant>

template <class... Ts>
struct Switch : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Switch(Ts...) -> Switch<Ts...>;

template <class Variant, class... Ts>
decltype(auto) Match(Variant&& v, Ts&&... ts)
{
    return std::visit(
        Switch{std::forward<Ts>(ts)...},
        std::forward<Variant>(v)
        );
}

template <typename Exp, typename OnValue, typename OnError>
decltype(auto) Match(Exp&& exp, OnValue&& on_value, OnError&& on_error)
{
    if (exp.has_value()) {
        return std::invoke(
            std::forward<OnValue>(on_value),
            *std::forward<Exp>(exp));
    }

    return std::invoke(
        std::forward<OnError>(on_error),
        std::forward<Exp>(exp).error());
}