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
         std::array<int, 5> arr {1,2,3,4,5};
         Botan::span<int> s(std::begin(arr), size_t(3U));

         result.test_is_eq("size()", s.size(), size_t(3U));
         result.test_not_null("data()", s.data());
         }),
      Botan_Tests::CHECK("ctor with first and last", [](auto& result)
         {
         std::array<int, 5> arr {1,2,3,4,5};
         Botan::span<int> s(std::begin(arr), std::end(arr));

         result.test_is_eq("size()", s.size(), arr.size());
         result.test_not_null("data()", s.data());
         }),
      Botan_Tests::CHECK("ctor with C array", [](auto& result)
         {
         int arr[5] = {1,2,3,4,5};
         Botan::span<int> s(arr);

         result.test_is_eq("size()", s.size(), size_t(5));
         result.test_not_null("data()", s.data());
         }),
      Botan_Tests::CHECK("ctor with std::array", [](auto& result)
         {
         std::array<int, 5> arr {1,2,3,4,5};
         Botan::span<int> s(arr);

         result.test_is_eq("size()", s.size(), arr.size());
         result.test_not_null("data()", s.data());
         }),
      Botan_Tests::CHECK("ctor with const std::array", [](auto& result)
         {
         const std::array<int, 5> arr {1,2,3,4,5};
         const Botan::span<const int> s(arr);

         result.test_is_eq("size()", s.size(), arr.size());
         result.test_not_null("data()", s.data());
         }),
      Botan_Tests::CHECK("ctor with span", [](auto& result)
         {
         std::array<int, 5> arr {1,2,3,4,5};
         const Botan::span<int> s(arr);
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
      Botan_Tests::CHECK("operator=", [](auto& result)
         {
         Botan::span<int> s;

         std::array<int, 5> arr {1,2,3,4,5};
         Botan::span<int> s1(arr);

         s = s1;
         
         result.test_is_eq("size()", s.size(), s1.size());
         result.test_is_eq("data()", s.data(), s1.data());
         })
      };
}

BOTAN_REGISTER_TEST_FN("utils", "span", test_ctors, test_assignment_operator);
}

}

