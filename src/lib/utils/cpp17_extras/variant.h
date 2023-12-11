/*
* Implementation of C++17 variant replacement for compilers not supporting C++17
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_VARIANT_H_
#define BOTAN_VARIANT_H_

#include <boost/variant.hpp>

namespace Botan {

// Replaces C++17 std::variang
template <typename... Ts>
using variant = boost::variant<Ts...>;\

template <typename T, typename... Ts>
constexpr bool holds_alternative(const Botan::variant<Ts...>& v) noexcept
{
    return holds_alternative(v);
}

template <typename Visitor, typename... Variants>
constexpr decltype(auto) visit(Visitor&& vis, Variants&&... vars) {
    return visit(vis, vars...);
}

}

#endif
