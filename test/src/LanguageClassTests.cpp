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
    const static string TRANSLATION_WITH_INTERNAL_COMMENT = R"(translation = "this # shouldn't match")";

    ASSERT_EQ(removeInlineComments(SECTION_WITH_COMMENT), R"([section])");
    ASSERT_EQ(removeInlineComments(SECTION_WITH_COMMENT_2), R"([section])");
    ASSERT_EQ(removeInlineComments(TRANSLATION_WITH_COMMENT), R"(translation = "")");
    ASSERT_EQ(removeInlineComments(TRANSLATION_WITH_COMMENT_2), R"(translation[] = "")");
    ASSERT_EQ(removeInlineComments(TRANSLATION_WITH_INTERNAL_COMMENT), TRANSLATION_WITH_INTERNAL_COMMENT);
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

TEST_F(LanguageClassTests, testContainsVariable) {
    string varName{};
    ASSERT_TRUE(containsVariable("${var_name}", varName));
    ASSERT_EQ(varName, "var_name");

    varName.clear();
    ASSERT_TRUE(containsVariable("${_Test}", varName));
    ASSERT_EQ(varName, "_Test");

    varName.clear();
    ASSERT_TRUE(containsVariable("${TESt}", varName));
    ASSERT_EQ(varName, "TESt");

    varName.clear();
    ASSERT_TRUE(containsVariable("${test:test_01}", varName));
    ASSERT_EQ(varName, "test:test_01");

    varName.clear();
    ASSERT_FALSE(containsVariable("${0bla}", varName));
    ASSERT_TRUE(varName.empty());

    varName.clear();
    ASSERT_FALSE(containsVariable("{dDWdw}", varName));
    ASSERT_TRUE(varName.empty());

    varName.clear();
    ASSERT_FALSE(containsVariable("[öiubub]", varName));
    ASSERT_TRUE(varName.empty());

    varName.clear();
    ASSERT_FALSE(containsVariable("${*broken_var}", varName));
    ASSERT_TRUE(varName.empty());
}

TEST_F(LanguageClassTests, testParseLineCommentLine) {
    ASSERT_NO_THROW(parseLine("#öbiubzvouvzvouv"));
}

TEST_F(LanguageClassTests, testParseLine_EmptyLine) {
    ASSERT_NO_THROW(parseLine(""));
}

TEST_F(LanguageClassTests, testParseLine_LangIdLine) {
    ASSERT_NO_THROW(parseLine(R"(lang_id = "test_language")"));
    ASSERT_EQ(getLangId(), "test_language");
}

TEST_F(LanguageClassTests, testParseLine_LangDescriptionLine) {
    ASSERT_NO_THROW(parseLine(R"(lang_desc = "This is a test description")"));
    ASSERT_EQ(getLangDescription(), "This is a test description");
}

TEST_F(LanguageClassTests, testParseLine_LangVerLine) {
    ASSERT_NO_THROW(parseLine(R"(lang_ver = "1.9.0")"));
    const auto& langVer = getLanguageVersion();
    ASSERT_EQ(langVer.getMajorVersion(), 1);
    ASSERT_EQ(langVer.getMinorVersion(), 9);
    ASSERT_EQ(langVer.getRevision(), 0);
}

TEST_F(LanguageClassTests, testParseString_validData) {
    borr::language lang;
    ASSERT_NO_THROW(borr::language::fromString(R"(
        # test translation

        lang_id = "test_lang"
        lang_ver = "1.0.0"
        lang_desc = "This is a test"

        [test]
        test_value_0 = "Test01"
        test_value_1[] = "Multi"
        test_value_1[] = "Line"
    )", lang));

    ASSERT_EQ(lang.getLangDescription(), "This is a test");
    ASSERT_EQ(lang.getLangId(), "test_lang");
    ASSERT_EQ(lang.getLanguageVersion().getMajorVersion(), 1);
    ASSERT_EQ(lang.getLanguageVersion().getMinorVersion(), 0);
    ASSERT_EQ(lang.getLanguageVersion().getRevision(), 0);

    ASSERT_EQ(lang.getString("test", "test_value_0"), "Test01");
    ASSERT_EQ(lang.getString("test", "test_value_1"), "Multi\nLine");
}

TEST_F(LanguageClassTests, testVariableExpansion) {
    borr::language lang;

    const auto customExpander = [&](const string&) { return "This is an expansion test"; };

    const auto timeStart = time(nullptr);
    const auto detailledTimeStart = localtime(&timeStart);

    ASSERT_NO_THROW(borr::language::addVarExpansionCallback("customExpander", customExpander));

    ASSERT_NO_THROW(borr::language::fromString(R"(
        lang_id = "test_lang"
        lang_ver = "1.0.0"
        lang_desc = "This is a test"

        [test]
        test_01 = "${date}"
        test_02 = "${time}"
        test_03 = "The date is ${test:test_01}"
        test_04 = "The time is ${test:test_02}"
        test_05 = "${customExpander}"
    )", lang));

    ASSERT_EQ(lang.getString("test", "test_05"), customExpander(""));
    ASSERT_EQ(lang.getString("test", "test_01"), dateExpander(""));
    ASSERT_EQ(lang.getString("test", "test_02"), timeExpander(""));
    ASSERT_EQ(lang.getString("test", "test_03"), "The date is " + dateExpander(""));
    ASSERT_EQ(lang.getString("test", "test_04"), "The time is " + timeExpander(""));

}