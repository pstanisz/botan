/*
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_CONTAINS_H_
#define BOTAN_CONTAINS_H_

namespace Botan {

template <typename C>
inline bool contains(const C& container, const typename C::key_type& what)
    { return container.find(what) != std::end(container); };

}

#endif