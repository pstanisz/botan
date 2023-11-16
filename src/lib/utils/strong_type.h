/**
 * A wrapper class to implement strong types
 * (C) 2022 Jack Lloyd
 *     2022 René Meusel - Rohde & Schwarz Cybersecurity
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#ifndef BOTAN_STRONG_TYPE_H_
#define BOTAN_STRONG_TYPE_H_

#include <ostream>
#include <span>

#include <botan/concepts.h>

namespace Botan
{

namespace detail
{

template <typename T>
class Strong_Base
   {
   private:
      T m_value;

   public:
      Strong_Base() = default;
      Strong_Base(const Strong_Base&) = default;
      Strong_Base(Strong_Base&&) = default;
      Strong_Base& operator=(const Strong_Base&) = default;
      Strong_Base& operator=(Strong_Base&&) = default;

      explicit Strong_Base(T v) : m_value(std::move(v)) {}

      T& get() { return m_value; }
      const T& get() const { return m_value; }
   };

template <typename T, typename = void>
class Strong_Adapter : public Strong_Base<T>
   {
   public:
      using Strong_Base<T>::Strong_Base;
   };

template <typename T>
class Strong_Adapter<T, std::enable_if_t<concepts::is_container_v<T>>> : public Strong_Base<T>
   {
   public:
      using value_type = typename T::value_type;
      using size_type = typename T::size_type;
      using iterator = typename T::iterator;
      using const_iterator = typename T::const_iterator;
      using pointer = typename T::pointer;
      using const_pointer = typename T::const_pointer;

   public:
      using Strong_Base<T>::Strong_Base;

      explicit Strong_Adapter(std::span<const value_type> span)
         : Strong_Adapter(T(span.begin(), span.end())) {
            static_assert(concepts::is_contiguous_container_v<T>, "contiguous_container required");
         }

      explicit Strong_Adapter(size_t size)
         : Strong_Adapter(T(size)) {
            static_assert(concepts::is_resizable_container_v<T>, "resizable_container required");
         }

      // Disambiguates the usage of string literals, otherwise:
      // Strong_Adapter(std::span<>) and Strong_Adapter(const char*)
      // would be ambiguous.
      explicit Strong_Adapter(const char* str)
         : Strong_Adapter(std::string(str)) {
            static_assert(concepts::same_as_v<T, std::string>, "type same as std::string required");
         }

   public:
      decltype(auto) begin() noexcept(noexcept(this->get().begin()))
         { return this->get().begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->get().begin()))
         { return this->get().begin(); }

      decltype(auto) end() noexcept(noexcept(this->get().end()))
         { return this->get().end(); }

      decltype(auto) end() const noexcept(noexcept(this->get().end()))
         { return this->get().end(); }

      decltype(auto) cbegin() noexcept(noexcept(this->get().cbegin()))
         { return this->get().cbegin(); }

      decltype(auto) cbegin() const noexcept(noexcept(this->get().cbegin()))
         { return this->get().cbegin(); }

      decltype(auto) cend() noexcept(noexcept(this->get().cend()))
         { return this->get().cend(); }

      decltype(auto) cend() const noexcept(noexcept(this->get().cend()))
         { return this->get().cend(); }

      size_type size() const noexcept(noexcept(this->get().size()))
         { return this->get().size(); }

      decltype(auto) data() noexcept(noexcept(this->get().data()))
         { 
            static_assert(concepts::is_contiguous_container_v<T>, "contiguous_container required");
            return this->get().data();
         }

      decltype(auto) data() const noexcept(noexcept(this->get().data()))
         { 
            static_assert(concepts::is_contiguous_container_v<T>, "contiguous_container required");
            return this->get().data();
         }

      bool empty() const noexcept(noexcept(this->get().empty()))
         { 
             static_assert(concepts::has_empty_method_v<T>, "has_empty required");
             return this->get().empty();
         }

      void resize(size_type size) noexcept(noexcept(this->get().resize(size)))
         { 
            static_assert(concepts::is_resizable_container_v<T>, "resizable_container required");
            this->get().resize(size);
         }
   };

}

/**
 * Strong types can be used as wrappers around common types to provide
 * compile time semantics. They usually contribute to more maintainable and
 * less error-prone code especially when dealing with function parameters.
 *
 * Internally, this provides adapters so that the wrapping strong type behaves
 * as much as the underlying type as possible and desirable.
 *
 * This implementation was inspired by:
 *   https://stackoverflow.com/a/69030899
 */
template<typename T, typename TagTypeT>
class Strong : public detail::Strong_Adapter<T>
   {
   public:
      using detail::Strong_Adapter<T>::Strong_Adapter;

   private:
      using Tag = TagTypeT;
   };

template<typename T, typename... Tags, typename = concepts::streamable<T>>
decltype(auto) operator<<(std::ostream& os, const Strong<T, Tags...>& v)
   { 
      return os << v.get();
   }

template<typename T, typename... Tags, typename = concepts::equality_comparable<T>>
bool operator==(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { 
      return lhs.get() == rhs.get();
   }

// template<typename T, typename... Tags>
// requires(concepts::three_way_comparable<T>)
// auto operator<=>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
//    { return lhs.get() <=> rhs.get(); }

template<typename T, typename... Tags, typename = concepts::less_comparable<T>>
auto operator<(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() < rhs.get(); }

template<typename T, typename... Tags, typename = concepts::greater_comparable<T>>
auto operator>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() > rhs.get(); }

}

#endif
