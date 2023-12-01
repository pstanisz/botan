/*
 * (C) 2023 Piotr Staniszewski - CodeVision
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include "tests.h"

#include <botan/erase_if.h>

#include <map>

namespace Botan_Tests
{

namespace {

std::vector<Test::Result> test_erase_if()
    {
    return 
        {
        Botan_Tests::CHECK("erase_if from empty", [](auto &result)
            {
            std::map<int, std::string> m {};
            
            auto erased_count = Botan::erase_if(m, [](const auto&) { return true; });
            result.test_is_eq("erase_if from empty", erased_count, size_t(0));
            }),
        Botan_Tests::CHECK("erase_if none", [](auto &result)
            {
            std::map<int, std::string> m {{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
            
            auto erased_count = Botan::erase_if(m, [](const auto&) { return false; });
            result.test_is_eq("erase_if erases nothing", erased_count, size_t(0));
            }),
        Botan_Tests::CHECK("erase_if single", [](auto &result)
            {
            std::map<int, std::string> m {{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
            
            auto erased_count = Botan::erase_if(m, [](const auto& elem) { return elem.first == 2; });
            result.test_is_eq("erase_if erases 1 element", erased_count, size_t(1));
            }),
        Botan_Tests::CHECK("erase_if all", [](auto &result)
            {
            std::map<int, std::string> m {{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
            
            auto erased_count = Botan::erase_if(m, [](const auto&) { return true; });
            result.test_is_eq("erase_if erases all elements", erased_count, size_t(4));
            }),
        };
    }
}

BOTAN_REGISTER_TEST_FN("utils", "erase_if", test_erase_if);

}
