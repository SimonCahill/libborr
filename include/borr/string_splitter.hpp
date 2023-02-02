/**
 * @file string_splitter.hpp
 * @author Simon Cahill (simon@simonc.eu)
 * @brief Contains the full implementation of an iterator-based string splitter based on an answer provided by StackOverflow user Calmarius.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022 Simon Cahill and Contributors
 */

#ifndef FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP
#define FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP

#include <string>

using std::string;
using std::string_view;

/**
 * @brief Splits a string into tokens, usable in an iterator loop, based on a delimiter.
 */
struct StringSplit {

    /**
     * @brief Simple implementation of an iterator usable in a iterative for-loop.
     */
    struct Iterator {
        private:
            size_t m_tokenStart = 0;
            size_t m_tokenEnd = 0;
            string m_input;
            string_view m_inputView;
            string m_delimiter;
            bool m_done = false;

        public:
            /**
             * @brief Constructs an empty iterator which indicates the end of an iterator.
             */
            Iterator() { m_done = true; }

            /**
             * @brief Constructs a new instance of this iterator.
             * 
             * @param input The input string.
             * @param delimiter The delimiter to split the string by.
             */
            Iterator(const string& input, const string& delimiter):
            m_input(std::move(input)), m_inputView(m_input), m_delimiter(std::move(delimiter)) {
                m_tokenEnd = m_inputView.find(m_delimiter, m_tokenStart);
            }

            /**
             * @brief Dereference operator overload.
             * 
             * @return string_view Returns a substring of the input marked by the current iterator position.
             */
            string_view operator*() { return m_inputView.substr(m_tokenStart, m_tokenEnd - m_tokenStart); }

            /**
             * @brief Increments the iterator's position by 1.
             * 
             * @return Iterator& A reference to the current instance.
             */
            Iterator& operator++() {
                if (m_tokenEnd == string::npos) {
                    m_done = true;
                    return *this;
                }

                m_tokenStart = m_tokenEnd + m_delimiter.size();
                m_tokenEnd = m_inputView.find(m_delimiter, m_tokenStart);
                return *this;
            }

            /**
             * @brief Not-equal operator overload. Determines whether two @see Iterator instances are inequal.
             * 
             * @param other A const reference to a different @see Iterator.
             * 
             * @return true If the two instances are inequal.
             * @return false Otherwise.
             * 
             * @remarks This only checks if both iterators point to the end of the iterator!
             */
            bool operator!=(const Iterator& other) { return m_done && other.m_done; }
    };

    private:
        Iterator m_iteratorBeginning; //!< The beginning iterator

    public:
        /**
         * @brief Constructs a new instance of @see StringSplit
         * 
         * @param input The input string to be split into tokens.
         * @param delim The delimiter by which to split the strings.
         */
        StringSplit(const string& input, const string& delim): m_iteratorBeginning{std::move(input), std::move(delim)} { }

        /**
         * @brief Gets the beginning of the iterator.
         * 
         * @return Iterator The beginning iterator (position)
         */
        Iterator begin() { return m_iteratorBeginning; }

        /**
         * @brief Returns the end of the iterator.
         * 
         * @return Iterator The end iterator.
         */
        Iterator end() { return Iterator{}; }
};

#endif // FAIL2ABUSEIPDB_INCLUDE_STRING_SPLITTER_HPP