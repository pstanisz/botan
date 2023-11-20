/**
 * Useful concepts that are available throughout the library
 * (C) 2023 Jack Lloyd
 *     2023 René Meusel - Rohde & Schwarz Cybersecurity
 *     2023 Piotr Staniszewski - CodeVision
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#ifndef BOTAN_CONCEPTS_H_
#define BOTAN_CONCEPTS_H_

#include <compare>
#include <concepts>
#include <type_traits>
#include <ostream>
#include <cstdint>

#define NO_CPP20

namespace Botan::concepts {

#ifndef NO_CPP20

// TODO: C++20 use std::convertible_to<> that was not available in Android NDK
//       as of this writing. Tested with API Level up to 33.
template <class FromT, class ToT>
concept convertible_to =
  std::is_convertible_v<FromT, ToT> &&
  requires {
    static_cast<ToT>(std::declval<FromT>());
  };

// TODO: C++20 provides concepts like std::equality_comparable or
//       std::three_way_comparable, but at the time of this writing, some
//       target platforms did not ship with those (Xcode 14, Android NDK r25,
//       emscripten)

template<typename T>
concept equality_comparable = requires(const std::remove_reference_t<T>& a, const std::remove_reference_t<T> b)
   {
   { a == b } -> convertible_to<bool>;
   };

template<typename T>
concept three_way_comparison_result =
   convertible_to<T, std::weak_ordering> ||
   convertible_to<T, std::partial_ordering> ||
   convertible_to<T, std::strong_ordering>;

template<typename T>
concept three_way_comparable = requires(const std::remove_reference_t<T>& a, const std::remove_reference_t<T> b)
   {
   { a <=> b } -> three_way_comparison_result;
   };

template<class T>
concept destructible = std::is_nothrow_destructible_v<T>;

template<class T, class... Args>
concept constructible_from =
   destructible<T> && std::is_constructible_v<T, Args...>;

template<class T>
concept default_initializable =
    constructible_from<T> &&
    requires { T{}; } &&
    requires { ::new (static_cast<void*>(nullptr)) T; };

// TODO: C++20 provides concepts like std::ranges::range or ::sized_range
//       but at the time of this writing clang had not caught up on all
//       platforms. E.g. clang 14 on Xcode does not support ranges properly.

template<typename IterT, typename ContainerT>
concept container_iterator =
   std::same_as<IterT, typename ContainerT::iterator> ||
   std::same_as<IterT, typename ContainerT::const_iterator>;

template<typename PtrT, typename ContainerT>
concept container_pointer =
   std::same_as<PtrT, typename ContainerT::pointer> ||
   std::same_as<PtrT, typename ContainerT::const_pointer>;


template<typename T>
concept container = requires(T a)
   {
   { a.begin() } -> container_iterator<T>;
   { a.end() } -> container_iterator<T>;
   { a.cbegin() } -> container_iterator<T>;
   { a.cend() } -> container_iterator<T>;
   { a.size() } -> std::same_as<typename T::size_type>;
   typename T::value_type;
   };

template<typename T>
concept contiguous_container =
   container<T> &&
   requires(T a)
   {
   { a.data() } -> container_pointer<T>;
   };

template<typename T>
concept has_empty = requires(T a)
   {
   { a.empty() } -> std::same_as<bool>;
   };

template <typename T>
concept resizable_container =
   container<T> &&
   requires(T& c, typename T::size_type s)
      {
      T(s);
      c.resize(s);
      };

template <typename T>
concept resizable_byte_buffer =
   contiguous_container<T> &&
   resizable_container<T> &&
   std::same_as<typename T::value_type, uint8_t>;

template<typename T>
concept streamable = requires(std::ostream& os, T a)
   { os << a; };

#endif

// TODO: C++20 use std::convertible_to<> that was not available in Android NDK
//       as of this writing. Tested with API Level up to 33.
template <class FromT, class ToT>
using convertible_to = typename std::void_t<decltype(std::is_convertible_v<FromT, ToT>), decltype(static_cast<ToT>(std::declval<FromT>()))>;

template <class FromT, class ToT, typename = void>
struct is_convertible_to : std::false_type {};

template <class FromT, class ToT>
struct is_convertible_to<FromT, ToT, convertible_to<FromT, ToT>> : std::true_type {};

template <class FromT, class ToT>
constexpr bool is_convertible_to_v = is_convertible_to<FromT, ToT>::value;

template <typename T>
using equality_comparable = typename std::enable_if_t<
    is_convertible_to_v<decltype(std::declval<const std::remove_reference_t<T> &>() == std::declval<const std::remove_reference_t<T>>()), bool>>;

template <typename T, typename = void>
struct is_equality_comparable : std::false_type {};

template <typename T>
struct is_equality_comparable<T, equality_comparable<T>> : std::true_type {};

template <typename T>
constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;

template <typename T>
using less_comparable = typename std::enable_if_t<
    is_convertible_to_v<decltype(std::declval<const std::remove_reference_t<T> &>() < std::declval<const std::remove_reference_t<T>>()), bool>>;

template <typename T>
using greater_comparable = typename std::enable_if_t<
    is_convertible_to_v<decltype(std::declval<const std::remove_reference_t<T> &>() > std::declval<const std::remove_reference_t<T>>()), bool>>;

template <typename T>
using destructible = typename std::is_nothrow_destructible<T>;

template <class T>
constexpr bool is_destructible_v = destructible<T>::value;

template <class T, class... Args>
using constructible_from = typename std::enable_if_t<is_destructible_v<T> && std::is_constructible_v<T, Args...>>;

template <class T, class... Args>
constexpr bool is_constructible_from_v = is_destructible_v<T> && std::is_constructible_v<T, Args...>;

template <typename T>
using default_initializable = typename std::void_t<std::enable_if_t<is_constructible_from_v<T>>, decltype(T{}), decltype(::new (static_cast<void *>(nullptr)) T)>;

template <class T, typename = void>
struct is_default_initializable : std::false_type {};

template <class T>
struct is_default_initializable<T, default_initializable<T>> : std::true_type {};

template <class T>
constexpr bool is_default_initializable_v = is_default_initializable<T>::value;

template <typename T, typename U, typename = void>
struct same_as : std::false_type {};

template <typename T, typename U>
struct same_as<T, U, std::enable_if_t<std::is_same_v<T, U> && std::is_same_v<U, T>>> : std::true_type {};

template <typename T, typename U>
constexpr bool same_as_v = same_as<T, U>::value;

template <typename IterT, typename ContainerT>
using container_iterator = typename std::enable_if_t<
    same_as_v<IterT, typename ContainerT::iterator> || same_as_v<IterT, typename ContainerT::const_iterator>>;

template <typename IterT, typename ContainerT, typename = void>
struct is_container_iterator : std::false_type {};

template <typename IterT, typename ContainerT>
struct is_container_iterator<IterT, ContainerT, container_iterator<IterT, ContainerT>> : std::true_type {};

template <typename IterT, typename ContainerT>
constexpr bool is_container_iterator_v = is_container_iterator<IterT, ContainerT>::value;

template <typename PtrT, typename ContainerT>
using container_pointer = typename std::enable_if_t<
    same_as_v<PtrT, typename ContainerT::pointer> || same_as_v<PtrT, typename ContainerT::const_pointer>>;

template <typename PtrT, typename ContainerT, typename = void>
struct is_container_pointer : std::false_type {};

template <typename PtrT, typename ContainerT>
struct is_container_pointer<PtrT, ContainerT, container_pointer<PtrT, ContainerT>> : std::true_type {};

template <typename PtrT, typename ContainerT>
constexpr bool is_container_pointer_v = is_container_pointer<PtrT, ContainerT>::value;

template <typename T>
using container = typename std::enable_if_t<is_container_iterator_v<decltype(std::declval<T &>().begin()), T> &&
                                            is_container_iterator_v<decltype(std::declval<T &>().end()), T> &&
                                            is_container_iterator_v<decltype(std::declval<T &>().cbegin()), T> &&
                                            is_container_iterator_v<decltype(std::declval<T &>().cend()), T> &&
                                            same_as_v<decltype(std::declval<T &>().size()), typename T::size_type>>;

template <typename T, typename = void>
struct is_container : std::false_type {};

template <typename T>
struct is_container<T, container<T>> : std::true_type {
    using value_type = typename T::value_type;
};

template <typename T>
constexpr bool is_container_v = is_container<T>::value;

template <typename T>
using contiguous_container = typename std::enable_if_t<is_container_v<T> &&
                                                       is_container_pointer_v<decltype(std::declval<T &>().data()), T>>;

template <typename T, typename = void>
struct is_contiguous_container : std::false_type {};

template <typename T>
struct is_contiguous_container<T, contiguous_container<T>> : std::true_type {};

template <typename T>
constexpr bool is_contiguous_container_v = is_contiguous_container<T>::value;

template <typename T>
using has_empty = typename std::enable_if_t<same_as_v<decltype(std::declval<T &>().empty()), bool>>;

template <typename T, typename = void>
struct has_empty_method : std::false_type {};

template <typename T>
struct has_empty_method<T, has_empty<T>> : std::true_type {};

template <typename T>
constexpr bool has_empty_method_v = has_empty_method<T>::value;

template <typename T>
constexpr bool has_empty_v = has_empty<T>::value;

template <typename T>
using resizable_container = typename std::void_t<
    std::enable_if_t<is_container_v<T>>,
    decltype(T(typename T::size_type{})),
    decltype(std::declval<T &>().resize(typename T::size_type{}))>;

template <typename T, typename = void>
struct is_resizable_container : std::false_type {};

template <typename T>
struct is_resizable_container<T, resizable_container<T>> : std::true_type {};

template <typename T>
constexpr bool is_resizable_container_v = is_resizable_container<T>::value;

template <typename T>
using resizable_byte_buffer = typename std::enable_if_t<is_contiguous_container_v<T> &&
                                                        is_resizable_container_v<T> &&
                                                        same_as_v<typename T::value_type, uint8_t>>;

template <typename T, typename = void>
struct is_resizable_byte_buffer : std::false_type {};

template <typename T>
struct is_resizable_byte_buffer<T, resizable_byte_buffer<T>> : std::true_type {};

template <typename T>
constexpr bool is_resizable_byte_buffer_v = is_resizable_byte_buffer<T>::value;

template <typename T>
using streamable = typename std::void_t<decltype(std::declval<std::ostream &>() << std::declval<T &>())>;

template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, streamable<T>> : std::true_type {};

template <typename T>
constexpr bool is_streamable_v = is_streamable<T>::value;

}

#endif
