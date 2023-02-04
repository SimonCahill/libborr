/**
 * @file ExtensionsTests.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains test suites for testing extension methods used in libborr.
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "borr/extensions.hpp"

using std::string;
using std::vector;

TEST(ExtensionsTests, testTrimStart) {
    const static string NON_TRIMMED_STRING = " \t  THIS IS NOT TRIMMED   ";
    const static string TRIMMED_STRING     = "THIS IS NOT TRIMMED   "; // well I mean it kinda is now...

    ASSERT_EQ(borr::extensions::trimStart(NON_TRIMMED_STRING), TRIMMED_STRING);
}

TEST(ExtensionsTests, testTrimStartWithOtherChars) {
    const static string NON_TRIMMED_STRING = "$$$%%%&&&THIS IS NOT TRIMMED";
    const static string TRIMMED_STRING     = "THIS IS NOT TRIMMED"; // see comment in testTrimStart

    ASSERT_EQ(borr::extensions::trimStart(NON_TRIMMED_STRING, "$%&"), TRIMMED_STRING);
}

TEST(ExtensionsTests, testTrimEnd) {
    const static string NON_TRIMMED_STRING = " \t  THIS IS NOT TRIMMED   ";
    const static string TRIMMED_STRING     = " \t  THIS IS NOT TRIMMED"; // well I mean it kinda is now...

    ASSERT_EQ(borr::extensions::trimEnd(NON_TRIMMED_STRING), TRIMMED_STRING);
}

TEST(ExtensionsTests, testTrimEndWithOtherChars) {
    const static string NON_TRIMMED_STRING = "THIS IS NOT TRIMMED$$$%%%&&&";
    const static string TRIMMED_STRING     = "THIS IS NOT TRIMMED"; // see comment in testTrimStart

    ASSERT_EQ(borr::extensions::trimEnd(NON_TRIMMED_STRING, "$%&"), TRIMMED_STRING);
}

TEST(ExtensionsTests, testTrim) {
    const static string NON_TRIMMED_STRING = " \t  THIS IS NOT TRIMMED   ";
    const static string TRIMMED_STRING     = "THIS IS NOT TRIMMED"; // well I mean it kinda is now...

    ASSERT_EQ(borr::extensions::trim(NON_TRIMMED_STRING), TRIMMED_STRING);
}

TEST(ExtensionsTests, testTrimWithOtherChars) {
    const static string NON_TRIMMED_STRING = "///(())==THIS IS NOT TRIMMED$$$%%%&&&";
    const static string TRIMMED_STRING     = "THIS IS NOT TRIMMED"; // well I mean it kinda is now...

    ASSERT_EQ(borr::extensions::trim(NON_TRIMMED_STRING, "/()=$%&"), TRIMMED_STRING);
}

TEST(ExtensionsTests, testSplitString_whitespace) {
    const static string STRING_TO_SPLIT = "Test tseT sEtt";

    vector<string> tokens;
    ASSERT_TRUE(borr::extensions::splitString(STRING_TO_SPLIT, " ", tokens));

    ASSERT_EQ(tokens.size(), 3);
    ASSERT_EQ(tokens[0], "Test");
    ASSERT_EQ(tokens[1], "tseT");
    ASSERT_EQ(tokens[2], "sEtt");

}

TEST(ExtensionsTests, testSplitString_newLines) {
    const static string STRING_TO_SPLIT(R"(
        This
        Contains
        Newlines
    )");

    vector<string> tokens;
    ASSERT_TRUE(borr::extensions::splitString(borr::extensions::trim(STRING_TO_SPLIT), "\n", tokens));

    ASSERT_EQ(tokens.size(), 3);
}