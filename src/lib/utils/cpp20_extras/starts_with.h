/*
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_STARTS_WITH_H_
#define BOTAN_STARTS_WITH_H_

#include <string>
#include <botan/string_view.h>

namespace Botan {

namespace {

constexpr unsigned long ZERO_POS = 0UL;

}

// Replaces C++20 member method starts_with available in std::string or Botan::string_view
inline bool starts_with(Botan::string_view str, Botan::string_view what)
    { return (str.find(what) == ZERO_POS); }

}

#endif