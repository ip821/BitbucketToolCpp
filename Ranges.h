#pragma once

#ifdef __WXMSW__
#include <ranges>
#include <utility>
#include <vector>

struct to_std_vector
{
    template <std::ranges::input_range R>
    auto operator()(R&& r) const
    {
        using T = std::ranges::range_value_t<R>;
        std::vector<T> out;

        if constexpr (std::ranges::sized_range<R>)
            out.reserve(std::ranges::size(r));

        for (auto&& x : r)
            out.push_back(x);

        return out;
    }
};

template <std::ranges::input_range R>
auto operator|(R&& r, const to_std_vector& fn)
{
    return fn(std::forward<R>(r));
}

#else
#define to_std_vector to<std::vector>
#endif
