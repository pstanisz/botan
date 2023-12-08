/*
* Implementation of C++17 string_view replacement for compilers not supporting C++17
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_STRING_VIEW_H_
#define BOTAN_STRING_VIEW_H_

#include <string>

namespace Botan {

// Replaces C++17 std::string_view
using string_view = std::string;

}

#endif
