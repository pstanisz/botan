/*
* (C) 2023 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_PREFETCH_UTILS_H_
#define BOTAN_PREFETCH_UTILS_H_

#include <botan/types.h>
#include <type_traits>

namespace Botan {

/**
* Prefetch an array
*
* This function returns a uint64_t which is accumulated from values
* read from the array. This may help confuse the compiler sufficiently
* to not elide otherwise "useless" reads. The return value will always
* be zero.
*/
uint64_t prefetch_array_raw(size_t bytes, const void* array) noexcept;

/**
* Prefetch single array
*/
template<typename T, size_t N>
T prefetch_arrays(T (&arr)[N]) noexcept
   {
   static_assert(std::is_integral<T>::value, "prefetch_arrays must use integral type");
   return static_cast<T>(prefetch_array_raw(sizeof(T)*N, arr));
   }

/**
* Prefetch several arrays
*
* This function returns a uint64_t which is accumulated from values
* read from the array. This may help confuse the compiler sufficiently
* to not elide otherwise "useless" reads. The return value will always
* be zero.
*/
template<typename T, size_t N, size_t... Ns>
T prefetch_arrays(T (&array)[N], T (&...arrays)[Ns]) noexcept
   {
   static_assert(std::is_integral<T>::value, "prefetch_arrays must use integral type");
   return (static_cast<T>(prefetch_array_raw(sizeof(T)*N, array)) & prefetch_arrays(arrays...));
   }

}

#endif
