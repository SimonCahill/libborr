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
#include <chrono>
#if __cpp_lib_format >= 201907L
#   include <format>
#else
#   include <fmt/format.h>
#endif
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "borr/extensions.hpp"
#include "borr/language.hpp"
#include "borr/langversion.hpp"
#include "borr/resources.hpp"
#include "borr/string_splitter.hpp"

namespace borr {

    namespace fs = std::filesystem;

    using std::error_code;
    using std::regex;
    using std::regex_replace;
    using std::smatch;
    using std::stringstream;
    using std::vector;

    varcbacklist_t language::_callbackList = {}; //!< Initialise private static member

    varcbacklist_t language::_defaultExpandersList = {
        { "date", dateExpander },
        { "time", timeExpander },
        { "lib", libExpander },
        { "os", osExpander },
        { "liburl", liburlExpander },
    };

    /**
     * @brief Parses a borrfile and ensures its contents are parsed into the given object reference.
     * 
     * @param file The file to parse.
     */
    language language::fromFile(const fs::directory_entry& file) {
        if (!file.exists() || !file.is_regular_file()) {
            throw fs::filesystem_error("Invalid file path given!", file.path(), error_code(EINVAL, std::generic_category()));
        }

        ifstream inStream(file.path());
        stringstream fContents{};

        fContents << inStream.rdbuf();

        return fromString(fContents.str());
    }

    /**
     * @brief Parses a string object into a language instance.
     * 
     * @param fContents The string (file contents) to parse.
     * 
     * @throws runtime_error If an error occurred. TODO: Custom exceptions.
     */
    language language::fromString(const string& fContents) {
        language outLang{};
        vector<string> tokens;
        if (!extensions::splitString(fContents, "\n", tokens)) {
            throw std::runtime_error("Failed to split input string! Are newlines missing?");
        }

        outLang.clear();

        for (const auto& line : tokens) {
            outLang.parseLine(line);
        }

        return outLang;
    }

    /**
     * @brief Default constructor.
     */
    language::language() { }

    /**
     * @brief Allows a user to add custom variable expansion callbacks.
     * 
     * @param varName The name of the variable this callback should expand.
     * @param cb The actual callback itself.
     * 
     * @return true If the callback was added successfully.
     * @return false Otherwise.
     */
    bool language::addVarExpansionCallback(const string& varName, const varexpansioncallback_t& cb) {
        return _callbackList.try_emplace(varName, static_cast<varexpansioncallback_t>(cb)).second;
    }

    /**
     * @brief Determines whether or not a given translation string contains a variable.
     * 
     * @remarks This member function may be called multiple times on a given string.
     * 
     * @param translation The translation string.
     * @param outVarName The name of the found variable.
     * 
     * @return true If the string contains a variable.
     * @return false Otherwise.
     */
    bool language::containsVariable(const string& translation, string& outVarName) const {
        namespace rc = std::regex_constants;
        const static regex VAR_REGEX(VARIABLE_REGEX.data(), rc::optimize);

        smatch matches;
        if (std::regex_search(translation, matches, VAR_REGEX)) {
            outVarName = extensions::trim(extensions::trim(*matches.begin(), "$"), "{}");
            return true;
        }

        return false;
    }

    /**
     * @brief Determines whether a line is empty or is commented out.
     * 
     *
     * @code 
     * # this would count as an empty line
     * [section] # this does not
     * @endcode
     * 
     * @param line The line to check.
     * 
     * @return true If the line is empty or commented out.
     * @return false Otherwise.
     */
    bool language::isEmptyOrComment(const string& line) const {
        return extensions::trim(line).empty() || extensions::trim(line).at(0) == '#';
    }

    /**
     * @brief Detemines whether or not a given field supports multiple lines.
     * 
     * @remarks Multiline fields are transparent to the caller when using @c getString @endcode!
     * 
     * @param field The field to check.
     * 
     * @return true If the field support multiline strings.
     * @return false Otherwise.
     */
    bool language::isMultilineField(const string& field) const {
        return *field.rbegin() == ']' && *(field.rbegin() + 1) == '[';
    }

    /**
     * @brief Determines whether or not a given line declares a new section or not.
     * 
     * @param line The line to check.
     * @param outSection Output variable to contain the section name.
     * 
     * @return true If the line contains a section declaration.
     * @return false Otherwise.
     */
    bool language::isSection(const string& line, string& outSection) const {
        namespace rc = std::regex_constants;
        if (!std::regex_match(extensions::trim(line), regex(SECTION_REGEX.data(), rc::optimize))) { return false; }

        outSection = extensions::trim(line, "[]");
        return true;
    }

    /**
     * @brief Determines whether or not a given line contains a translation value or not.
     * 
     * @param line The line to check.
     * @param outFieldName The name of the translation.
     * @param outTranslation The translation contents.
     * 
     * @return true If the line contains a translation string.
     * @return false Otherwise.
     */
    bool language::isTranslation(const string& line, string& outFieldName, string& outTranslation) const {
        namespace rc = std::regex_constants;
        if (!std::regex_match(line, regex(TRANSLATION_REGEX.data(), rc::optimize))) { return false; }

        // Now just split the string at the first '=' and return the two halfs
        const auto posOfDelim = line.find('=');

        outFieldName = extensions::trim(line.substr(0, posOfDelim - 1));
        outTranslation = extensions::trim(line.substr(posOfDelim + 1), "\" \t\r");
        return true;
    }

    /**
     * @brief Gets an entire section containing translations.
     * 
     * @remarks Variables are @b NOT expanded!
     * 
     * @param sectionName The name of the section to retrieve.
     * 
     * @return optsect_t An optional<map<string, string>> which either contains the section or is nullopt if section was not found.
     */
    optsect_t language::getSection(const string& sectionName) const {
        const auto iterPos = m_translationDict.find(sectionName);

        if (iterPos == m_translationDict.end()) { return {}; }

        return iterPos->second;
    }

    /**
     * @brief Gets a single string from the translation table.
     * 
     * @remarks Variables @b ARE expanded here if expandVariables is true!
     * 
     * @param section The name of the section in which to search for the translation.
     * @param field The name of the translation you're looking for.
     * @param expandVariables Whether or not to expand variables (default: true)
     * 
     * @returns An optional<string> which contains the translation or nullopt, depending on whether the translation was found or not.
     */
    optstr_t language::getString(const string& section, const string& field, bool expandVariables /*= true*/) const {
        const auto sect = getSection(section);
        if (!sect.has_value()) { return {}; }

        if (const auto iterPos = sect.value().find(field); iterPos != sect.value().end()) {
            auto translation = iterPos->second;
            string varName{};
            while (containsVariable(translation, varName)) {
                extensions::stringReplace(translation, "${" + varName + "}", expandVariable(varName));
            }

            return translation;
        }

        return {};
    }

    /**
     * @brief Expands a given variable if a possible expander was found.
     * 
     * @remarks
     * Custom expanders overwrite default expanders!
     * If no expansion was found, the result will be an empty string.
     * 
     * @param varName The name of the variable to expand.
     * 
     * @return string The expanded variable - or an empty string if no expander was found.
     */
    string language::expandVariable(const string& varName) const {
        // first check if custom var expanders were set up
        if (const auto iterPos = _callbackList.find(varName); iterPos != _callbackList.end()) {
            return iterPos->second(varName);
        }

        if (const auto iterPos = _defaultExpandersList.find(varName); iterPos != _defaultExpandersList.end()) {
            const auto value = iterPos->second(varName);
            return value;
        }

        // now check if the variable references a different translation
        if (const auto iterPos = varName.find(':'); iterPos != string::npos) {
            vector<string> tokens;
            if (!extensions::splitString(varName, ":", tokens, 2) || tokens.size() != 2) { return {}; }

            return getString(tokens[0], tokens[1]).value_or("");
        }

        return {};
    }

    /**
     * @brief Removes any inline comemnts found in a string.
     * 
     * @param line The line to check.
     * 
     * @return string The stripped string.
     */
    string language::removeInlineComments(const string& line) const {
        namespace rc = std::regex_constants;
        static const regex COMMENT_REGEX = regex(R"(#[^\n]+[^"]$)", rc::optimize);

        string copy = line;
        smatch matches;
        if (std::regex_search(copy, matches, COMMENT_REGEX)) {
            for (const auto& match : matches) {
                copy = copy.replace(copy.find(match.str()), match.str().length(), "");
            }
        }

        return extensions::trim(copy);
    }

    /**
     * @brief Clears all variables and translation tables.
     */
    void language::clear() {
        m_langDescription = {};
        m_langId = {};
        m_langVer = {};
        m_currentSection = {};
        m_translationDict.clear();
    }

    /**
     * @brief Parses a single line from a borrfile.
     * 
     * @param line The line to parse.
     */
    void language::parseLine(const string& line) {
        if (isEmptyOrComment(line)) { return; }

        // now search for inline comments
        const auto commentlessLine = removeInlineComments(line);

        if (isSection(commentlessLine, m_currentSection)) { return; } // nothing more to do here

        string field;
        string translation;
        if (!isTranslation(commentlessLine, field, translation)) { return; } // nothing more to do here

        if (m_currentSection.empty()) {
            if (field == LANG_DESC_FIELD) {
                printf("Found LANG_DESC_FIELD\n");
                m_langDescription = translation;
                return;
            } else if (field == LANG_ID_FIELD) {
                printf("Found LANG_ID_FIELD\n");
                m_langId = translation;
                return;
            } else if (field == LANG_VER_FIELD) {
                printf("Found LANG_VER_FIELD\n");
                langversion::fromString(translation, m_langVer);
                return;
            }
            return;
        }

        if (!getSection(m_currentSection).has_value()) {
            m_translationDict.emplace(m_currentSection, sect_t{});
        }

        auto& section = m_translationDict[m_currentSection];

        const auto trimmedField = extensions::trim(field, "[]");

        if (section.find(trimmedField) != section.end() && isMultilineField(field)) {
            section[trimmedField] = section[trimmedField] + "\n" + translation;
        } else {
            section.emplace(trimmedField, translation);
        }
    }

    /**
     * @brief Removes a translation expander from the list of expanders.
     * 
     * @param varName The variable name for which to remove the expander.
     */
    void language::removeVarExpansionCallback(const string& varName) {
        const auto iterPos = _callbackList.find(varName);

        if (iterPos == _callbackList.end()) { return; } // fail silently
        _callbackList.erase(iterPos);
    }

    #pragma region "Default Expanders"
    /**
     * @brief Expands "date" to the current (local) date.
     * 
     * @remarks This needs to be changed to a pure C++ system once std::format is widely available on macOS/clang.
     * At this current point in time, std::format() is experimental in clang C++14.
     * 
     * @return string The current (local) date as "Y-m-d".
     */
    string language::dateExpander(const string&) {
        const auto timeNow = time(nullptr);
        tm* localTime = localtime(&timeNow);

        stringstream sStream;
        sStream << (localTime->tm_year + 1900) << "-" << localTime->tm_mon << "-" << localTime->tm_mday;

        return sStream.str();
    }

    /**
     * @brief Expands the "time" variable to the current (local) time.
     * 
     * @remarks
     * This needs to be changed to a pure C++ system once std::format is widely available on macOS/clang.
     * 
     * @return string The current (local) time as "hh:MM:ss".
     */
    string language::timeExpander(const string&) {
        const auto timeNow = time(nullptr);
        tm* localTime = localtime(&timeNow);

        stringstream sStream;
        sStream << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec;

        return sStream.str();
    }

    /**
     * @brief Expands the "lib" variable.
     * 
     * @return string The name of the library used.
     */
    string language::libExpander(const string&) {
        return resources::getBorrDescription() + " v" + resources::getBorrVersion();
    }

    /**
     * @brief Expands the OS variable.
     * 
     * @remarks The variable expansion is determined at compile time.
     * 
     * @return string The name of the current OS.
     */
    string language::osExpander(const string&) {
        return resources::getOperatingSystemName();
    }

    /**
     * @brief Expands the "liburl" variable.
     * 
     * @return string The URL to the repository of the library.
     */
    string language::liburlExpander(const string&) {
        return resources::getLibUrl();
    }

    #pragma endregion

}