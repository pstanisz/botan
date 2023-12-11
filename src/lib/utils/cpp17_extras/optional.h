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
template< typename T>
using optional = std::experimental::optional<T>;

constexpr std::experimental::nullopt_t nullopt = std::experimental::nullopt;

}

#endif
