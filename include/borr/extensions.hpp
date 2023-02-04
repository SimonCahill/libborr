/**
 * @file extensions.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains useful functions which aren't provided in the STL.
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#ifndef LIBBORR_INCLUDE_BORR_EXTENSIONS_HPP
#define LIBBORR_INCLUDE_BORR_EXTENSIONS_HPP

#include <functional>
#include <string>
#include <vector>

namespace borr::extensions {

    using std::function;
    using std::string;
    using std::vector;


    /**
     * @brief Splits a string by the givn delimiters.
     * 
     * @param str The string to split.
     * @param delimiters The delimiters to split the string by.
     * @param outTokens The output tokens.
     * @param maxLen The maximum amount of tokens.
     * 
     * @return true If splitting was successful.
     * @return false Otherwise.
     */
    inline bool splitString(const string& str, const string& delimiters, vector<string>& outTokens, size_t maxLen = SIZE_MAX) {
        // Skip delimiters at beginning.
        size_t lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        size_t pos = str.find_first_of(delimiters, lastPos);

        while ((string::npos != pos || string::npos != lastPos) && maxLen > outTokens.size()) {
            const auto token = str.substr(lastPos, pos - lastPos);

            if (!token.empty()) {
                // Found a token, add it to the vector.
                outTokens.push_back(token);
            }

            // Skip delimiters.
            lastPos = string::npos == pos ? string::npos : pos + 1;
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }

        return outTokens.size() > 0;
    }

    /**
     * @brief Trims the beginning of a given string.
     *
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trimStart(string nonTrimmed, const string& trimChar = " \t\r") {
        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };

        nonTrimmed.erase(nonTrimmed.begin(), find_if(nonTrimmed.begin(), nonTrimmed.end(), std::not_fn(shouldTrimChar)));

        return nonTrimmed;
    }

    /**
     * @brief Trims the end of a given string.
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trimEnd(string nonTrimmed, const string& trimChar = " \t\r") {
        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };
        nonTrimmed.erase(find_if(nonTrimmed.rbegin(), nonTrimmed.rend(), std::not_fn(shouldTrimChar)).base(), nonTrimmed.end());

        return nonTrimmed;
    }

    /**
     * @brief Trims both the beginning and the end of a given string.
     *
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trim(const string& nonTrimmed, const string& trimChar = " \t\r") { return trimStart(trimEnd(nonTrimmed, trimChar), trimChar); }

}

#endif // LIBBORR_INCLUDE_BORR_EXTENSIONS_HPP