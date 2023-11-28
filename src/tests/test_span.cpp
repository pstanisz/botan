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
#include <iostream>

namespace Botan_Tests {

namespace {

constexpr size_t SIZE = 5UL;
int C_ARRAY[SIZE] = {1, 2, 3, 4, 5};
std::array<int, SIZE> ARRAY = {1, 2, 3, 4, 5};
const std::array<int, SIZE> CONST_ARRAY = {1, 2, 3, 4, 5};

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
        Botan::span<int> s(std::begin(ARRAY), ARRAY.size());

         result.test_is_eq("size()", s.size(), ARRAY.size());
         result.test_is_eq("data()", s.data(), ARRAY.begin());
         }),
      Botan_Tests::CHECK("ctor with first and last", [](auto& result)
         {
        Botan::span<int> s(std::begin(ARRAY), std::end(ARRAY));

         result.test_is_eq("size()", s.size(), ARRAY.size());
         result.test_is_eq("data()", s.data(), ARRAY.begin());
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
         result.test_is_eq("data()", s.data(), ARRAY.begin());
         }),
      Botan_Tests::CHECK("ctor with const std::array", [](auto& result)
         {
         const Botan::span<const int> s(CONST_ARRAY);

         result.test_is_eq("size()", s.size(), CONST_ARRAY.size());
         result.test_is_eq("data()", s.data(), CONST_ARRAY.begin());
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

         result.test_is_eq("empty() is true", s.empty(), true);
         }),
      Botan_Tests::CHECK("empty() with non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);

         result.test_is_eq("empty() is false", s.empty(), false);
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

         result.test_is_eq("first(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("first(0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.first(0UL);

         result.test_is_eq("first(0) gives empty span", sub.empty(), true);
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

         result.test_is_eq("last(0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("last(0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.last(0UL);

         result.test_is_eq("last(0) gives empty span", sub.empty(), true);
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

         result.test_is_eq("subspan(0, 0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("subspan(0, 0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(0UL, 0UL);

         result.test_is_eq("subspan(0, 0) gives empty span", sub.empty(), true);
         }),
      Botan_Tests::CHECK("subspan(1, 0) from non-empty", [](auto& result)
         {
        Botan::span<int> s(ARRAY);
         auto sub = s.subspan(1UL, 0UL);

         result.test_is_eq("subspan(1, 0) gives empty span", sub.empty(), true);
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

         result.test_is_eq("as_bytes() gives empty span", const_bytes.empty(), true);
         }),
      Botan_Tests::CHECK("as_writable_bytes() from empty", [](auto& result)
         {
        Botan::span<int> s;
         auto bytes = Botan::as_bytes(s);

         result.test_is_eq("as_writable_bytes() gives empty span", bytes.empty(), true);
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

// TODO:
std::vector<Test::Result> test_misc()
   {
   return
      {
      Botan_Tests::CHECK("from vector<int>", [](auto& result)
         {
         std::vector<int> vec {1, 2, 3};
         Botan::span s(vec);

         const Botan::secure_vector<unsigned char> vec1 {1, 2, 3};
         const Botan::span s1(vec1);
         }), 
      Botan_Tests::CHECK("from secure_vector", [](auto& result)
         {
         Botan::secure_vector<uint8_t> vec {1, 2, 3};
         Botan::span s(vec);

         const Botan::secure_vector<unsigned char> vec1 {1, 2, 3};
         const Botan::span s1(vec1);
         }),
      Botan_Tests::CHECK("from const secure_vector", [](auto& result)
         {
         std::string str = "abc";
         Botan::span s(str);
         })
      };
   }

// TODO:
std::vector<Test::Result> test_span_detail()
   {
   return
      {
      Botan_Tests::CHECK("is_span_convertible", [](auto& result)
         {
         result.test_is_eq("is_span_convertible<int, int>", Botan::detail::is_span_convertible<int, int>::value, true);
         result.test_is_eq("is_span_convertible<int, const int>", Botan::detail::is_span_convertible<int, const int>::value, true);
         result.test_is_eq("is_span_convertible<int, unsigned int>", Botan::detail::is_span_convertible<int, unsigned int>::value, false);
         result.test_is_eq("is_span_convertible<int*, void*>", Botan::detail::is_span_convertible<int*, void*>::value, false);
         }),
      Botan_Tests::CHECK("is_span_capacity", [](auto& result)
         {
         result.test_is_eq("is_span_capacity<0, 0>", Botan::detail::is_span_capacity<0, 0>::value, true);
         result.test_is_eq("is_span_capacity<10, 10>", Botan::detail::is_span_capacity<10, 10>::value, true);
         result.test_is_eq("is_span_capacity<1, 0>", Botan::detail::is_span_capacity<1, 0>::value, false);
         result.test_is_eq("is_span_capacity<10, 11>", Botan::detail::is_span_capacity<10, 11>::value, false);
         result.test_is_eq("is_span_capacity<10UL, 10>", Botan::detail::is_span_capacity<10UL, 10>::value, true);
         result.test_is_eq("is_span_capacity<dynamic_extent, 10>", Botan::detail::is_span_capacity<Botan::dynamic_extent, 10>::value, true);
         }),
      Botan_Tests::CHECK("is_span_compatible", [](auto& result)
         {
         result.test_is_eq("is_span_compatible<int, 5, int, 5>", Botan::detail::is_span_compatible<int, 5, int, 5>::value, true);
         result.test_is_eq("is_span_compatible<int, 5, int, 10>", Botan::detail::is_span_compatible<int, 5, int, 10>::value, false);
         result.test_is_eq("is_span_compatible<int, 5, const int, 5>", Botan::detail::is_span_compatible<int, 5, const int, 5>::value, false);
         result.test_is_eq("is_span_compatible<int, dynamic_extent, int, 5>", Botan::detail::is_span_compatible<int, Botan::dynamic_extent, int, 5>::value, true);
         result.test_is_eq("is_span_compatible<int, dynamic_extent, long, 5>", Botan::detail::is_span_compatible<int, Botan::dynamic_extent, long, 5>::value, false);
         }),
         
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

