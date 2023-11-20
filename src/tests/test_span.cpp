/*
* (C) 2023 Piotr Staniszewski - CodeVision
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include "tests.h"

#include <botan/span.h>
#include <array>

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
         result.test_is_eq("end() points to one after last", &(*(--s.end())), &ARRAY[SIZE-1]);
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
         result.test_is_eq("rend() points to one before first", &(*(--s.rend())), &ARRAY[0]);
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

BOTAN_REGISTER_TEST_FN("utils", "span", test_ctors,
                                        test_assignment_operator,
                                        test_iterators,
                                        test_element_access,
                                        test_observers);
}

}

