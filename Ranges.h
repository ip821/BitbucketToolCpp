#pragma once

#ifdef __WXMSW__
#include <ranges>
#include <utility>
#include <vector>

namespace std::ranges
{
#include <vector>
#include <ranges>
#include <utility>

    template <template<class...> class Container>
    struct to_closure
    {
        template <std::ranges::input_range R>
        auto operator()(R&& r) const
        {
            using T = std::ranges::range_value_t<R>;
            Container<T> out;

            if constexpr (std::ranges::sized_range<R> && requires(Container<T> c, std::size_t n) { c.reserve(n); })
                out.reserve(std::ranges::size(r));

            for (auto&& x : r)
                out.push_back(std::forward<decltype(x)>(x));

            return out;
        }
    };

    template <template<class...> class Container>
    constexpr auto to()
    {
        return to_closure<Container>{};
    }

    template <std::ranges::input_range R, template<class...> class Container>
    auto operator|(R&& r, to_closure<Container> closure)
    {
        return closure(std::forward<R>(r));
    }
}

#endif
