/**
 * @file Main.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the reference implementation (usage) for libborr.
 * @version 0.1
 * @date 2023-02-05
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors
 */

#include <borr/language.hpp>

#include <exception>
#include <iostream>
#include <string>

#include <getopt.h>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::string;

using borr::language;

namespace fs = std::filesystem;

const string SHORT_OPTS = R"(hl:)";
const option LONG_OPTS[] = {
    { "help",   no_argument,        nullptr,    'h' },
    { "lang",   required_argument,  nullptr,    'l' },
    { nullptr,  no_argument,        nullptr,     0  }
};

void printHelp(const string&);

int main(int32_t argc, char** argv) {
    int32_t currentOpt = 0;

    string langFile{};

    while ((currentOpt = getopt_long(argc, argv, SHORT_OPTS.c_str(), LONG_OPTS, nullptr)) != -1) {
        switch (currentOpt) {
            case 'h':
                printHelp(argv[0]);
                return 0;
            case 'l':
                if (!optarg) {
                    cerr << "Missing lang file!" << endl;
                    return 1;
                }
                langFile = optarg;
                break;
            default:
                cerr << "Unknown option " << static_cast<char>(currentOpt) << endl;
                printHelp(argv[0]);
                return 1;
        }
    }

    if (langFile.empty()) {
        cerr << "No borrfile passed!" << endl;
        printHelp(argv[0]);
        return 1;
    }

    language lang;
    try {
        language::fromFile(fs::directory_entry(langFile), lang);
    } catch (const exception& ex) {
        cerr << "Failed to parse language file: " << ex.what() << endl;
        return 1;
    }

    cout << "Selected language: " << lang.getLangId() << endl
         << "Language description: " << lang.getLangDescription() << endl
         << "Language version: " << *lang.getLanguageVersion() << endl << endl;

    auto section = lang.getSection("variables_tests").value();
    cout << R"(Reading variables from section "variables_tests":)" << endl << endl;
    for (const auto field : section) {
        cout << "Found translation (" << field.first << "): " << lang.getString("variables_tests", field.first).value_or("not found") << endl;
    }
    cout << endl << endl;

    cout << "Copyright and licensing information:" << endl
         << lang.getString("normal_tests", "copyright_info").value_or("copyright info not found :c")
         << endl << endl;
}

void printHelp(const string& bin) {
    cout << "Usage: " << bin << " -h" << endl
         << "Usage: " << bin << "-l<borrfile>" << endl << endl
         << "Arguments:" << endl
         << "\t--help, -h\t\tDisplays this menu and exits" << endl
         << "\t--lang, -l<langfile>\tParse the langfile" << endl;
}