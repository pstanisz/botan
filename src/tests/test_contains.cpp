/*
 * (C) 2023 Piotr Staniszewski - CodeVision
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include "tests.h"

#include <botan/contains.h>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace Botan_Tests {

namespace {

std::vector<Test::Result> test_contains() {
   return {
      Botan_Tests::CHECK("std::map contains",
                         [](auto& result) {
                            std::map<int, std::string> input{{1, "a"}, {2, "b"}, {3, "c"}};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 1), true);
                         }),
      Botan_Tests::CHECK("std::multimap contains",
                         [](auto& result) {
                            std::multimap<int, std::string> input{{1, "a"}, {2, "b"}, {3, "c"}, {2, "d"}};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 2), true);
                         }),
      Botan_Tests::CHECK("std::set contains",
                         [](auto& result) {
                            std::set<int> input{1, 2, 3};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 1), true);
                         }),
      Botan_Tests::CHECK("std::multiset contains",
                         [](auto& result) {
                            std::multiset<int> input{1, 2, 3, 2};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 2), true);
                         }),

      Botan_Tests::CHECK("std::unordered_map contains",
                         [](auto& result) {
                            std::unordered_map<int, std::string> input{{1, "a"}, {2, "b"}, {3, "c"}};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 1), true);
                         }),
      Botan_Tests::CHECK("std::unordered_multimap contains",
                         [](auto& result) {
                            std::unordered_multimap<int, std::string> input{{1, "a"}, {2, "b"}, {3, "c"}, {2, "d"}};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 2), true);
                         }),
      Botan_Tests::CHECK("std::unordered_set contains",
                         [](auto& result) {
                            std::unordered_set<int> input{1, 2, 3};

                            result.confirm("not contains", Botan::contains(input, 5), false);
                            result.confirm("contains", Botan::contains(input, 1), true);
                         }),
      Botan_Tests::CHECK("std::unordered_multiset contains", [](auto& result) {
         std::unordered_multiset<int> input{1, 2, 3, 2};

         result.confirm("not contains", Botan::contains(input, 5), false);
         result.confirm("contains", Botan::contains(input, 2), true);
      })};
}
}  // namespace

BOTAN_REGISTER_TEST_FN("utils", "contains", test_contains);

}  // namespace Botan_Tests
