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
     * @brief Trims the beginning of a given string.
     *
     * @param nonTrimmed The non-trimmed string.
     * @param trimChar The character to trim off the string. (default: space)
     *
     * @return The trimmed string.
     */
    inline string trimStart(string nonTrimmed, const string& trimChar = " \t\r") {
        //nonTrimmed.erase(nonTrimmed.begin(), find_if(nonTrimmed.begin(), nonTrimmed.end(), not1(ptr_fun<int32_t, int32_t>(isspace))));

        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };

        nonTrimmed.erase(nonTrimmed.begin(), find_if(nonTrimmed.begin(), nonTrimmed.end(), not1(shouldTrimChar)));

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
        // nonTrimmed.erase(find_if(nonTrimmed.rbegin(), nonTrimmed.rend(), not1(ptr_fun<int32_t, int32_t>(isspace))).base(), nonTrimmed.end());

        function<bool(char)> shouldTrimChar = [=](char c) -> bool { return trimChar.size() == 0 ? isspace(c) : trimChar.find(c) != string::npos; };
        nonTrimmed.erase(find_if(nonTrimmed.rbegin(), nonTrimmed.rend(), not1(shouldTrimChar)).base(), nonTrimmed.end());

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