# libborr

libborr - named after the Norse god of language and speech - is a cross-platform translation provider written in C++.
libborr uses a custom, ini-based file format which contains meta information about the translations and language within.

# STATUS
This library is currently in its beta phase and may be subject to breaking changes!

# Supported Features

libborr supports splitting translations into sections, allowing for the same translation name to be used across different portions of your application.
It also allows for meta-information to be stored for each language; some of which is required for the parser:

| Required field    | Value                                 | Example                               |
|-------------------|---------------------------------------|---------------------------------------|
| lang_id           | The ID of the language as lang_REGION | lang_id = "en_GB"                     |
| lang_ver          | The version of the language file      | lang_ver = "1.0.0"                    |
| lang_desc         | A brief description of the language   | lang_desc = "The English translation" | 

Other, custom fields, may be added at your will and requirement.
These will be stored under the section `""` and can be retrieved by using `lang.getString("", "your_custom_field");`.

What you do with these values is entirely up to your application.
Failing in providing the required fields will result in a failed parse.

## Sections

A section in a borr file splits translations from one another. This is based in the ini-format.
A section may contain an arbitrary number of fields, containing translations.

A section must consist of text surrounded by square brackets:; `[section]`.
The text within the brackets must meet the following criteria:
 - begin with a lower- or uppercase letter or underscore
 - may be an arbitrary length
 - may contain any of the following characters: `A-z 0-9 _`

Sections not meeting this criteria will not be recognised and ignored.
This behaviour is by design and **not a bug!**

```borrfile
[my_section]
my_field = "My translation"

[another_section]
my_field = "A different translation"
```

## Fields
libborr supports two separate types of fields; each field is however a `string`.

### single-line fields
Single-line fields are likely the most common type of field you'll use.
One field consists of a single line of text, but supports a myriad of special characters.

```borrfile
my_field = "a single line"
```

### multi-line fields
libborr also supports multi-line fields for use in your application.
A multiline field is similar in structure to single-line fields, suffixed with a `[]` at the end.

Although the fields are separate in the file, they will be concatenated within the parser.
Multiline fields may also be separated by other fields.

```borrfile
my_multiline_field[] = "Line 1"
my_multiline_field[] = "Line 2"
my_multiline_field[] = "Line 3"
singleline_field     = "Some other translation"
my_multiline_field[] = "Line 4"
```

## Variables
libborr supports the use of variables - which are text replacements which happen during runtime.

Each application using libborr can define custom variable expanders which can be set across all languages loaded.


```borrfile
[translations_w_variables]
my_multiline_field[] = "${myCustomVar} is a custom-expanded variable."
my_multiline_field[] = "libborr also provides default expansions, such as ${date}"
my_multiline_field[] = "${time}."
reference_to_other_field = "Libborr can reference other translations and field. ${other_translations:some_field} like this!"

[other_translations]
some_field = "Something"
```

### Setting a custom expander
A custom expander is a C++ function object which returns an `std::string` object and takes a `const string&` as a function parameter.

Custom expanders are searched before searching for default expanders and other translations in (potentially) other sections.

```cpp
// setting a custom expander
borr::language::addVarExpansionCallback(
     "myCustomVar", [](const string& var) {
          // var is the trimmed name of the variable
          // in the borrfile, the variable is written ${myCustomVar},
          // var thus contains "myCustomVar"
          return "Some computed or otherwise dynamic expression/text";
     }
);

// removing a custom expander
borr::language::removeVarExpansionCallback("myCustomVar");
```

Default variable expanders can also be overwritten by using the above method.
If the default expansion behaviour for `${date}` doesn't suit your needs, then it can be overridden by your application.

## File Structure
As allured to at the beginning, borrfiles require a specific structure.

This section describes the file structure and provides examples.

| Structure  | Description                                   | Example                                   |
|------------|-----------------------------------------------|-------------------------------------------|
| lang_desc  | The language description description field.   | lang_desc = "English (United Kingdom)"    |
| lang_id    | The language ID with region and country.      | lang_id = "en_GB"                         |
| lang_ver   | The language file's current version.          | lang_ver = "v1.0.0" (v is optional!)      |
| #          | A comment. Comments can start anywhere.       | # this is a comment in my langfile        |
| [section]  | A new language section; for pages or menus.   | [home_page] # translations for home page  |
| field      | A field is a string container                 | page_title = "My Home Page!"              |
| field[]    | A multi-line field.                           | about[] = "This is an example of multi-"  |
| field[]    | A multi-line field.                           | about[] = "line translation fields."      |
| ${}        | A variable; used for text replacement.        | awesome = "${page_title} Is Awesome!"     |
| ${s:f}     | A special variable; used for text replacement.| field = "© ${home_page:page_title}        |

### Example
An example file can be found in [here](https://github.com/SimonCahill/libborr/blob/master/reference/resources/en_GB.borr).

## Library Usage

A reference implentation can be found [here](https://github.com/SimonCahill/libborr/blob/master/reference/src/Main.cpp).

But here are some examples for a quick look.

### Parsing a file

```cpp
#include <borr/language.hpp>

#include <ifstream>
#include <filesystem>
#include <sstream>

void loadFromFile() {
     borr::language enLang;

     namespace fs = std::filesystem;
     try {
          borr::language::fromFile(fs::directory_entry("/path/to/en_GB.borr", enLang));
     } catch (const std::exception& ex) {
          // handle exception here
     }
}

void loadFromString() {
     borr::language::deLang;

     namespace fs = std::filesystem;

     try {
          stringstream sStream;
          ifstream ifStream("/path/to/de_DE.borr", deLang);

          sStream << ifStream.rdbuf();

          borr::language::fromString(sStream.str(), deLang);
     } catch (const std::exception& ex) {
          // handle exception here
     }
}
```

### Reading translations
Reading translations is as simple as parsing a borrfile.
You have several options, such as disabling variable expansion.

libborr will not fail if your string was not found, because libborr returns an `std::optional<string>` for the value.

```cpp
void foo(const language& lang) {
     const auto translationWithExpandedVars = lang.getString("section", "field");
     const auto translationWithoutExpandedVars = lang.getString("section", "field", false);

     // do something
     bar(translationWithExpandedVars.value_or("some replacement value"));

     // do something else
     if (!translationWithoutExpandedVars.has_value()) {
          // handle error or something
     }
}
```

### Getting entire sections
If, for whatever reason, you want to get the entire section, this is also possible.
As with individual translations, libborr will "fail" silently, using `std::optional<sect_t>`.

```cpp
void foobar(const language& lang) {
     const auto section = lang.getSection("my_section");

     for (const auto& field : section) {
          field->first; // is the field name
          field->second; // is the actual contents
     }
}
```