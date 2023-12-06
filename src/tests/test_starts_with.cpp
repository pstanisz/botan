/*
 * (C) 2023 Piotr Staniszewski - CodeVision
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include "tests.h"

#include <botan/starts_with.h>

namespace Botan_Tests {

namespace {

std::vector<Test::Result> test_starts_with() {
   return {Botan_Tests::CHECK("starts_with empty",
                              [](auto& result) {
                                 std::string input{"Hello, World!"};

                                 // This is a behavior of std::string's starts_with("")
                                 result.confirm("starts with", Botan::starts_with(input, ""), true);
                              }),
           Botan_Tests::CHECK("empty starts_with empty",
                              [](auto& result) {
                                 std::string input{""};

                                 // This is a behavior of empty std::string's starts_with("")
                                 result.confirm("starts with", Botan::starts_with(input, ""), true);
                              }),
           Botan_Tests::CHECK("empty starts_with non-empty",
                              [](auto& result) {
                                 std::string input{""};

                                 result.confirm("not starts with", Botan::starts_with(input, "Hello"), false);
                              }),
           Botan_Tests::CHECK("std::string starts_with",
                              [](auto& result) {
                                 std::string input{"Hello, World!"};

                                 result.confirm("not starts with", Botan::starts_with(input, "World!"), false);
                                 result.confirm("starts with", Botan::starts_with(input, "Hello"), true);
                              }),
           Botan_Tests::CHECK("std::string_view starts_with",
                              [](auto& result) {
                                 std::string str{"Hello, World!"};
                                 std::string_view input{str};

                                 result.confirm("not starts with", Botan::starts_with(input, "World!"), false);
                                 result.confirm("starts with", Botan::starts_with(input, "Hello"), true);
                              }),
           Botan_Tests::CHECK("starts_with std::string",
                              [](auto& result) {
                                 std::string input{"Hello, World!"};

                                 result.confirm(
                                    "not starts with", Botan::starts_with(input, std::string("World!")), false);
                                 result.confirm("starts with", Botan::starts_with(input, std::string("Hello")), true);
                              }),
           Botan_Tests::CHECK("starts_with std::string_view",
                              [](auto& result) {
                                 std::string input{"Hello, World!"};
                                 std::string_view world = "World!";
                                 std::string_view hello = "Hello";

                                 result.confirm("not starts with", Botan::starts_with(input, world), false);
                                 result.confirm("starts with", Botan::starts_with(input, hello), true);
                              }),
           Botan_Tests::CHECK("starts_with char*", [](auto& result) {
              std::string input{"Hello, World!"};
              const char world[] = "World!";
              const char hello[] = "Hello";

              result.confirm("not starts with", Botan::starts_with(input, world), false);
              result.confirm("starts with", Botan::starts_with(input, hello), true);
           })};
}
}  // namespace

BOTAN_REGISTER_TEST_FN("utils", "starts_with", test_starts_with);

}  // namespace Botan_Tests
