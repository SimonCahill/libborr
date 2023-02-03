/**
 * @file LanguageClassTests.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of unit tests for testing the language class.
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#include "borr/language.hpp"
#include "borr/extensions.hpp"

#include <gtest/gtest.h>

using std::string;
using std::vector;

class LanguageClassTests: public testing::Test, public borr::language {
    protected:
        void SetUp() {
        }
};

TEST_F(LanguageClassTests, testIsEmptyOrComment) {
    const static string COMMENT_LINE    = "#this line is a comment";
    const static string COMMENT_LINE_2  = "# this line is also a comment!";
    const static string COMMENT_LINE_3  = "  # this is another comment line";
    const static string EMPTY_LINE      = "";
    const static string EMPTY_LINE_2    = "       ";
    const static string EMPTY_LINE_3    = "\t\t\t";

    ASSERT_TRUE(isEmptyOrComment(COMMENT_LINE));
    ASSERT_TRUE(isEmptyOrComment(COMMENT_LINE_2));
    ASSERT_TRUE(isEmptyOrComment(COMMENT_LINE_3));
    ASSERT_TRUE(isEmptyOrComment(EMPTY_LINE));
    ASSERT_TRUE(isEmptyOrComment(EMPTY_LINE_2));
    ASSERT_TRUE(isEmptyOrComment(EMPTY_LINE_3));

    ASSERT_FALSE(isEmptyOrComment("This is neither a command, not an empty line!"));
}

TEST_F(LanguageClassTests, testRemoveInlineComments) {
    const static string SECTION_WITH_COMMENT = R"([section] # and a comment)";
    const static string SECTION_WITH_COMMENT_2 = R"([section]#comment)";
    const static string TRANSLATION_WITH_COMMENT = R"(translation = "" # comment)";
    const static string TRANSLATION_WITH_COMMENT_2 = R"(translation[] = ""#COMMENTS ARE WEIRD)";

    ASSERT_EQ(removeInlineComments(SECTION_WITH_COMMENT), R"([section])");
    ASSERT_EQ(removeInlineComments(SECTION_WITH_COMMENT_2), R"([section])");
    ASSERT_EQ(removeInlineComments(TRANSLATION_WITH_COMMENT), R"(translation = "")");
    ASSERT_EQ(removeInlineComments(TRANSLATION_WITH_COMMENT_2), R"(translation[] = "")");
}

TEST_F(LanguageClassTests, testIsMultilineField) {
    ASSERT_FALSE(isMultilineField(R"(translation)"));
    ASSERT_TRUE(isMultilineField(R"(translation[])"));
    ASSERT_TRUE(isMultilineField(R"(translation [])"));
}

TEST_F(LanguageClassTests, testIsTranslation) {
    string field, value;

    ASSERT_TRUE(isTranslation(R"(translation = "Test")", field, value));
    ASSERT_EQ(field, "translation");
    ASSERT_EQ(value, "Test");

    ASSERT_TRUE(isTranslation(R"(translation       =           "Test")", field, value));
    ASSERT_EQ(field, "translation");
    ASSERT_EQ(value, "Test");

    ASSERT_TRUE(isTranslation(R"(translation[] = "Multiline Test")", field, value));
    ASSERT_EQ(field, "translation[]");
    ASSERT_EQ(value, "Multiline Test");

    ASSERT_FALSE(isTranslation("[bla]", field, value));
    ASSERT_FALSE(isTranslation("oinoubiudwbiudbw9b9fb9f3ubpfbpf 3g93 3", field, value));
    ASSERT_FALSE(isTranslation("", field, value));
}

TEST_F(LanguageClassTests, testIsSection) {
    const static vector<string> VALID_SECTIONS = {
        R"([section])",
        R"([_section])",
        R"([______section_section_section])",
    };
    const static vector<string> INVALID_SECTIONS = {
        R"(ipogzf6fo)",
        R"(0987654456)"
        "\t",
        "",
        "    ",
        "\n",
        "(test)",
        "{test}",
        "[0invalid]",
        "[1_invalid]",
        "{_invalid}",
        "---",
        R"([SecTi0N]     )",

        R"(    [0987654321234567_hjvuztresxghfcjo6fuvh])"
    };

    for (const auto& sect : VALID_SECTIONS) {
        string trimmedSection{};
        ASSERT_NO_THROW(isSection(sect, trimmedSection));
        ASSERT_TRUE(isSection(sect, trimmedSection));
        ASSERT_EQ(trimmedSection, borr::extensions::trim(sect, " []"));
    }

    for (const auto& sect : INVALID_SECTIONS) {
        string trimmedSection{};
        ASSERT_NO_THROW(isSection(sect, trimmedSection));
        ASSERT_FALSE(isSection(sect, trimmedSection));
        ASSERT_TRUE(trimmedSection.empty());
    }
}