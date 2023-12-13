/*
* Implementation of C++17 optional replacement for compilers not supporting C++17
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_OPTIONAL_H_
#define BOTAN_OPTIONAL_H_

#include <experimental/optional>

namespace Botan {

// Replaces C++17 std::optional
template <typename T>
using optional = std::experimental::optional<T>;

constexpr std::experimental::nullopt_t nullopt = std::experimental::nullopt;

template <typename T>
bool has_value(const Botan::optional<T>& opt) noexcept
{
    return static_cast<bool>(opt);
}

template <typename T>
void reset(Botan::optional<T>& opt) noexcept
{
    opt = Botan::optional<T>{};
}

template<typename _Tp>
constexpr Botan::optional<std::decay_t<_Tp>>
make_optional(_Tp&& __t)
{ return Botan::optional<std::decay_t<_Tp>> { std::forward<_Tp>(__t) }; }

template<typename _Tp>
constexpr Botan::optional<std::decay_t<_Tp>>
make_optional()
{ return Botan::optional<std::decay_t<_Tp>> {}; }

}

#endif
