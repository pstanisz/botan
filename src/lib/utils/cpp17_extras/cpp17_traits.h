/*
* Implementation of C++17 type_traits replacement for compilers not supporting C++20
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_CPP17_TRAITS_H_
#define BOTAN_CPP17_TRAITS_H_

namespace Botan {

template <typename...>
using void_t = void;

}

#endif
