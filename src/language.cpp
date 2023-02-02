/**
 * @file language.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of the language class.
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <regex>
#include <string>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "borr/extensions.hpp"
#include "borr/language.hpp"
#include "borr/langversion.hpp"
#include "borr/string_splitter.hpp"

namespace borr {

    namespace fs = std::filesystem;

    using std::error_code;
    using std::regex;
    using std::regex_replace;
    using std::smatch;

    language language::fromFile(const fs::directory_entry& file) {
        if (!file.exists() || !file.is_regular_file()) {
            throw fs::filesystem_error("Invalid file path given!", file.path(), error_code(EINVAL, std::generic_category()));
        }

        ifstream inStream(file.path());
        string fContents{};

        inStream >> fContents;

        return fromString(fContents);
    }

    language language::fromString(const string& fContents) {
        language lang;
        StringSplit splitter(fContents, "\n");

        for (const auto& line : splitter) {
            lang.parseLine(line.data());
        }

        return lang;
    }

    language::language(): m_langDescription({}), m_langId({}), m_langVer(), m_translationDict({}) {
    }

    language::language(const language& instance):
        m_langDescription(instance.m_langDescription),
        m_langId(instance.m_langId),
        m_langVer(instance.m_langVer),
        m_translationDict(instance.m_translationDict) {
    }

    bool language::containsVariable(const string& translation, string& outVarName) const {
        namespace rc = std::regex_constants;
        const static regex VAR_REGEX(VARIABLE_REGEX.data(), rc::optimize | rc::extended);

        smatch matches;
        if (std::regex_search(translation, matches, VAR_REGEX)) {
            outVarName = extensions::trim(extensions::trim(*matches.begin(), "$"), "{}");
            return true;
        }

        return false;
    }

    bool language::isEmptyOrComment(const string& line) const {
        return extensions::trim(line).empty() || extensions::trim(line).at(0) == '#';
    }

    bool language::isMultilineField(const string& field) const {
        return *field.rbegin() == ']' && *(field.rbegin() + 1) == '[';
    }

    bool language::isSection(const string& line, string& outSection) const {
        namespace rc = std::regex_constants;
        if (!std::regex_match(line, regex(SECTION_REGEX.data(), rc::extended | rc::optimize))) { return false; }

        outSection = extensions::trim(line, "[]");
        return true;
    }

    bool language::isTranslation(const string& line, string& outFieldName, string& outTranslation) const {
        namespace rc = std::regex_constants;
        if (!std::regex_match(line, regex(TRANSLATION_REGEX.data(), rc::extended | rc::optimize))) { return false; }

        // Now just split the string at the first '=' and return the two halfs
        const auto posOfDelim = line.find('=');

        outFieldName = extensions::trim(line.substr(0, posOfDelim - 1));
        outTranslation = extensions::trim(line.substr(posOfDelim + 1), "\" \t\r");
        return true;
    }

    optsect_t language::getSection(const string& sectionName) const {
        const auto iterPos = m_translationDict.find(sectionName);

        if (iterPos == m_translationDict.end()) { return {}; }

        return iterPos->second;
    }

    string language::expandVariable(const string& varName) const {
        return {};
    }

    string language::removeInlineComments(const string& line) const {
        namespace rc = std::regex_constants;
        static const regex COMMENT_REGEX = regex(R"(#[^\n]+$)", rc::optimize | rc::extended);

        string copy = line;

        while (std::regex_match(line, COMMENT_REGEX)) {
            copy = regex_replace(copy, COMMENT_REGEX, "");
        }

        return copy;
    }

    void language::parseLine(const string& line) {
        if (isEmptyOrComment(line)) { return; }

        // now search for inline comments
        const auto commentlessLine = removeInlineComments(line);

        static string currentSection{};
        if (isSection(commentlessLine, currentSection)) { return; } // nothing more to do here

        string field;
        string translation;
        if (!isTranslation(commentlessLine, field, translation)) { return; } // nothing more to do here

        if (currentSection.empty()) {
            if (field == LANG_DESC_FIELD) {
                m_langDescription = translation;
                return;
            } else if (field == LANG_ID_FIELD) {
                m_langId = translation;
                return;
            } else if (field == LANG_VER_FIELD) {
                langversion::fromString(translation, m_langVer);
                return;
            }
            return;
        }

        if (!getSection(currentSection).has_value()) {
            m_translationDict.emplace(currentSection, sect_t{});
        }

        auto& section = m_translationDict[currentSection];

        const auto trimmedField = extensions::trim(field, "[]");

        if (section.find(field) != section.end() && isMultilineField(field)) {
            section[field] = section[field] + "\n" + translation;
        } else {
            section.emplace(field, translation);
        }
    }

}