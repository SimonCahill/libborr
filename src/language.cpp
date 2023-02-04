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
    using std::vector;

    varcbacklist_t language::_callbackList = {}; //!< Initialise private static member

    void language::fromFile(const fs::directory_entry& file, language& outLang) {
        if (!file.exists() || !file.is_regular_file()) {
            throw fs::filesystem_error("Invalid file path given!", file.path(), error_code(EINVAL, std::generic_category()));
        }

        ifstream inStream(file.path());
        string fContents{};

        inStream >> fContents;

        fromString(fContents, outLang);
    }

    void language::fromString(const string& fContents, language& outLang) {
        vector<string> tokens;
        if (!extensions::splitString(fContents, "\n", tokens)) {
            throw std::runtime_error("Failed to split input string! Are newlines missing?");
        }

        outLang.clear();

        for (const auto& line : tokens) {
            outLang.parseLine(line);
        }
    }

    language::language():
        m_langDescription({}), m_langId({}),
        m_langVer(), m_translationDict({})
        { }

    bool language::addVarExpansionCallback(const string& varName, const varexpansioncallback_t& cb) {
        return _callbackList.try_emplace(varName, static_cast<varexpansioncallback_t>(cb)).second;
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
        if (!std::regex_match(extensions::trim(line), regex(SECTION_REGEX.data(), rc::optimize))) { return false; }

        outSection = extensions::trim(line, "[]");
        return true;
    }

    bool language::isTranslation(const string& line, string& outFieldName, string& outTranslation) const {
        namespace rc = std::regex_constants;
        if (!std::regex_match(line, regex(TRANSLATION_REGEX.data(), rc::optimize))) { return false; }

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

    optstr_t language::getString(const string& section, const string& field, bool expandVariables /*= true*/) const {
        const auto sect = getSection(section);
        if (!sect.has_value()) { return {}; }

        if (sect.value().find(field) == sect.value().end()) { return {}; }

        return {};
    }

    string language::expandVariable(const string& varName) const {
        return {};
    }

    string language::removeInlineComments(const string& line) const {
        namespace rc = std::regex_constants;
        static const regex COMMENT_REGEX = regex(R"(#[^\n]+$)", rc::optimize);

        string copy = line;
        smatch matches;
        if (std::regex_search(copy, matches, COMMENT_REGEX)) {
            for (const auto& match : matches) {
                copy = copy.replace(copy.find(match.str()), match.str().length(), "");
            }
        }

        return extensions::trim(copy);
    }

    void language::clear() {
        m_langDescription = {};
        m_langId = {};
        m_langVer = {};
        m_translationDict.clear();
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

    void language::removeVarExpansionCallback(const string& varName) {
        const auto iterPos = _callbackList.find(varName);

        if (iterPos == _callbackList.end()) { return; } // fail silently
        _callbackList.erase(iterPos);
    }

}