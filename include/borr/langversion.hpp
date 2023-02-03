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

namespace borr {

    using std::string;

    /**
     * @brief A simple class containing the version of a language.
     */
    class langversion {
        public: // +++ Static +++
            static void fromString(const string& verField, langversion& outVersion) {
                size_t previousPeriod = 0;
                size_t nextPeriod = string::npos;

                while ((nextPeriod = verField.find('.', previousPeriod)) != string::npos) {
                    if (previousPeriod >= verField.length() - 1) { previousPeriod = string::npos; }

                    if (outVersion.m_major == string::npos) {
                        outVersion.m_major = std::stoull(verField.substr(previousPeriod, nextPeriod - previousPeriod));
                    } else if (outVersion.m_minor == string::npos) {
                        outVersion.m_minor = std::stoull(verField.substr(previousPeriod, nextPeriod - previousPeriod));
                    } else if (outVersion.m_revision == string::npos) {
                        outVersion.m_revision = std::stoull(verField.substr(previousPeriod, nextPeriod - previousPeriod));
                    } else { return; } // fail silently

                    previousPeriod = nextPeriod + 1;
                }
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