/*
* Implementation of C++17 attributes replacement for compilers not supporting C++20
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_CPP17_ATTRS_H_
#define BOTAN_CPP17_ATTRS_H_

namespace Botan {

#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)

#define NO_DISCARD _Check_return_

#elif defined(BOTAN_BUILD_COMPILER_IS_GCC) || defined(BOTAN_BUILD_COMPILER_IS_CLANG)

#define NO_DISCARD __attribute__((warn_unused_result))

#else

#define NO_DISCARD /*nothing*/

#endif

// TODO: pstanisz
#define MAYBE_UNUSED /*nothing*/

}

#endif
