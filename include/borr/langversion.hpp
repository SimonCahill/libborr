/**
 * @file langversion.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of a simple struct for containing and presenting language versions.
 * @version 0.1
 * @date 2023-01-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#ifndef LIBBORR_INCLUDE_BORR_LANGVERSION_HPP
#define LIBBORR_INCLUDE_BORR_LANGVERSION_HPP

#include <string>
#include <sstream>
#include <vector>

#include "borr/extensions.hpp"

namespace borr {

    using std::string;
    using std::vector;

    /**
     * @brief A simple class containing the version of a language.
     */
    class langversion {
        public: // +++ Static +++
            /**
             * @brief Emplaces all values from a given string.
             * 
             * @param verField The translation field from which to retrieve the values.
             * @param outVersion Out parameter containing the updated version.
             */
            static void fromString(const string& verField, langversion& outVersion) {
                vector<string> versionTokens{};
                if (!extensions::splitString(verField, ".", versionTokens, 3)) {
                    throw std::runtime_error("Failed to parse version string! Please ensure version structure is num.num.num!");
                }

                outVersion.m_major = std::stoull(versionTokens.at(0));
                outVersion.m_minor = std::stoull(versionTokens.at(1));
                outVersion.m_revision = std::stoull(versionTokens.at(2));
            }

        public: // +++ Constructor / Destructor +++
            langversion(): m_major(string::npos), m_minor(string::npos), m_revision(string::npos) {}
            explicit langversion(const langversion& x): m_major(x.m_major), m_minor(x.m_minor), m_revision(x.m_revision) {}
            ~langversion() = default; //!< Default dtor

        public: // +++ Getters +++
            size_t      getMajorVersion()   const { return m_major; }
            size_t      getMinorVersion()   const { return m_minor; }
            size_t      getRevision()       const { return m_revision; }

            string      operator*() const {
                std::stringstream verString;
                verString << "v" << m_major << "." << m_minor << "." << m_revision;

                return verString.str();
            }

        private:
            size_t  m_major;
            size_t  m_minor;
            size_t  m_revision;
    };

}

#endif // LIBBORR_INCLUDE_BORR_LANGVERSION_HPP