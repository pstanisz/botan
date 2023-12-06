/*
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_ERASE_IF_H_
#define BOTAN_ERASE_IF_H_

#include <map>

namespace Botan {

// Replaces C++20 member method erase_if available in std::map
template <typename Key, typename T, typename Compare, typename Alloc, typename Pred>
typename std::map<Key, T, Compare, Alloc>::size_type erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred) {
   auto old_size = c.size();
   for(auto first = c.begin(), last = c.end(); first != last;) {
      if(pred(*first)) {
         first = c.erase(first);
      } else {
         ++first;
      }
   }
   return old_size - c.size();
}

}  // namespace Botan

#endif
