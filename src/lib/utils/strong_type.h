/**
 * A wrapper class to implement strong types
 * (C) 2022 Jack Lloyd
 *     2022 René Meusel - Rohde & Schwarz Cybersecurity
 *     2023 Piotr Staniszewski - CodeVision
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#ifndef BOTAN_STRONG_TYPE_H_
#define BOTAN_STRONG_TYPE_H_

#include <ostream>
#include <botan/span.h>
#include <botan/concepts.h>

namespace Botan {

/**
 * Added as an additional "capability tag" to enable arithmetic operators with
 * plain numbers for Strong<> types that wrap a number.
 */
struct EnableArithmeticWithPlainNumber {};

namespace detail {

/**
 * Checks whether the @p CapabilityT is included in the @p Tags type pack.
 */
template <typename CapabilityT, typename... Tags>
constexpr bool has_capability = (std::is_same_v<CapabilityT, Tags> || ...);

template <typename T>
class Strong_Base {
   private:
      T m_value;

   public:
      using wrapped_type = T;

   public:
      Strong_Base() = default;
      Strong_Base(const Strong_Base&) = default;
      Strong_Base(Strong_Base&&) noexcept = default;
      Strong_Base& operator=(const Strong_Base&) = default;
      Strong_Base& operator=(Strong_Base&&) noexcept = default;

      constexpr explicit Strong_Base(T v) : m_value(std::move(v)) {}

      T& get() { return m_value; }

      const T& get() const { return m_value; }
};

template <typename T, typename = void>
class Strong_Adapter : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <typename T>
class Strong_Adapter<T, std::enable_if_t<concepts::is_integral_v<T>>> : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <typename T>
class Strong_Adapter<T, std::enable_if_t<concepts::is_container_v<T>>> : public Strong_Base<T> {
   public:
      using value_type = typename T::value_type;
      using size_type = typename T::size_type;
      using iterator = typename T::iterator;
      using const_iterator = typename T::const_iterator;
      using pointer = typename T::pointer;
      using const_pointer = typename T::const_pointer;

   public:
      using Strong_Base<T>::Strong_Base;

      template <typename U = T, typename = std::enable_if_t<concepts::is_contiguous_container_v<U>>>
      explicit Strong_Adapter(Botan::span<const value_type> span)
         : Strong_Adapter(T(span.begin(), span.end())) {}

      template <typename U = T, typename = std::enable_if_t<concepts::is_resizable_container_v<U>>>
      explicit Strong_Adapter(size_t size)
         : Strong_Adapter(T(size)) {}

      template <typename InputIt>
      Strong_Adapter(InputIt begin, InputIt end) : Strong_Adapter(T(begin, end)) {}

      // Disambiguates the usage of string literals, otherwise:
      // Strong_Adapter(Botan::span<>) and Strong_Adapter(const char*)
      // would be ambiguous.
      template <typename U = T, typename = std::enable_if_t<concepts::same_as_v<U, std::string>>>
      explicit Strong_Adapter(const char* str)
         : Strong_Adapter(std::string(str)) {}

   public:
      decltype(auto) begin() noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) end() noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) end() const noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) cbegin() noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cbegin() const noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cend() noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      decltype(auto) cend() const noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      size_type size() const noexcept(noexcept(this->get().size())) { return this->get().size(); }

      template <typename U = T, typename = std::enable_if_t<concepts::is_contiguous_container_v<U>>>
      decltype(auto) data() noexcept(noexcept(this->get().data()))
         { return this->get().data(); }

      template <typename U = T, typename = std::enable_if_t<concepts::is_contiguous_container_v<U>>>
      decltype(auto) data() const noexcept(noexcept(this->get().data()))
         { return this->get().data(); }

      template <typename U = T, typename = std::enable_if_t<concepts::has_empty_method_v<U>>>
      bool empty() const noexcept(noexcept(this->get().empty()))
         { return this->get().empty(); }

      template <typename U = T, typename = std::enable_if_t<concepts::is_resizable_container_v<U>>>
      void resize(size_type size) noexcept(noexcept(this->get().resize(size)))
         { this->get().resize(size); }
   };

}  // namespace detail

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
template <typename T, typename TagTypeT, typename... Capabilities>
class Strong : public detail::Strong_Adapter<T> {
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

template<typename T, typename... Tags, typename = concepts::equality_comparable<T>>
bool operator!=(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return !operator==(lhs, rhs); }

template<typename T, typename... Tags, typename = concepts::less_comparable<T>>
auto operator<(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() < rhs.get(); }

template<typename T, typename... Tags, typename = concepts::greater_comparable<T>>
auto operator>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() > rhs.get(); }

template<typename T, typename... Tags, typename = concepts::less_comparable<T>>
auto operator<=(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() <= rhs.get(); }

template<typename T, typename... Tags, typename = concepts::greater_comparable<T>>
auto operator>=(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs)
   { return lhs.get() >= rhs.get(); }

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator<(T1 a, Strong<T2, Tags...> b) {
   return a < b.get();
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator<=(Strong<T1, Tags...> a, T2 b) {
   return a.get() <= b;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator<=(T1 a, Strong<T2, Tags...> b) {
   return a <= b.get();
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator>=(Strong<T1, Tags...> a, T2 b) {
   return a.get() >= b;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator>=(T1 a, Strong<T2, Tags...> b) {
   return a >= b.get();
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator<(Strong<T1, Tags...> a, T2 b) {
   return a.get() < b;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator>(T1 a, Strong<T2, Tags...> b) {
   return a > b.get();
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator>(Strong<T1, Tags...> a, T2 b) {
   return a.get() > b;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator==(T1 a, Strong<T2, Tags...> b) {
   return a == b.get();
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator==(Strong<T1, Tags...> a, T2 b) {
   return a.get() == b;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator!=(T1 a, Strong<T2, Tags...> b) {
   return !operator==(a, b);
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2>>>
auto operator!=(Strong<T1, Tags...> a, T2 b) {
   return !operator==(a, b);
}

template <typename T1, typename T2, typename... Tags, 
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator+(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a + b.get());
}

template <typename T1, typename T2, typename... Tags, typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator+(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() + b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator+(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() + b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator-(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a - b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator-(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() - b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator-(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() - b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator*(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a * b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator*(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() * b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator*(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() * b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator/(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a / b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator/(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() / b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator/(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() / b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator^(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a ^ b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator^(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() ^ b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator^(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() ^ b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator&(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a & b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator&(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() & b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator&(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() & b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator|(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a | b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator|(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() | b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator|(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() | b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator>>(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a >> b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator>>(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() >> b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator>>(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() >> b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator<<(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a << b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr decltype(auto) operator<<(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() << b);
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr decltype(auto) operator<<(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() << b.get());
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator+=(Strong<T1, Tags...>& a, T2 b) {
   a.get() += b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator+=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() += b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator-=(Strong<T1, Tags...>& a, T2 b) {
   a.get() -= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator-=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() -= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator*=(Strong<T1, Tags...>& a, T2 b) {
   a.get() *= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator*=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() *= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator/=(Strong<T1, Tags...>& a, T2 b) {
   a.get() /= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator/=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() /= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator^=(Strong<T1, Tags...>& a, T2 b) {
   a.get() ^= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator^=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() ^= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator&=(Strong<T1, Tags...>& a, T2 b) {
   a.get() &= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator&=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() &= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator|=(Strong<T1, Tags...>& a, T2 b) {
   a.get() |= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator|=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() |= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator>>=(Strong<T1, Tags...>& a, T2 b) {
   a.get() >>= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator>>=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() >>= b.get();
   return a;
}

template <typename T1, typename T2, typename... Tags,
   typename = std::enable_if_t<concepts::is_both_integral_v<T1, T2> && detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>>>
constexpr auto operator<<=(Strong<T1, Tags...>& a, T2 b) {
   a.get() <<= b;
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator<<=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() <<= b.get();
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator++(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   ++a.get();
   return tmp;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator++(Strong<T, Tags...>& a) {
   ++a.get();
   return a;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator--(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   --a.get();
   return tmp;
}

template <typename T, typename... Tags, typename = std::enable_if_t<concepts::is_integral_v<T>>>
constexpr auto operator--(Strong<T, Tags...>& a) {
   --a.get();
   return a;
}

/**
 * This mimmicks a Botan::span but keeps track of the strong-type information. Use
 * this when you would want to use `const Strong<...>&` as a parameter
 * declaration. In particular this allows assigning strong-type information to
 * slices of a bigger buffer without copying the bytes. E.g:
 *
 *    using Foo = Strong<std::vector<uint8_t>, Foo_>;
 *
 *    void bar(StrongSpan<Foo> foo) { ... }
 *
 *    std::vector<uint8_t> buffer;
 *    BufferSlicer slicer(buffer);
 *    bar(slicer.take<Foo>());  // This does not copy the data from buffer but
 *                              // just annotates the 'Foo' strong-type info.
 */
template <typename T, typename = concepts::contiguous_strong_type<T>>
class StrongSpan {
      using underlying_span = std::
         conditional_t<std::is_const_v<T>, Botan::span<const typename T::value_type>, Botan::span<typename T::value_type>>;

   public:
      using value_type = typename underlying_span::value_type;
      using size_type = typename underlying_span::size_type;
      using iterator = typename underlying_span::iterator;
      using pointer = typename underlying_span::pointer;
      using const_pointer = typename underlying_span::const_pointer;

      StrongSpan() = default;

      explicit StrongSpan(underlying_span span) : m_span(span) {}

      StrongSpan(T& strong) : m_span(strong) {}

      // Allows implicit conversion from `StrongSpan<T>` to `StrongSpan<const T>`.
      // Note that this is not bi-directional. Conversion from `StrongSpan<const T>`
      // to `StrongSpan<T>` is not allowed.
      //
      // TODO: Technically, we should be able to phrase this with a `requires std::is_const_v<T>`
      //       instead of the `std::enable_if` constructions. clang-tidy (14 or 15) doesn't seem
      //       to pick up on that (yet?). As a result, for a non-const T it assumes this to be
      //       a declaration of an ordinary copy constructor. The existance of a copy constructor
      //       is interpreted as "not cheap to copy", setting off the `performance-unnecessary-value-param` check.
      //       See also: https://github.com/randombit/botan/issues/3591
      template <typename T2,
                typename = std::enable_if_t<concepts::is_contiguous_strong_type_v<T2> && std::is_same_v<T2, std::remove_const_t<T>>>>
      StrongSpan(const StrongSpan<T2>& other) : m_span(other.get()) {}

      StrongSpan(const StrongSpan& other) = default;

      ~StrongSpan() = default;

      /**
       * @returns the underlying Botan::span without any type constraints
       */
      underlying_span get() const { return m_span; }

      /**
       * @returns the underlying Botan::span without any type constraints
       */
      underlying_span get() { return m_span; }

      decltype(auto) data() noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) data() const noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) size() const noexcept(noexcept(this->m_span.size())) { return this->m_span.size(); }

      bool empty() const noexcept(noexcept(this->m_span.empty())) { return this->m_span.empty(); }

      decltype(auto) begin() noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) end() noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

      decltype(auto) end() const noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

   private:
      underlying_span m_span;
};

}  // namespace Botan

#endif
