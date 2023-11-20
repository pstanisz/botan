/*
* Implementation of C++20 std::span replacement for compilers not supporting C++20
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_SPAN_H_
#define BOTAN_SPAN_H_

#include <span>

namespace Botan {

template <typename T, std::size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;

}

#endif