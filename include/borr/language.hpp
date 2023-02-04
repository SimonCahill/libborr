/**
 * @file language.hpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the declaration of a simple language parser and management class.
 * @version 0.1
 * @date 2023-01-27
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#ifndef LIBBORR_INCLUDE_BORR_LANGUAGE_HPP
#define LIBBORR_INCLUDE_BORR_LANGUAGE_HPP

/////////////////////
// SYSTEM INCLUDES //
/////////////////////
// stl
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>

/////////////////////
// LOCAL  INCLUDES //
/////////////////////
#include "langversion.hpp"

/**
 * @brief Root namespace of the libborr.
 */
namespace borr {

    namespace fs = std::filesystem;

    using std::ifstream;
    using std::map;
    using std::optional;
    using std::string;
    using std::string_view;


    using sect_t = map<string, string>;
    using optsect_t = optional<sect_t>;
    using dict_t = map<string, sect_t>;
    using translation_t = std::optional<string>;
    using ver_t = langversion;
    using optstr_t = optional<string>;

    #ifndef UNIT_TESTING
    class LanguageClassTests;
    #endif // UNIT_TESTING

    /**
     * @brief The language class - a language manager and file parser.
     * 
     * This class represents a single language which is parsed from a language file (typically *.borr or *.lang).
     * A language file is a psuedo-ini format which allows for comments, multi-line strings and variable (string) replacement.
     * 
     * Before a language file switches from being a simple ini file to a language file,
     * it must meet the following criteria:
     * 
     * In the main scope:
     *  - A lang_id field must be present, with the language ID (en_EN, en_US, en_AU or what have you)
     *  - A lang_desc field must be present, although it may be left blank
     *  - A lang_ver field must be present, containing the language file's version (semantic version: MAJOR.MINOR.REVISION).
     * Other values in the main scope will be ignored by the parser!
     * 
     * Language files can contain different structures which allow them to be flexible and usable in a variety of domains.
     * Here is a list of all structures a language file can (or sometimes must) contain for it to be valid.
     * 
     * | Structure  | Description                                   | Example                                   |
     * |------------|-----------------------------------------------|-------------------------------------------|
     * | lang_desc  | The language description description field.   | lang_desc = "English (United Kingdom)"    |
     * | lang_id    | The language ID with region and country.      | lang_id = "en_GB"                         |
     * | lang_ver   | The language file's current version.          | lang_ver = "v1.0.0" (v is optional!)      |
     * | #          | A comment. Comments can start anywhere.       | # this is a comment in my langfile        |
     * | [section]  | A new language section; for pages or menus.   | [home_page] # translations for home page  |
     * | field      | A field is a string container                 | page_title = "My Home Page!"              |
     * | field[]    | A multi-line field.                           | about[] = "This is an example of multi-"  |
     * | field[]    | A multi-line field.                           | about[] = "line translation fields."      |
     * | ${}        | A variable; used for text replacement.        | awesome = "${page_title} Is Awesome!"     |
     * | ${}        | A special variable; used for text replacement.| copyright = "© ${year}"                   |
     * 
     * libborr provides a plethora of features for parsing and using borr files and is extensible by use of modern C++ features, such as lambdas.
     * 
     * Here is an example of using libborr for parsing your language files.
     * 
     * @code
     * // en_GB.borr
     * 
     * # Language file for British English
     * lang_id = "en_GB"
     * lang_ver = "v1.0.0"
     * lang_desc = "British English translations for My Awesome App!"
     * 
     * [start_page]
     * page_title = "Start Here!"
     * my_button = "Click me!"
     * 
     * [about_page]
     * page_title = "About ${app_name}"
     * about_text[] = "This is my awesome app, written with love!"
     * about_text[] = "© ${year} Me" # This would be line 2
     * about_text[] = "Code for \"${start_page:my_button}\" button copied from StackOverflow"
     * 
     * // de_DE.borr
     * 
     * # Language file for British English
     * lang_id = "de_DE"
     * lang_ver = "v1.0.0"
     * lang_desc = "Deutsche Übersetzung für Meine Coole App!"
     * 
     * [start_page]
     * page_title = "Hier geht's los!"
     * my_button = "Klick mich!"
     * 
     * [about_page]
     * page_title = "Über ${app_name}"
     * about_text[] = "Dies ist meine coole App; mit Liebe geschrieben!"
     * about_text[] = "© ${year} Me" # This would be line 2
     * about_text[] = "Code für \"${start_page:my_button}\"-Knopf von StackOverflow kopiert"
     * @endcode
     * 
     * With those two files, you could call something like this from within your application:
     * 
     * @code
     * void loadLanguages() {
     *      const auto enLangFile = fs::directory_entry("./languages/en_EN.borr");
     *      const auto deLangContents = readFile("./languages/de_DE.borr");
     * 
     *      const auto enLang = language::fromFile(enLangFile);
     *      const auto deLang = language::fromString(deLangContents);
     * 
     *      // do whatever
     * 
     *      // to get translation
     *      const auto deStartPageTitle = deLang.getString("start_page", "page_title"):
     *      const auto deStartButtonText = deLang.getString("start_page", "my_button");
     * 
     *      // do whatever with strings
     * } 
     * @endcode
     * 
     * And that's basically the main API. It's rare you'll need much more than that.
     */
    class language {
        // #ifdef UNIT_TESTING // I'm too dumb for this right now
        // #warning "Unit tests are enabled!"
        // private:
        // friend class LanguageClassTests;
        // #endif // UNIT_TESTING

        public: // +++ Static Const +++
            static constexpr string_view LANG_ID_FIELD = "lang_id";
            static constexpr string_view LANG_VER_FIELD = "lang_ver";
            static constexpr string_view LANG_DESC_FIELD = "lang_desc";
            static constexpr string_view VARIABLE_REGEX = R"(\$\{[\s\S]\})";
            static constexpr string_view SECTION_REGEX = R"(^\[[A-z_]([A-z_]+)?\]$)";
            static constexpr string_view TRANSLATION_REGEX = R"(^[A-z_][A-z0-9_]+(\[\])?[\s]+?=[\s]+?"([^"]+)?"$)";

        public: // +++ Static +++
            static void fromFile(const fs::directory_entry&, language&); //!< Load a language from disk
            static void fromString(const string&, language&); //!< Load a pre-loaded language file from memory

        public: // +++ Constructor / Destructor +++
                            // language(const language&) = default; //!< Default copy ctor
            ~               language() = default; //!< Default dtor

        public: // +++ Getters +++
            optsect_t       getSection(const string&) const; //!< Gets a complete translation section. No variables are expanded!
            optstr_t        getString(const string&, const string&, bool expandVariables = true) const; //!< Gets a single translation with optional variable expansion

            const ver_t&    getLanguageVersion() const { return m_langVer; }
            const string&   getLangId() const { return m_langId; }
            const string&   getLangDescription() const { return m_langDescription; }

        public: // +++ Constructor ++
            language(); //!< Protected default ctor

        protected: // +++ Actual Parsing +++
            virtual bool    isEmptyOrComment(const string&) const; //!< Determines whether or not the current line is empty or a comment so it can be ignored.
            virtual bool    isMultilineField(const string&) const; //!< Determines whether or not a field name is multiline or not
            virtual bool    isSection(const string&, string& outSectName) const; //!< Determines whether the current line is a section or not
            virtual bool    isTranslation(const string&, string& outFieldName, string& outTranslation) const; //!< Determines whether the current line is a translation or not

            virtual string  removeInlineComments(const string&) const;

            virtual void    clear();
            virtual void    parseLine(const string&);

        protected: // +++ Translation retrieval +++
            virtual bool    containsVariable(const string&, string& outVarName) const;

            virtual string  expandVariable(const string&) const;

        private:
            dict_t          m_translationDict; //!< The translation dictionary containing sections and translations

            langversion     m_langVer; //!< The language file's version

            string          m_langId; //!< The language's ID (region_COUNTRY)
            string          m_langDescription; //!< The language's description
    };

}

#endif // LIBBORR_INCLUDE_BORR_LANGUAGE_HPP