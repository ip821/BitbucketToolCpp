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
