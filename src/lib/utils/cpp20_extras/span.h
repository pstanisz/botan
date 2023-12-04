/*
* Implementation of C++20 std::span replacement for compilers not supporting C++20
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_SPAN_H_
#define BOTAN_SPAN_H_

#include <cstddef>
#include <limits>
#include <array>

#include <botan/type_traits.h>

namespace Botan {

// Non-member span constant
inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

// Forward
template <typename Type, std::size_t Extent = dynamic_extent>
class span;

namespace detail {

template <typename T, typename U>
struct is_span_convertible {
    static constexpr bool value = std::is_convertible_v<T(*)[], U(*)[]>;
};

template <typename T, typename U>
inline constexpr bool is_span_convertible_v = is_span_convertible<T, U>::value;

template <std::size_t E>
struct is_dynamic_extent {
    static constexpr bool value = (E == dynamic_extent);
};

template <std::size_t E>
inline constexpr bool is_dynamic_extent_v = is_dynamic_extent<E>::value;

template <std::size_t E, std::size_t N>
struct is_span_capacity {
    static constexpr bool value = (is_dynamic_extent_v<E> || E == N);
};

template <std::size_t E, std::size_t N>
inline constexpr bool is_span_capacity_v = is_span_capacity<E, N>::value;

template <typename T, std::size_t E, typename U, std::size_t N>
struct is_span_compatible {
    static constexpr bool value = is_span_capacity_v<E, N> && is_span_convertible_v<U, T>;
};

template <typename T, std::size_t E, typename U, std::size_t N>
inline constexpr bool is_span_compatible_v = is_span_compatible<T, E, U, N>::value;

template <std::size_t E, std::size_t N>
struct is_span_explicit {
    static constexpr bool value = (!is_dynamic_extent_v<E> && is_dynamic_extent_v<N>);
};

template <std::size_t E, std::size_t N>
inline constexpr bool is_span_explicit_v = is_span_explicit<E, N>::value;

template <typename T, std::size_t E, typename U, std::size_t N>
struct is_span_copyable {
    static constexpr bool value = (is_dynamic_extent_v<N> || is_span_capacity_v<E, N>) && is_span_convertible_v<U, T>;
};

template <typename T, std::size_t E, typename U, std::size_t N>
inline constexpr bool is_span_copyable_v = is_span_copyable<T, E, U, N>::value;

template <typename>
struct is_span : std::false_type {};

template <typename T, std::size_t E>
struct is_span<span<T, E>> : std::true_type {};

template <typename C>
inline constexpr bool is_span_v = is_span<C>::value;

template <typename>
struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename C>
inline constexpr bool is_std_array_v = is_std_array<C>::value;

template <typename Container>
constexpr auto size(const Container& c) -> decltype(c.size()) {
    return c.size();
}

template <typename T, std::size_t N>
constexpr std::size_t size(const T (&)[N]) noexcept {
    return N;
}

template <typename Container>
constexpr auto data(Container& c) -> decltype(c.data()) {
    return c.data();
}

template <typename Container>
constexpr auto data(const Container& c) -> decltype(c.data()) {
    return c.data();
}

template <typename T, std::size_t N>
constexpr T* data(T (&arr)[N]) noexcept {
    return arr;
}

template <typename, typename = void>
struct has_size : std::false_type {};

template <typename C>
struct has_size<C, std::void_t<decltype(Botan::detail::size(std::declval<C>()))>> : std::true_type {};

template <typename C>
inline constexpr bool has_size_v = has_size<C>::value;

template <typename, typename = void>
struct has_data : std::false_type {};

template <typename C>
struct has_data<C, std::void_t<decltype(Botan::detail::data(std::declval<C>()))>> : std::true_type {};

template <typename C>
inline constexpr bool has_data_v = has_data<C>::value;

template <typename C, typename U = Botan::remove_cvref_t<C>>
struct is_container {
    static constexpr bool value = !is_span_v<U> && !is_std_array_v<U> && !std::is_array_v<U> && 
        has_size_v<C> && has_data_v<C>;
};

template <typename C>
inline constexpr bool is_container_v = is_container<C>::value;

template <typename, typename, typename = void>
struct is_container_type_compatible : std::false_type {};

template <typename C, typename U>
struct is_container_type_compatible<C, U,
    typename std::enable_if_t<std::is_convertible_v<std::remove_pointer_t<decltype(Botan::detail::data(std::declval<C>()))> (*)[], U (*)[]>>> : std::true_type {};

template <typename C, typename U>
inline constexpr bool is_container_type_compatible_v = is_container_type_compatible<C, U>::value;

} // detail


// Span implementation according to https://en.cppreference.com/w/cpp/container/span
template <typename Type, std::size_t Extent>
class span {
public:
    // Member types
    using element_type = Type;
    using value_type = typename std::remove_cv_t<Type>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = Type*;
    using const_pointer = const Type*;
    using reference = Type&;
    using const_reference = const Type&;
    using iterator = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    // Member constant
    static constexpr std::size_t extent = Extent;

    // Constructors
    template <std::size_t N = Extent, 
        typename std::enable_if_t<N == 0 || detail::is_dynamic_extent_v<N>, bool> = true>
    constexpr span() noexcept : m_data(nullptr), m_size(0UL) {}

    template <typename Iter_first,
        typename std::enable_if_t<detail::is_dynamic_extent_v<Extent> && detail::is_span_convertible_v<Iter_first, Type>, bool> = true>
    constexpr span(Iter_first* first, size_type count) noexcept : m_data(first), m_size(count) {}

    template <typename Iter_first,
        typename std::enable_if_t<!detail::is_dynamic_extent_v<Extent> && detail::is_span_convertible_v<Iter_first, Type>, bool> = true>
    explicit constexpr span(Iter_first* first, size_type count) noexcept : m_data(first), m_size(count) {}

    template <typename Iter_first, typename Iter_last,
        typename std::enable_if_t<detail::is_dynamic_extent_v<Extent> &&
            detail::is_span_convertible_v<Iter_first, Type>, bool> = true>
    constexpr span(Iter_first* first, Iter_last* last) noexcept : m_data(first), m_size(last - first) {}

    template <typename Iter_first, typename Iter_last,
        typename std::enable_if_t<!detail::is_dynamic_extent_v<Extent> &&
            detail::is_span_convertible_v<Iter_first, Type>, bool> = true>
    explicit constexpr span(Iter_first* first, Iter_last* last) noexcept : m_data(first), m_size(last - first) {}
    
    template <std::size_t N,
        typename std::enable_if_t<detail::is_span_capacity_v<Extent, N>, bool> = true>
    constexpr span(typename std::enable_if_t<true, Type> (&arr)[N]) noexcept : m_data(arr), m_size(N) {}

    template <typename U, std::size_t N,
        typename std::enable_if_t<detail::is_span_compatible_v<Type, Extent, U, N>, bool> = true>
    constexpr span(std::array<U, N>& arr) noexcept : m_data(arr.data()), m_size(N) {}

    template <typename U, std::size_t N,
        typename std::enable_if_t<detail::is_span_compatible_v<Type, Extent, const U, N>, bool> = true>
    constexpr span(const std::array<U, N>& arr) noexcept : m_data(arr.data()), m_size(N) {}

    // Instead of ranges
    template <typename Container, std::size_t E = Extent,
        typename std::enable_if_t<detail::is_dynamic_extent_v<E> && detail::is_container_v<Container> &&
                detail::is_container_type_compatible_v<Container&, Type>, bool> = true>
    constexpr span(Container& cont)
        :  m_data(Botan::detail::data(cont)), m_size(Botan::detail::size(cont)) {}

    template <typename Container, std::size_t E = Extent,
        typename std::enable_if_t<detail::is_dynamic_extent_v<E> && detail::is_container_v<Container> &&
                detail::is_container_type_compatible_v<Container&, Type>, bool> = true>
    constexpr span(const Container& cont)
        :  m_data(Botan::detail::data(cont)), m_size(Botan::detail::size(cont)) {}

    constexpr span(const span& other) noexcept = default;

    template <typename U, std::size_t N,
        typename std::enable_if_t<!detail::is_span_explicit_v<Extent, N> &&
            detail::is_span_copyable_v<Type, Extent, U, N>, bool> = true>
    constexpr span(const span<U, N>& other) noexcept : m_data(other.data()), m_size(other.size()) {}

    template <typename U, std::size_t N,
        typename std::enable_if_t<detail::is_span_explicit_v<Extent, N> &&
            detail::is_span_copyable_v<Type, Extent, U, N>, bool> = true>
    explicit constexpr span(const span<U, N>& other) noexcept : m_data(other.data()), m_size(other.size()) {}

    constexpr span& operator=(const span& other) noexcept = default;

    // Iterators
    constexpr iterator begin() const noexcept { return data(); }
    constexpr iterator end() const noexcept { return data() + size(); }
    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    // Element access
    constexpr reference front() const { return *data(); }
    constexpr reference back() const { return *(data() + (size() - 1)); }
    constexpr reference operator[](size_type idx) const { return *(data() + idx); }
    constexpr pointer data() const noexcept { return m_data; }
    
    // Observers
    constexpr std::size_t size() const noexcept { return m_size; }
    constexpr size_type size_bytes() const noexcept { return size() * sizeof(Type); }
    constexpr bool empty() const noexcept { return (size() == 0); }
    
    // Subviews
    template <std::size_t Count>
    constexpr span<Type, Count> first() const { 
        static_assert(Count <= Extent, "Count <= Extent");
        return span<Type, Count>(data(), Count);
    }

    constexpr span<Type, dynamic_extent> first(size_type count) const { return span<Type, dynamic_extent>(data(), count); }

    template <std::size_t Count>
    constexpr span<Type, Count> last() const {
        static_assert(Count <= Extent, "Count <= Extent");
        return span<Type, Count>(data() + (size() - Count), Count); }
    
    constexpr span<Type, dynamic_extent> last(size_type count) const { return span<Type, dynamic_extent>(data() + (size() - count), count); }

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr typename std::enable_if_t<Count != dynamic_extent, span<Type, Count>> subspan() const {
        static_assert(Offset <= Extent && Count <= Extent - Offset, "Offset <= Extent && Count <= Extent - Offset");
        return span<Type, Count>(data() + Offset, Count);
    }
    
    constexpr span<Type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const {
        return span<Type, dynamic_extent>(data() + offset, (count == dynamic_extent ? size() - offset : count));
    }

private:
    element_type*   m_data;
    size_type       m_size;
};

// Deduction guidelines as described in https://en.cppreference.com/w/cpp/container/span/deduction_guides
template <typename Iter_first, typename End_or_size >
span(Iter_first*, End_or_size) -> span<std::remove_reference_t<Iter_first>>;

template <typename Type, std::size_t N>
span(Type (&)[N] ) -> span<Type, N>;

template <typename Type, std::size_t N>
span(std::array<Type, N>&) -> span<Type, N>;

template <typename Type, std::size_t N>
span(const std::array<Type, N>&) -> span<const Type, N>;

// Instead of ranges
template <typename Container>
span(Container&) -> span<typename std::remove_reference_t<decltype(*Botan::detail::data(std::declval<Container&>()))>>;

template <typename Container>
span(const Container&) -> span<const typename Container::value_type>;

// Non-member span functions
template <typename Type, std::size_t Extent>
span<const std::byte, (detail::is_dynamic_extent_v<Extent> ? dynamic_extent : sizeof(Type) * Extent)> as_bytes(span<Type, Extent> s) noexcept {
    return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}

template <typename Type, size_t Extent, typename std::enable_if_t<!std::is_const_v<Type>, bool> = true>
span<std::byte, (detail::is_dynamic_extent_v<Extent> ? dynamic_extent : sizeof(Type) * Extent)> as_writable_bytes(span<Type, Extent> s) noexcept {
    return {reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

}

#endif
