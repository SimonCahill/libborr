# libborr

**!!! CURRENTLY WORK IN PROGRESS !!!**

libborr is a language file parsing library which allows you to easily translate your C++ application to any language(s) you might need to!
libboor uses regular expressions to efficiently and easily parse .borr (or .lang) files with their special structure to ensure you always know which language
you're selecting and provides a super simple API to use.

# The API and file documentation

**Please not this is not the real documentation, but an excerpt of the code docs! This will updated shortly!**

This class represents a single language which is parsed from a language file (typically *.borr or *.lang).
A language file is a psuedo-ini format which allows for comments, multi-line strings and variable (string) replacement.

Before a language file switches from being a simple ini file to a language file,
it must meet the following criteria:

In the main scope:
 - A lang_id field must be present, with the language ID (en_EN, en_US, en_AU or what have you)
 - A lang_desc field must be present, although it may be left blank
 - A lang_ver field must be present, containing the language file's version (semantic version: MAJOR.MINOR.REVISION).
Other values in the main scope will be ignored by the parser!

Language files can contain different structures which allow them to be flexible and usable in a variety of domains.
Here is a list of all structures a language file can (or sometimes must) contain for it to be valid.

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
| ${}        | A special variable; used for text replacement.| copyright = "© ${year}"                   |

libborr provides a plethora of features for parsing and using borr files and is extensible by use of modern C++ features, such as lambdas.

Here is an example of using libborr for parsing your language files.

```
// en_GB.borr

# Language file for British English
lang_id = "en_GB"
lang_ver = "v1.0.0"
lang_desc = "British English translations for My Awesome App!"

[start_page]
page_title = "Start Here!"
my_button = "Click me!"

[about_page]
page_title = "About ${app_name}"
about_text[] = "This is my awesome app, written with love!"
about_text[] = "© ${year} Me" # This would be line 2
about_text[] = "Code for \"${start_page:my_button}\" button copied from StackOverflow"

// de_DE.borr

# Language file for British English
lang_id = "de_DE"
lang_ver = "v1.0.0"
lang_desc = "Deutsche Übersetzung für Meine Coole App!"

[start_page]
page_title = "Hier geht's los!"
my_button = "Klick mich!"

[about_page]
page_title = "Über ${app_name}"
about_text[] = "Dies ist meine coole App; mit Liebe geschrieben!"
about_text[] = "© ${year} Me" # This would be line 2
about_text[] = "Code für \"${start_page:my_button}\"-Knopf von StackOverflow kopiert"
```

With those two files, you could call something like this from within your application:

```cpp
void loadLanguages() {
     const auto enLangFile = fs::directory_entry("./languages/en_EN.borr");
     const auto deLangContents = readFile("./languages/de_DE.borr");
     const auto enLang = language::fromFile(enLangFile);
     const auto deLang = language::fromString(deLangContents);
     // do whatever 
     // to get translation
     const auto deStartPageTitle = deLang.getString("start_page", "page_title"):

     const auto deStartButtonText = deLang.getString("start_page", "my_button");
     // do whatever with strings
} 
```

And that's basically the main API. It's rare you'll need much more than that.
