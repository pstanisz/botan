/*
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include "tests.h"

#include <botan/span.h>

#include <cstddef>
#include <limits>
#include <array>
#include <vector>

namespace Botan_Tests {

namespace {

constexpr size_t SIZE = 5UL;
int C_ARRAY[SIZE] = {1, 2, 3, 4, 5};
std::array<int, SIZE> ARRAY = {1, 2, 3, 4, 5};
const std::array<int, SIZE> CONST_ARRAY = {1, 2, 3, 4, 5};
std::vector<int> VECTOR = {1, 2, 3, 4, 5};
const std::vector<int> CONST_VECTOR = {1, 2, 3, 4, 5};
const std::string CONST_STRING = "abcdef";

template <typename T>
struct Dummy {};

template <typename T>
struct With_size {
   std::size_t size() const noexcept { return 1; }
};

template <typename T>
struct With_data {
   T* data() const noexcept { return &value; }

   T value;
};

template <typename T>
struct With_size_and_data : public With_size<T>, With_data<T> {};

std::vector<Test::Result> test_ctors()
   {
   return
      {
      Botan_Tests::CHECK("default ctor", [](auto& result)
         {
         Botan::span<int> s;
         
         result.test_is_eq("size()", s.size(), size_t(0U));
         result.test_is_eq("data()", s.data(), static_cast<int*>(nullptr));
         }),
      Botan_Tests::CHECK("ctor with first and size", [](auto& result)
         {
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         Botan::span<int> s(&(*std::begin(ARRAY)), ARRAY.size());
#else
         Botan::span<int> s(std::begin(ARRAY), ARRAY.size());
#endif

         result.test_is_eq("size()", s.size(), ARRAY.size());
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         result.test_is_eq("data()", s.data(), &(*ARRAY.begin()));
#else
         result.test_is_eq("data()", s.data(), ARRAY.begin());
#endif
         }),
      Botan_Tests::CHECK("ctor with first and last", [](auto& result)
         {
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         Botan::span<int> s(&(*std::begin(ARRAY)), &(*std::end(ARRAY)));
#else
         Botan::span<int> s(std::begin(ARRAY), std::end(ARRAY));
#endif

         result.test_is_eq("size()", s.size(), ARRAY.size());
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         result.test_is_eq("data()", s.data(), &(*ARRAY.begin()));
#else
         result.test_is_eq("data()", s.data(), ARRAY.begin());
#endif
         }),
      Botan_Tests::CHECK("ctor with C array", [](auto& result)
         {
         Botan::span<int> s(C_ARRAY);

         result.test_is_eq("size()", s.size(), size_t(SIZE));
         result.test_is_eq("data()", s.data(), &C_ARRAY[0]);
         }),
      Botan_Tests::CHECK("ctor with std::array", [](auto& result)
         {
         Botan::span<int> s(ARRAY);

         result.test_is_eq("size()", s.size(), ARRAY.size());
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         result.test_is_eq("data()", s.data(), &(*ARRAY.begin()));
#else
         result.test_is_eq("data()", s.data(), ARRAY.begin());
#endif
         }),
      Botan_Tests::CHECK("ctor with const std::array", [](auto& result)
         {
         const Botan::span<const int> s(CONST_ARRAY);

         result.test_is_eq("size()", s.size(), CONST_ARRAY.size());
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
         result.test_is_eq("data()", s.data(), &(*CONST_ARRAY.begin()));
#else
         result.test_is_eq("data()", s.data(), CONST_ARRAY.begin());
#endif
         }),
      Botan_Tests::CHECK("ctor with std::vector", [](auto& result)
         {
         Botan::span<int> s(VECTOR);

         result.test_is_eq("size()", s.size(), VECTOR.size());
         }),
      Botan_Tests::CHECK("ctor with const std::vector", [](auto& result)
         {
         const Botan::span<const int> s(CONST_VECTOR);

         result.test_is_eq("size()", s.size(), CONST_VECTOR.size());
         }),
      Botan_Tests::CHECK("ctor with span", [](auto& result)
         {
         const Botan::span<int> s(ARRAY);
         Botan::span<int> s2(s);

         result.test_is_eq("size()", s2.size(), s.size());
         result.test_is_eq("data()", s2.data(), s.data());
         })
      };
   }

std::vector<Test::Result> test_assignment_operator()
   {
   return
      {
      Botan_Tests::CHECK("operator= from empty", [](auto& result)
         {
        Botan::span<int> s;
        Botan::span<int> s1;

         s = s1;
         result.test_is_eq("size()", s.size(), s1.size());
         result.test_is_eq("data()", s.data(), s1.data());
         }),
      Botan_Tests::CHECK("operator= from non-empty", [](auto& result)
         {
        Botan::span<int> s;
        Botan::span<int> s1(ARRAY);

         s = s1;
         result.test_is_eq("size()", s.size(), s1.size());
         result.test_is_eq("data()", s.data(), s1.data());
         })
      };
   }

std::vector<Test::Result> test_iterators()
   {
   return
      {
      Botan_Tests::CHECK("begin() and end() with empty", [](auto& result)
         {
        Botan::span<int> s;

         result.test_is_eq("begin() == end()", s.begin(), s.end());
         }),
      Botan_Tests::CHECK("begin() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("begin() points to first", &(*s.begin()), &ARRAY[0]);
         }),
      Botan_Tests::CHECK("end() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("begin() to end() distance", size_t(std::distance(s.begin(), s.end())), SIZE);
         result.test_is_eq("end() points to one after last", &(*(s.end() - 1)), &ARRAY[SIZE-1]);
         }),
      Botan_Tests::CHECK("rbegin() and rend() with empty", [](auto& result)
         {
        Botan::span<int> s;

         result.test_is_eq("rbegin() == rend()", s.rbegin(), s.rend());
         }),
      Botan_Tests::CHECK("rbegin() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("rbegin() points to last", &(*s.rbegin()), &ARRAY[SIZE-1]);
         }),
      Botan_Tests::CHECK("rend() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("rbegin() to rend() distance", size_t(std::distance(s.rbegin(), s.rend())), SIZE);
         result.test_is_eq("rend() points to one before first", &(*(s.rend() - 1)), &ARRAY[0]);
         })
      };
   }

std::vector<Test::Result> test_element_access()
   {
   return
      {
      Botan_Tests::CHECK("front()", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("front() gives first", s.front(), ARRAY[0]);
         }),
      Botan_Tests::CHECK("back()", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("back() gives last", s.back(), ARRAY[SIZE-1]);
         }),
      Botan_Tests::CHECK("operator[]", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         
         for (decltype(ARRAY.size()) i = 0; i < ARRAY.size(); ++i)
            {
            result.test_is_eq("operator[] gives reference to element", s[i], ARRAY[i]);
            }
         }),
      Botan_Tests::CHECK("data()", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         
         result.test_is_eq("data() gives pointer to beginning", s.data(), &ARRAY[0]);
         })
      };
   }

std::vector<Test::Result> test_observers()
   {
   return
      {
      Botan_Tests::CHECK("size()", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("size() gives span length", s.size(), ARRAY.size());
         }),
      Botan_Tests::CHECK("size_bytes()", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("size_bytes() gives span size in bytes", s.size_bytes(), sizeof(ARRAY));
         }),
      Botan_Tests::CHECK("empty() with empty", [](auto& result)
         {
        Botan::span<int> s;

         result.confirm("empty() is true", s.empty(), true);
         }),
      Botan_Tests::CHECK("empty() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.confirm("empty() is false", s.empty(), false);
         })
      };
   }

std::vector<Test::Result> test_subviews()
   {
   return
      {
      Botan_Tests::CHECK("first(0) from empty", [](auto& result)
         {
        Botan::span<int> s;
         auto sub = s.first(0UL);

         result.confirm("first(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("first(0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.first(0UL);

         result.confirm("first(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("first(1) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.first(1UL);

         result.test_is_eq("first(1) gives one element span", sub.size(), size_t(1UL));
         result.test_is_eq("first(1) gives first element", &(*sub.begin()), &ARRAY[0]);
         }),
      Botan_Tests::CHECK("first(SIZE) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.first(SIZE);

         result.test_is_eq("first(SIZE) gives SIZE element span", sub.size(), size_t(SIZE));
         }),
      Botan_Tests::CHECK("last(0) from empty", [](auto& result)
         {
        Botan::span<int> s;
         auto sub = s.last(0UL);

         result.confirm("last(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("last(0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.last(0UL);

         result.confirm("last(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("last(1) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.last(1UL);

         result.test_is_eq("last(1) gives one element span", sub.size(), size_t(1UL));
         result.test_is_eq("last(1) gives last element", &(*(sub.end() - 1)), &ARRAY[SIZE - 1]);
         }),
      Botan_Tests::CHECK("last(SIZE) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.last(SIZE);

         result.test_is_eq("last(SIZE) gives SIZE element span", sub.size(), size_t(SIZE));
         }),
      Botan_Tests::CHECK("subspan(0, 0) from empty", [](auto& result)
         {
        Botan::span<int> s;
         auto sub = s.subspan(0UL, 0UL);

         result.confirm("subspan(0, 0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("subspan(0, 0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(0UL, 0UL);

         result.confirm("subspan(0, 0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("subspan(1, 0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(1UL, 0UL);

         result.confirm("subspan(1, 0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("subspan(1, 1) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(1UL, 1UL);

         result.test_is_eq("subspan(1, 1) gives one element span", sub.size(), size_t(1UL));
         result.test_is_eq("subspan(1, 1) starts from second element", &(*sub.begin()), &ARRAY[1]);
         }),
      Botan_Tests::CHECK("subspan(1, 4) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(1UL, 4UL);

         result.test_is_eq("subspan(1, 4) gives 4 elements span", sub.size(), size_t(SIZE - 1UL));
         result.test_is_eq("subspan(1, 4) starts from second element", &(*sub.begin()), &ARRAY[1]);
         result.test_is_eq("subspan(1, 4) ends with last element", &(*(sub.end() - 1)), &ARRAY[SIZE - 1UL]);
         })
      };
   }

std::vector<Test::Result> test_non_member()
   {
   return
      {
      Botan_Tests::CHECK("as_bytes() from empty", [](auto& result)
         {
         Botan::span<int> s;
         auto const const_bytes = Botan::as_bytes(s);

         result.confirm("as_bytes() gives empty span", const_bytes.empty(), true);
         }),
      Botan_Tests::CHECK("as_writable_bytes() from empty", [](auto& result)
         {
        Botan::span<int> s;
         auto bytes = Botan::as_bytes(s);

         result.confirm("as_writable_bytes() gives empty span", bytes.empty(), true);
         }),
      Botan_Tests::CHECK("as_bytes() from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto const const_bytes = Botan::as_bytes(s);

         result.test_is_eq("as_bytes() gives sizeof(ARRAY) elements const span", const_bytes.size(), size_t(sizeof(ARRAY)));
         }),
      Botan_Tests::CHECK("as_writable_bytes() from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto bytes = Botan::as_writable_bytes(s);

         result.test_is_eq("as_writable_bytes() gives sizeof(ARRAY) elements non-const span", bytes.size(), size_t(sizeof(ARRAY)));
         })
      };
   }

std::vector<Test::Result> test_misc()
   {
   return
      {
      Botan_Tests::CHECK("implicit from std::vector", [](auto& result)
         {
         std::vector<int> vec {1, 2, 3};
         Botan::span s(vec);

         result.test_is_eq("span from std::vector", s.size(), vec.size());
         }), 
      Botan_Tests::CHECK("implicit from secure_vector", [](auto& result)
         {
         Botan::secure_vector<uint8_t> vec {1, 2, 3};
         Botan::span s(vec);

         result.test_is_eq("span from secure_vector", s.size(), vec.size());
         }),
      Botan_Tests::CHECK("implicit from std::string", [](auto& result)
         {
         std::string str = "abc";
         Botan::span s(str);

         result.test_is_eq("span from std::string", s.size(), str.size());
         })
      };
   }

std::vector<Test::Result> test_span_detail()
   {
   return
      {
      Botan_Tests::CHECK("is_span_convertible_v", [](auto& result)
         {
         result.confirm("is_span_convertible_v<int, int>", Botan::detail::is_span_convertible_v<int, int>, true);
         result.confirm("is_span_convertible_v<int, const int>", Botan::detail::is_span_convertible_v<int, const int>, true);
         result.confirm("is_span_convertible_v<int, char>", Botan::detail::is_span_convertible_v<int, char>, false);
         result.confirm("is_span_convertible_v<int, unsigned int>", Botan::detail::is_span_convertible_v<int, unsigned int>, false);
         result.confirm("is_span_convertible_v<int*, void*>", Botan::detail::is_span_convertible_v<int*, void*>, false);
         }),
      Botan_Tests::CHECK("is_dynamic_extent_v", [](auto& result)
         {
         result.confirm("is_dynamic_extent_v<0>", Botan::detail::is_dynamic_extent_v<0>, false);
         result.confirm("is_dynamic_extent_v<100UL>", Botan::detail::is_dynamic_extent_v<100UL>, false);
         result.confirm("is_dynamic_extent_v<Botan::dynamic_extent>", Botan::detail::is_dynamic_extent_v<Botan::dynamic_extent>, true);
         }),
      Botan_Tests::CHECK("is_span_capacity_v", [](auto& result)
         {
         result.confirm("is_span_capacity_v<0, 0>", Botan::detail::is_span_capacity_v<0, 0>, true);
         result.confirm("is_span_capacity_v<10, 10>", Botan::detail::is_span_capacity_v<10, 10>, true);
         result.confirm("is_span_capacity_v<1, 0>", Botan::detail::is_span_capacity_v<1, 0>, false);
         result.confirm("is_span_capacity_v<10, 11>", Botan::detail::is_span_capacity_v<10, 11>, false);
         result.confirm("is_span_capacity_v<10UL, 10>", Botan::detail::is_span_capacity_v<10UL, 10>, true);
         result.confirm("is_span_capacity_v<dynamic_extent, 10>", Botan::detail::is_span_capacity_v<Botan::dynamic_extent, 10>, true);
         }),
      Botan_Tests::CHECK("is_span_compatible_v", [](auto& result)
         {
         result.confirm("is_span_compatible_v<int, 5, int, 5>", Botan::detail::is_span_compatible_v<int, 5, int, 5>, true);
         result.confirm("is_span_compatible_v<int, 5, int, 10>", Botan::detail::is_span_compatible_v<int, 5, int, 10>, false);
         result.confirm("is_span_compatible_v<int, 5, const int, 5>", Botan::detail::is_span_compatible_v<int, 5, const int, 5>, false);
         result.confirm("is_span_compatible_v<int, dynamic_extent, int, 5>", Botan::detail::is_span_compatible_v<int, Botan::dynamic_extent, int, 5>, true);
         result.confirm("is_span_compatible_v<int, dynamic_extent, long, 5>", Botan::detail::is_span_compatible_v<int, Botan::dynamic_extent, long, 5>, false);
         }),
      Botan_Tests::CHECK("is_span_explicit_v", [](auto& result)
         {
         result.confirm("is_span_explicit_v<0, 0>", Botan::detail::is_span_explicit_v<0, 0>, false);
         result.confirm("is_span_explicit_v<10, 10>", Botan::detail::is_span_explicit_v<10, 10>, false);
         result.confirm("is_span_explicit_v<dynamic_extent, 10>", Botan::detail::is_span_explicit_v<Botan::dynamic_extent, 10>, false);
         result.confirm("is_span_explicit_v<10, dynamic_extent>", Botan::detail::is_span_explicit_v<10, Botan::dynamic_extent>, true);
         result.confirm("is_span_explicit_v<dynamic_extent, dynamic_extent>", Botan::detail::is_span_explicit_v<Botan::dynamic_extent, Botan::dynamic_extent>, false);
         }),
      Botan_Tests::CHECK("is_span_copyable_v", [](auto& result)
         {
         result.confirm("is_span_copyable_v<int, 5, int, 10>", Botan::detail::is_span_copyable_v<int, 5, int, 10>, false);
         result.confirm("is_span_copyable_v<int, 10, int, 10>", Botan::detail::is_span_copyable_v<int, 10, int, 10>, true);
         result.confirm("is_span_copyable_v<int, 5, int, dynamic_extent>", Botan::detail::is_span_copyable_v<int, 5, int, Botan::dynamic_extent>, true);
         result.confirm("is_span_copyable_v<int, 10, int, dynamic_extent>", Botan::detail::is_span_copyable_v<int, 10, int, Botan::dynamic_extent>, true);
         result.confirm("is_span_copyable_v<const int, 5, int, dynamic_extent>", Botan::detail::is_span_copyable_v<const int, 5, int, Botan::dynamic_extent>, true);
         result.confirm("is_span_copyable_v<const int, 10, int, dynamic_extent>", Botan::detail::is_span_copyable_v<const int, 10, int, Botan::dynamic_extent>, true);
         result.confirm("is_span_copyable_v<int, 10, long, dynamic_extent>", Botan::detail::is_span_copyable_v<int, 10, long, Botan::dynamic_extent>, false);
         }),
      Botan_Tests::CHECK("is_span_v", [](auto& result)
         {
         result.confirm("is_span_v<int>", Botan::detail::is_span_v<int>, false);
         result.confirm("is_span_v<std::string>", Botan::detail::is_span_v<std::string>, false);
         result.confirm("is_span_v<std::array<int, 5>>", Botan::detail::is_span_v<std::array<int, 5>>, false);
         result.confirm("is_span_v<std::vector<int>>", Botan::detail::is_span_v<std::vector<int>>, false);
         result.confirm("is_span_v<Botan::span<int>>", Botan::detail::is_span_v<Botan::span<int>>, true);
         }),
      Botan_Tests::CHECK("is_std_array_v", [](auto& result)
         {
         result.confirm("is_std_array_v<int>", Botan::detail::is_std_array_v<int>, false);
         result.confirm("is_std_array_v<std::string>", Botan::detail::is_std_array_v<std::string>, false);
         result.confirm("is_std_array_v<std::array<int, 5>>", Botan::detail::is_std_array_v<std::array<int, 5>>, true);
         result.confirm("is_std_array_v<std::vector<int>>", Botan::detail::is_std_array_v<std::vector<int>>, false);
         result.confirm("is_std_array_v<Botan::span<int>>", Botan::detail::is_std_array_v<Botan::span<int>>, false);
         }),
      Botan_Tests::CHECK("size(const Container& c)", [](auto& result)
         {
         result.test_is_eq("size(C_ARRAY)", Botan::detail::size(C_ARRAY), SIZE);
         result.test_is_eq("size(ARRAY)", Botan::detail::size(ARRAY), ARRAY.size());
         result.test_is_eq("size(CONST_ARRAY)", Botan::detail::size(CONST_ARRAY), CONST_ARRAY.size());
         result.test_is_eq("size(VECTOR)", Botan::detail::size(VECTOR), VECTOR.size());
         result.test_is_eq("size(CONST_VECTOR)", Botan::detail::size(CONST_VECTOR), CONST_VECTOR.size());
         result.test_is_eq("size(CONST_STRING)", Botan::detail::size(CONST_STRING), CONST_STRING.size());
         }),
       Botan_Tests::CHECK("data(const Container& c)", [](auto& result)
         {
         result.test_is_eq("data(C_ARRAY)", Botan::detail::data(C_ARRAY), &C_ARRAY[0]);
         result.test_is_eq("data(ARRAY)", Botan::detail::data(ARRAY), ARRAY.data());
         result.test_is_eq("data(CONST_ARRAY)", Botan::detail::data(CONST_ARRAY), CONST_ARRAY.data());
         result.test_is_eq("data(VECTOR)", Botan::detail::data(VECTOR), VECTOR.data());
         result.test_is_eq("data(CONST_VECTOR)", Botan::detail::data(CONST_VECTOR), CONST_VECTOR.data());
         result.test_is_eq("data(CONST_STRING)", Botan::detail::data(CONST_STRING), CONST_STRING.data());
         }),
      Botan_Tests::CHECK("has_size_v", [](auto& result)
         {
         result.confirm("has_size_v<int>", Botan::detail::has_size_v<int>, false);
         result.confirm("has_size_v<float>", Botan::detail::has_size_v<float>, false);
         result.confirm("has_size_v<Dummy<int>>", Botan::detail::has_size_v<Dummy<int>>, false);
         result.confirm("has_size_v<With_size<int>>", Botan::detail::has_size_v<With_size<int>>, true);
         result.confirm("has_size_v<std::string>", Botan::detail::has_size_v<std::string>, true);
         result.confirm("has_size_v<std::array<int, 5>>", Botan::detail::has_size_v<std::array<int, 5>>, true);
         result.confirm("has_size_v<std::vector<int>>", Botan::detail::has_size_v<std::vector<int>>, true);
         result.confirm("has_size_v<Botan::span<int>>", Botan::detail::has_size_v<Botan::span<int>>, true);
         }),
      Botan_Tests::CHECK("has_data_v", [](auto& result)
         {
         result.confirm("has_data_v<int>", Botan::detail::has_data_v<int>, false);
         result.confirm("has_data_v<float>", Botan::detail::has_data_v<float>, false);
         result.confirm("has_data_v<Dummy<int>>", Botan::detail::has_data_v<Dummy<int>>, false);
         result.confirm("has_data_v<With_data<int>>", Botan::detail::has_data_v<With_data<int>>, true);
         result.confirm("has_data_v<std::string>", Botan::detail::has_data_v<std::string>, true);
         result.confirm("has_data_v<std::array<int, 5>>", Botan::detail::has_data_v<std::array<int, 5>>, true);
         result.confirm("has_data_v<std::vector<int>>", Botan::detail::has_data_v<std::vector<int>>, true);
         result.confirm("has_data_v<Botan::span<int>>", Botan::detail::has_data_v<Botan::span<int>>, true);
         }),
      Botan_Tests::CHECK("is_container_v", [](auto& result)
         {
         result.confirm("is_container_v<int>", Botan::detail::is_container_v<int>, false);
         result.confirm("is_container_v<float>", Botan::detail::is_container_v<float>, false);
         result.confirm("is_container_v<Dummy<int>>", Botan::detail::is_container_v<Dummy<int>>, false);
         result.confirm("is_container_v<With_size<int>>", Botan::detail::is_container_v<With_size<int>>, false);
         result.confirm("is_container_v<With_data<int>>", Botan::detail::is_container_v<With_data<int>>, false);
         result.confirm("is_container_v<With_size_and_data<int>>", Botan::detail::is_container_v<With_size_and_data<int>>, true);
         result.confirm("is_container_v<std::string>", Botan::detail::is_container_v<std::string>, true);
         result.confirm("is_container_v<std::array<int, 5>>", Botan::detail::is_container_v<std::array<int, 5>>, false);
         result.confirm("is_container_v<std::vector<int>>", Botan::detail::is_container_v<std::vector<int>>, true);
         result.confirm("is_container_v<Botan::span<int>>", Botan::detail::is_container_v<Botan::span<int>>, false);
         }),
      Botan_Tests::CHECK("is_container_type_compatible_v", [](auto& result)
         {
         result.confirm("is_container_type_compatible_v<int, int>", Botan::detail::is_container_type_compatible_v<int, int>, false);
         result.confirm("is_container_type_compatible_v<Dummy<int>, int>", Botan::detail::is_container_type_compatible_v<Dummy<int>, int>, false);
         result.confirm("is_container_type_compatible_v<With_size_and_data<int>, float>", Botan::detail::is_container_type_compatible_v<With_size_and_data<int>, float>, false);
         result.confirm("is_container_type_compatible_v<With_size_and_data<int>, int>", Botan::detail::is_container_type_compatible_v<With_size_and_data<int>, int>, true);
         result.confirm("is_container_type_compatible_v<std::string, int>", Botan::detail::is_container_type_compatible_v<std::string, int>, false);
         result.confirm("is_container_type_compatible_v<std::array<int, 5>, const int>", Botan::detail::is_container_type_compatible_v<std::array<int, 5>, const int>, true);
         result.confirm("is_container_type_compatible_v<std::vector<int>, unsigned int>", Botan::detail::is_container_type_compatible_v<std::vector<int>, unsigned int>, false);
         result.confirm("is_container_type_compatible_v<Botan::span<int>, const int>", Botan::detail::is_container_type_compatible_v<Botan::span<int>, const int>, true);
         result.confirm("is_container_type_compatible_v<Botan::span<const int>, int>", Botan::detail::is_container_type_compatible_v<Botan::span<const int>, int>, false);
         })
      };
   }

BOTAN_REGISTER_TEST_FN("utils", "span", test_ctors,
                                        test_assignment_operator,
                                        test_iterators,
                                        test_element_access,
                                        test_observers,
                                        test_subviews,
                                        test_non_member,
                                        test_misc,
                                        test_span_detail);

}

}

