/*
* Implementation of C++20 type_traits replacement for compilers not supporting C++20
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_TYPE_TRAITS_H_
#define BOTAN_TYPE_TRAITS_H_

#include <type_traits>

namespace Botan {

template <typename T>
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

}

#endif
